#pragma once

#include <functional>

class InetAddress;
class Channel;
class Epoll;

class Acceptor {
private:
  int sock;
  Epoll *epoll;
  InetAddress *addr;
  Channel *acceptChannel;

public:
  Acceptor(Epoll *);
  ~Acceptor();
  void acceptConnection();
  void setNewConnectionCallback(std::function<void(int)> cb);

  std::function<void(int)> newConnectionCallback;
};