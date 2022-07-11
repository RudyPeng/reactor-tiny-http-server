#pragma once

#include <functional>
#include <sys/epoll.h>

class Epoll;

class Channel {
private:
  int fd;
  bool inEpoll;
  uint32_t events;
  uint32_t revents;
  Epoll *ep;
  /* 设置为fuction方便，可以接收functor和指针 */
  std::function<void()> callback;

public:
  Channel(Epoll *, int);
  ~Channel();

  void setInEpoll();
  void setRevents(uint32_t);
  void activate(uint32_t);
  void handleEvent(); /* 调用Callback */
  void setCallback(std::function<void()>);

  int getFd() const { return fd; }
  uint32_t getEvents() const { return events; }
  uint32_t getRevents() const { return revents; }
  bool isInEpoll() const { return inEpoll; }
};