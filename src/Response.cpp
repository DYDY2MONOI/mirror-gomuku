#include "Response.hpp"

void Response::send(const std::string &response)
{
    Logger::instance().log("<< " + response);
    std::cout << response << std::endl;
}

void Response::move(int x, int y)
{
    send(std::to_string(x) + "," + std::to_string(y));
}

void Response::move(const std::pair<int, int> &coord)
{
    move(coord.first, coord.second);
}

void Response::ok()
{
    send("OK");
}

void Response::error()
{
    send("ERROR");
}

void Response::error(const std::string &message)
{
    send("ERROR " + message);
}

void Response::unknown()
{
    send("UNKNOWN");
}

void Response::debug(const std::string &message)
{
    send("DEBUG " + message);
}

void Response::message(const std::string &message)
{
    send("MESSAGE " + message);
}

void Response::about(const std::string &name, const std::string &version,
                     const std::string &author, const std::string &country)
{
    send("name=\"" + name + "\", version=\"" + version + "\", author=\"" +
         author + "\", country=\"" + country + "\"");
}

void Response::raw(const std::string &response)
{
    send(response);
}
