#pragma once

#include <functional>

class Epoll;
class Acceptor;
class ThreadPool;
class Server {
private:
  Epoll *epoll;
  Acceptor *acceptor;
  ThreadPool *threadpool;

public:
  Server();
  ~Server();

  void loop();
  void handleReadEvent(int);
  void newConnection(int);
  void addThread(std::function<void(void)>);
};