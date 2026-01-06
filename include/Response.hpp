#pragma once

#include <iostream>
#include <string>
#include <utility>

#include "Logger.hpp"

/**
 * @brief Centralized response formatting for Gomoku bot protocol compliance.
 *
 * This class provides static methods for all bot responses, ensuring strict
 * adherence to the Piskvork communication protocol format.
 *
 * Output Format Specifications:
 * - Coordinates: "X,Y" (no spaces, comma-separated)
 * - Status responses: "OK", "ERROR", "UNKNOWN"
 * - Debug messages: "DEBUG <message>" or "MESSAGE <message>"
 * - About info: key="value" pairs, comma-separated
 */
class Response {
public:
    /**
     * @brief Respond with a move at the given coordinates.
     * Format: "X,Y" where X and Y are non-negative integers.
     */
    static void move(int x, int y);
    static void move(const std::pair<int, int> &coord);

    /**
     * @brief Respond with "OK" to indicate success.
     */
    static void ok();

    /**
     * @brief Respond with "ERROR" to indicate failure.
     * Optionally followed by an error message.
     */
    static void error();
    static void error(const std::string &message);

    /**
     * @brief Respond with "UNKNOWN" for unrecognized commands.
     */
    static void unknown();

    /**
     * @brief Send a debug message to the game manager.
     * Format: "DEBUG <message>"
     */
    static void debug(const std::string &message);

    /**
     * @brief Send an informational message to the game manager.
     * Format: "MESSAGE <message>"
     */
    static void message(const std::string &message);

    /**
     * @brief Send the ABOUT response with bot metadata.
     * Format: name="value", version="value", author="value", country="value"
     */
    static void about(const std::string &name, const std::string &version,
                      const std::string &author, const std::string &country);

    /**
     * @brief Send a raw response (for custom responses).
     * Use sparingly - prefer specific methods for protocol compliance.
     */
    static void raw(const std::string &response);

private:
    /**
     * @brief Internal method to send a response and log it.
     */
    static void send(const std::string &response);
};
