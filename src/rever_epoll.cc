#include "rever_epoll.h"
#include "rever_channel.h"
#include "util.h"
#include <arpa/inet.h>
#include <cstring>
#include <mutex>
#include <unistd.h>

static std::mutex mtx;

Epoll::Epoll() : epfd_(-1) {
  epfd_ = epoll_create1(0);
  ERRIF(epfd_ == -1, "epoll_create(): ");
  LOG(INFO) << "epoll create, epfd: " << epfd_;
  for (int i = 0; i < MAX_EVENTS; ++i) {
    bzero(&events_[i], sizeof(struct epoll_event));
  }
}

Epoll::~Epoll() {
  if (epfd_ != -1) {
    close(epfd_);
    LOG(INFO) << "epfd: " << epfd_ << " closed";
    epfd_ = -1;
  }
}

void Epoll::AddFd(int fd, uint32_t op) {
  struct epoll_event ev;
  ev.data.fd = fd;
  ev.events = op;
  ERRIF(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl: ");
  LOG(INFO) << "fd: " << fd << " add to epoll";
}

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> vec;
  int n = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
  LOG(INFO) << "nuber of incoming requests: " << n;
  ERRIF(n == -1, "epoll_wait(): ");
  for (int i = 0; i < n; ++i) {
    Channel *ch = (Channel *)events_[i].data.ptr;
    ch->SetRevents(events_[i].events);
    vec.push_back(ch);
  }
  return vec;
}

void Epoll::UpdateChannel(Channel *ch) {
  /* 挂树上 */
  int fd = ch->GetFd();
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = ch;
  ev.events= ch->GetEvents();
  if (!ch->IsInEpoll()) {
    ch->SetInEpoll();
    LOG(INFO) << "epoll_ctl: " << fd << " add to epoll";
    ERRIF(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl: ");
  } else {
    ERRIF(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll_ctl modify: ");
  }
}

void Epoll::DeleteChannel(int fd) {
  {
    std::unique_lock<std::mutex> lock(mtx);
    delete fd_channel_[fd];
    fd_channel_.erase(fd);
    LOG(INFO) << "channel: " << fd << " deleted";
  }
}

void Epoll::AddToMap(int fd, Channel *ch) {
  {
    std::unique_lock<std::mutex> lock(mtx);
    fd_channel_[fd] = ch;
    LOG(INFO) << "channel: " << fd << " add to map";
  }
}