#include "Bot.hpp"
#include "CommandRouter.hpp"
#include "Logger.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>

static std::string trim(std::string value) {
  const auto notSpace = [](unsigned char c) { return !std::isspace(c); };
  value.erase(value.begin(),
              std::find_if(value.begin(), value.end(), notSpace));
  value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(),
              value.end());
  return value;
}

static std::string toUpper(std::string value) {
  std::transform(
      value.begin(), value.end(), value.begin(),
      [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return value;
}

static std::optional<Bot::Move> parseMove(const std::string &text) {
  std::string normalized = text;
  std::replace(normalized.begin(), normalized.end(), ',', ' ');
  std::istringstream iss(normalized);

  int x = 0;
  int y = 0;
  if (!(iss >> x >> y))
    return std::nullopt;
  return Bot::Move{x, y};
}

static std::optional<std::tuple<int, int, int>>
parseBoardLine(const std::string &line) {
  std::string normalized = line;
  std::replace(normalized.begin(), normalized.end(), ',', ' ');
  std::istringstream iss(normalized);

  int x = 0;
  int y = 0;
  int player = 0;
  if (!(iss >> x >> y >> player))
    return std::nullopt;
  return std::make_tuple(x, y, player);
}

static bool truthyEnv(const char *value) {
  if (!value)
    return false;
  const std::string v = toUpper(trim(value));
  return v == "1" || v == "TRUE" || v == "YES" || v == "ON";
}

static void configureLoggerFromEnvAndArgs(int argc, char **argv) {
  auto &logger = Logger::instance();

  const char *envLog = std::getenv("GOMOKU_LOG");
  if (envLog && *envLog) {
    (void)logger.setLogFile(envLog);
  } else if (truthyEnv(std::getenv("GOMOKU_DEBUG"))) {
    logger.enableStderr(true);
  }

  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);
    if (arg == "--debug") {
      logger.enableStderr(true);
      continue;
    }
    if (arg == "--no-debug") {
      logger.disable();
      continue;
    }
    if (arg == "--log" && i + 1 < argc) {
      (void)logger.setLogFile(argv[i + 1]);
      ++i;
      continue;
    }
  }
}

static void reply(const std::string &message) {
  Logger::instance().log("<< " + message);
  std::cout << message << std::endl;
}

int main(int argc, char **argv) {
  std::cout.setf(std::ios::unitbuf);
  configureLoggerFromEnvAndArgs(argc, argv);

  Bot bot;
  CommandRouter router;
  bool running = true;

  router.registerHandler("ABOUT", [](const std::string &) {
    reply("name=\"pbrain-gomoku-ai\", version=\"0.1\", author=\"gomoku\", "
          "country=\"FR\"");
  });

  router.registerHandler("END", [&](const std::string &) { running = false; });

  router.registerHandler("INFO", [](const std::string &args) {
    std::istringstream iss(args);
    std::string key;
    std::string value;
    if (iss >> key >> value) {
      const std::string upperKey = toUpper(key);
      if (upperKey == "DEBUG") {
        Logger::instance().enableStderr(truthyEnv(value.c_str()));
      } else if (upperKey == "LOG" || upperKey == "LOGFILE") {
        (void)Logger::instance().setLogFile(value);
      }
    }
  });

  router.registerHandler("START", [&](const std::string &args) {
    int size = 0;
    std::istringstream iss(args);
    if ((iss >> size) && bot.start(size))
      reply("OK");
    else
      reply("ERROR");
  });

  router.registerHandler("RESTART", [&](const std::string &) {
    if (bot.restart())
      reply("OK");
    else
      reply("ERROR");
  });

  router.registerHandler("TAKEBACK", [&](const std::string &args) {
    const auto move = parseMove(args);
    if (move && bot.takeback(*move))
      reply("OK");
    else
      reply("ERROR");
  });

  router.registerHandler("BEGIN", [&](const std::string &) {
    const auto move = bot.chooseMove();
    if (!move || !bot.applyOurMove(*move)) {
      reply("ERROR");
      return;
    }
    reply(std::to_string(move->first) + "," + std::to_string(move->second));
  });

  router.registerHandler("TURN", [&](const std::string &args) {
    const auto opponentMove = parseMove(args);
    if (!opponentMove || !bot.applyOpponentMove(*opponentMove)) {
      reply("ERROR");
      return;
    }

    const auto move = bot.chooseMove();
    if (!move || !bot.applyOurMove(*move)) {
      reply("ERROR");
      return;
    }
    reply(std::to_string(move->first) + "," + std::to_string(move->second));
  });

  router.registerHandler("BOARD", [&](const std::string &) {
    if (!bot.restart()) {
      reply("ERROR");
      return;
    }

    std::string line;
    while (std::getline(std::cin, line)) {
      line = trim(line);
      Logger::instance().log(">> " + line);
      if (toUpper(line) == "DONE")
        break;
      const auto entry = parseBoardLine(line);
      if (!entry) {
        reply("ERROR");
        return;
      }
      const auto [x, y, player] = *entry;
      if (!bot.applyBoardMove({x, y}, player)) {
        reply("ERROR");
        return;
      }
    }

    {
      const auto move = bot.chooseMove();
      if (!move || !bot.applyOurMove(*move)) {
        reply("ERROR");
        return;
      }
      reply(std::to_string(move->first) + "," + std::to_string(move->second));
    }
  });

  std::string line;
  while (running && std::getline(std::cin, line)) {
    router.process(line);
  }

  return 0;
}
