#include "Acceptor.h"
#include "Channel.h"
#include "InetAddress.h"
#include "Server.h"
#include "Socket.h"

Acceptor::Acceptor(Epoll *_epoll) : epoll(_epoll) {
  InetAddress *address = new InetAddress(INADDR_ANY, 9999);
  this->sock = Socket::create();
  Socket::bind(address, sock);
  Socket::listen(sock);
  Socket::setnonblocking(sock);
  acceptChannel = new Channel(epoll, sock);
  std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
  acceptChannel->setCallback(cb);
  acceptChannel->activate(EPOLLIN | EPOLLET);
}

Acceptor::~Acceptor() {
  delete addr;
  delete acceptChannel;
}

void Acceptor::acceptConnection() { newConnectionCallback(sock); }

void Acceptor::setNewConnectionCallback(std::function<void(int)> _cb) {
  newConnectionCallback = _cb;
}