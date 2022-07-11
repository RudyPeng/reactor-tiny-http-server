#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include <arpa/inet.h>
#include <cstring>
#include <mutex>
#include <unistd.h>

static std::mutex mtx;

Epoll::Epoll() : epfd(-1) {
  epfd = epoll_create1(0);
  errIf(epfd == -1, "epoll_create(): ");
  for (int i = 0; i < MAX_EVENTS; ++i) {
    bzero(&events[i], sizeof(struct epoll_event));
  }
}

Epoll::~Epoll() {
  if (epfd != -1) {
    close(epfd);
    epfd = -1;
  }
}

void Epoll::addFd(int fd, uint32_t op) {
  struct epoll_event ev;
  ev.data.fd = fd;
  ev.events = op;
  errIf(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl: ");
}

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> vec;
  int n = epoll_wait(epfd, events, MAX_EVENTS, timeout);
  errIf(n == -1, "epoll_wait(): ");
  for (int i = 0; i < n; ++i) {
    Channel *ch = (Channel *)events[i].data.ptr;
    ch->setRevents(events[i].events);
    vec.push_back(ch);
  }
  return vec;
}

void Epoll::updateChannel(Channel *ch) {
  /* 挂树上 */
  int fd = ch->getFd();
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = ch;
  ev.events = ch->getEvents();
  if (!ch->isInEpoll()) {
    ch->setInEpoll();
    errIf(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl: ");
  } else {
    errIf(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll_ctl modify: ");
  }
}

void Epoll::deleteChannel(int fd) {
  {
    std::unique_lock<std::mutex> lock(mtx);
    delete fd_channel[fd];
    fd_channel.erase(fd);
  }
}

void Epoll::addToMap(int fd, Channel *ch) {
  {
    std::unique_lock<std::mutex> lock(mtx);
    fd_channel[fd] = ch;
  }
}