#pragma once

#include <functional>

class Epoll;
class Acceptor;
class ThreadPool;
class Server {
private:
  Epoll *epoll_;
  Acceptor *acceptor_;
  ThreadPool *threadpool_;

public:
  Server();
  ~Server();

  void Loop();
  void HandleReadEvent(int);
  void NewConnection(int);
  void AddThread(std::function<void(void)>);
};