#include "glog/logging.h"
#include "rever_channel.h"
#include "rever_epoll.h"
#include <unistd.h>
#include <cstdio>

Channel::Channel(Epoll *_ep, int _fd)
    : fd_(_fd), 
      is_in_epoll_(false),
      events_(0), 
      revents_(0), 
      ep_(_ep) {}

void Channel::Activate(uint32_t op) {
  events_ = op;
  ep_->UpdateChannel(this);
}

void Channel::SetInEpoll() { is_in_epoll_ = true; }

void Channel::SetRevents(uint32_t _revents) { revents_ = _revents; }

void Channel::HandleEvent() {
  if (this->revents_ & EPOLLRDHUP) {
    printf("EPOLLRDHUP\n");
    LOG(INFO) << "fd: " << fd_ << " closed";
    close(fd_);
  } else {
    callback_();
  }
  // ep_->DeleteChannel(this->fd_);
}


void Channel::SetCallback(std::function<void()> _cb) { callback_ = _cb; }

Channel::~Channel() {}