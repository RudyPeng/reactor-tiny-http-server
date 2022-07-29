#include "glog/logging.h"
#include "rever_acceptor.h"
#include "rever_channel.h"
#include "rever_inet_address.h"
#include "rever_server.h"
#include "rever_socket.h"

Acceptor::Acceptor(Epoll *_epoll) : epoll_(_epoll) {
  InetAddress *address = new InetAddress(INADDR_ANY, 9999);
  sock_ = Socket::Create();
  Socket::Bind(address, sock_);
  Socket::Listen(sock_);
  LOG(INFO) << "acceptor bind to fd: " << sock_;
  Socket::SetNonBlocking(sock_);
  accept_channel_ = new Channel(epoll_, sock_);
  std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
  accept_channel_->SetCallback(cb);
  accept_channel_->Activate(EPOLLIN | EPOLLET);
}

Acceptor::~Acceptor() {
  delete addr_;
  delete accept_channel_;
}

void Acceptor::AcceptConnection() { new_connection_callback_(sock_); }

void Acceptor::SetNewConnectionCallback(std::function<void(int)> _cb) {
  new_connection_callback_ = _cb;
}