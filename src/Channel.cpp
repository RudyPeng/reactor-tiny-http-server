#include "Channel.h"
#include "Epoll.h"
#include <unistd.h>

Channel::Channel(Epoll *_ep, int _fd)
    : fd(_fd), inEpoll(false), events(0), revents(0), ep(_ep) {}

void Channel::activate(uint32_t op) {
  events = op;
  ep->updateChannel(this);
}

void Channel::setInEpoll() { inEpoll = true; }

void Channel::setRevents(uint32_t _revents) { revents = _revents; }

void Channel::handleEvent() {
  if (this->revents & EPOLLRDHUP) {
    printf("EPOLLRDHUP\n");
    close(fd);
  } else {
    callback();
  }
  // ep->deleteChannel(this->fd);
}


void Channel::setCallback(std::function<void()> _cb) { callback = _cb; }

Channel::~Channel() {}