#pragma once

#include <functional>
#include <sys/epoll.h>

class Epoll;

class Channel {
private:
  int fd_;
  bool is_in_epoll_;
  uint32_t events_;
  uint32_t revents_;
  Epoll *ep_;
  /* 设置为fuction方便，可以接收functor和指针 */
  std::function<void()> callback_;

public:
  Channel(Epoll *, int);
  ~Channel();

  void SetInEpoll();
  void SetRevents(uint32_t);
  void Activate(uint32_t);
  void HandleEvent(); /* 调用Callback */
  void SetCallback(std::function<void()>);

  int GetFd() const { return fd_; }
  uint32_t GetEvents() const { return events_; }
  uint32_t GetRevents() const { return revents_; }
  bool IsInEpoll() const { return is_in_epoll_; }
};