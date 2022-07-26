#pragma once

#include <functional>

class InetAddress;
class Channel;
class Epoll;

class Acceptor {
private:
  int sock_;
  Epoll *epoll_;
  InetAddress *addr_;
  Channel *accept_channel_;

public:
  Acceptor(Epoll *);
  ~Acceptor();
  void AcceptConnection();
  void SetNewConnectionCallback(std::function<void(int)> cb);

  std::function<void(int)> new_connection_callback_;
};