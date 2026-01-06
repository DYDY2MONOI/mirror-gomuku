#include "Logger.hpp"

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::enableStderr(bool enabled)
{
    if (!enabled) {
        disable();
        return;
    }
    file_.reset();
    out_ = &std::cerr;
}

bool Logger::setLogFile(const std::string &path)
{
    std::ofstream stream(path, std::ios::app);
    if (!stream.is_open())
        return false;
    file_.emplace(std::move(stream));
    out_ = &(*file_);
    return true;
}

void Logger::disable()
{
    out_ = nullptr;
    file_.reset();
}

bool Logger::enabled() const
{
    return out_ != nullptr;
}

void Logger::log(const std::string &message)
{
    if (!out_)
        return;
    (*out_) << message << '\n';
    out_->flush();
}
