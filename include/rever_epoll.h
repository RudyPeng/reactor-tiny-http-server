#pragma once
#include <sys/epoll.h>
#include <unordered_map>
#include <vector>

#define MAX_EVENTS 1000

class Channel;

class Epoll {
private:
  int epfd_;
  std::unordered_map<int, Channel *> fd_channel_;
  /* 保存到来的事件的数组 */
  // TODO may need use heap allocate since stack allocate may cause stack overflow
  struct epoll_event events_[MAX_EVENTS];

public:
  Epoll();
  ~Epoll();

  void AddFd(int fd, uint32_t op);
  void UpdateChannel(Channel *);
  void DeleteChannel(int); /* 根据fd删除对应的channel */
  void AddToMap(int fd, Channel *);
  std::vector<Channel *> poll(int timeout = -1);
};