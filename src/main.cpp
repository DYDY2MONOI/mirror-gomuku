#include "Bot.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>

static std::string trim(std::string value)
{
    const auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

static std::string toUpper(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

static std::pair<std::string, std::string> splitCommand(const std::string &line)
{
    const auto pos = line.find_first_of(" \t");
    if (pos == std::string::npos)
        return {line, ""};
    return {line.substr(0, pos), trim(line.substr(pos + 1))};
}

static std::optional<Bot::Move> parseMove(const std::string &text)
{
    std::string normalized = text;
    std::replace(normalized.begin(), normalized.end(), ',', ' ');
    std::istringstream iss(normalized);

    int x = 0;
    int y = 0;
    if (!(iss >> x >> y))
        return std::nullopt;
    return Bot::Move{x, y};
}

static std::optional<std::tuple<int, int, int>> parseBoardLine(const std::string &line)
{
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

int main()
{
    std::cout.setf(std::ios::unitbuf);

    Bot bot;
    std::string line;
    while (std::getline(std::cin, line)) {
        line = trim(line);
        if (line.empty())
            continue;

        const auto [rawCommand, args] = splitCommand(line);
        const std::string command = toUpper(rawCommand);

        if (command == "ABOUT") {
            std::cout << "name=\"pbrain-gomoku-ai\", version=\"0.1\", author=\"gomoku\", country=\"FR\"" << std::endl;
            continue;
        }

        if (command == "END") {
            break;
        }

        if (command == "INFO") {
            continue;
        }

        if (command == "START") {
            int size = 0;
            std::istringstream iss(args);
            if ((iss >> size) && bot.start(size))
                std::cout << "OK" << std::endl;
            else
                std::cout << "ERROR" << std::endl;
            continue;
        }

        if (command == "RESTART") {
            if (bot.restart())
                std::cout << "OK" << std::endl;
            else
                std::cout << "ERROR" << std::endl;
            continue;
        }

        if (command == "TAKEBACK") {
            const auto move = parseMove(args);
            if (move && bot.takeback(*move))
                std::cout << "OK" << std::endl;
            else
                std::cout << "ERROR" << std::endl;
            continue;
        }

        if (command == "BEGIN") {
            const auto move = bot.chooseMove();
            if (!move || !bot.applyOurMove(*move)) {
                std::cout << "ERROR" << std::endl;
                continue;
            }
            std::cout << move->first << "," << move->second << std::endl;
            continue;
        }

        if (command == "TURN") {
            const auto opponentMove = parseMove(args);
            if (!opponentMove || !bot.applyOpponentMove(*opponentMove)) {
                std::cout << "ERROR" << std::endl;
                continue;
            }

            const auto move = bot.chooseMove();
            if (!move || !bot.applyOurMove(*move)) {
                std::cout << "ERROR" << std::endl;
                continue;
            }
            std::cout << move->first << "," << move->second << std::endl;
            continue;
        }

        if (command == "BOARD") {
            if (!bot.restart()) {
                std::cout << "ERROR" << std::endl;
                continue;
            }

            while (std::getline(std::cin, line)) {
                line = trim(line);
                if (toUpper(line) == "DONE")
                    break;
                const auto entry = parseBoardLine(line);
                if (!entry) {
                    std::cout << "ERROR" << std::endl;
                    goto next_command;
                }
                const auto [x, y, player] = *entry;
                if (!bot.applyBoardMove({x, y}, player)) {
                    std::cout << "ERROR" << std::endl;
                    goto next_command;
                }
            }

            {
                const auto move = bot.chooseMove();
                if (!move || !bot.applyOurMove(*move)) {
                    std::cout << "ERROR" << std::endl;
                    continue;
                }
                std::cout << move->first << "," << move->second << std::endl;
            }
        next_command:
            continue;
        }

        std::cout << "UNKNOWN" << std::endl;
    }

    return 0;
}
