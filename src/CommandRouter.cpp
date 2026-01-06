#include "CommandRouter.hpp"
#include "Logger.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>

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

static std::pair<std::string, std::string>
splitCommand(const std::string &line) {
  const auto pos = line.find_first_of(" \t");
  if (pos == std::string::npos)
    return {line, ""};
  return {line.substr(0, pos), trim(line.substr(pos + 1))};
}

void CommandRouter::registerHandler(const std::string &command,
                                    Handler handler) {
  handlers_[toUpper(command)] = std::move(handler);
}

void CommandRouter::process(const std::string &line) {
  std::string trimmedLine = trim(line);
  if (trimmedLine.empty())
    return;

  Logger::instance().log(">> " + trimmedLine);

  const auto [rawCommand, args] = splitCommand(trimmedLine);
  const std::string command = toUpper(rawCommand);

  if (auto it = handlers_.find(command); it != handlers_.end()) {
    it->second(args);
  } else {
    Logger::instance().log("<< UNKNOWN");
    std::cout << "UNKNOWN" << std::endl;
  }
}
