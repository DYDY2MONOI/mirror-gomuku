#ifndef COMMAND_ROUTER_HPP_
#define COMMAND_ROUTER_HPP_

#include <functional>
#include <map>
#include <string>

class CommandRouter {
public:
  using Handler = std::function<void(const std::string &args)>;

  void registerHandler(const std::string &command, Handler handler);
  void process(const std::string &line);

private:
  std::map<std::string, Handler> handlers_;
};

#endif // COMMAND_ROUTER_HPP_
