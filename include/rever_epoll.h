#pragma once
#include <sys/epoll.h>
#include <unordered_map>
#include <vector>

#define MAX_EVENTS 1000

class Channel;

class Epoll {
private:
  int epfd;
  std::unordered_map<int, Channel *> fd_channel;
  /* 保存到来的事件的数组 */
  struct epoll_event events[MAX_EVENTS];

public:
  Epoll();
  ~Epoll();

  void addFd(int fd, uint32_t op);
  void updateChannel(Channel *);
  void deleteChannel(int); /* 根据fd删除对应的channel */
  void addToMap(int fd, Channel *);
  std::vector<Channel *> poll(int timeout = -1);
};