#pragma once

#include <fstream>
#include <iostream>
#include <optional>
#include <string>

class Logger {
public:
    static Logger &instance();

    void enableStderr(bool enabled);
    bool setLogFile(const std::string &path);
    void disable();

    bool enabled() const;
    void log(const std::string &message);

private:
    Logger() = default;

    std::ostream *out_ = nullptr;
    std::optional<std::ofstream> file_;
};
