#include "rever_server.h"
#include <errno.h>
#include <unistd.h>
#include "rever_acceptor.h"
#include "rever_channel.h"
#include "rever_epoll.h"
#include "rever_handler.h"
#include "rever_inet_address.h"
#include "rever_socket.h"
#include "rever_thread_pool.h"

void ShowNewConnection(InetAddress& cli_addr, int sock);

Server::Server()
    : epoll_(new Epoll()), acceptor_(nullptr), threadpool_(new ThreadPool()) {
  acceptor_ = new Acceptor(epoll_);
  std::function<void(int)> cb =
      std::bind(&Server::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(cb);
}

Server::~Server() {
  delete epoll_;
  delete acceptor_;
  delete threadpool_;
}

void Server::NewConnection(int sock) {
  InetAddress cli_addr;
  while (true) {
    /* 针对ET模式下的一次性取出所有待连接的方法 */
    int cli_sock = Socket::Accept(&cli_addr, sock);
    if (cli_sock < 0 && errno == EAGAIN)
      break;
    ShowNewConnection(cli_addr, cli_sock);
    Socket::SetNonBlocking(cli_sock);
    Channel* ch = new Channel(epoll_, cli_sock);
    std::function<void()> sock_cb =
        std::bind(&Server::HandleReadEvent, this, cli_sock);
    std::function<void()> add_cb = std::bind(&Server::AddThread, this, sock_cb);
    ch->SetCallback(add_cb);
    epoll_->AddToMap(cli_sock, ch);
    ch->Activate(EPOLLIN | EPOLLET | EPOLLRDHUP);
  }
}

void Server::HandleReadEvent(int sock) {
  Handler h(sock);
  h.Start();
  /* done connection */
  epoll_->DeleteChannel(sock);
  close(sock);
}

void ShowNewConnection(InetAddress& cli_addr, int sock) {
  LOG(INFO) << "new client fd " << sock
            << " ip: " << inet_ntoa(cli_addr.addr.sin_addr)
            << " port: " << ntohs(cli_addr.addr.sin_port);
}

void Server::Loop() {
  while (true) {
    auto events = epoll_->poll();
    int n = events.size();
    for (int i = 0; i < n; i++) {
      events[i]->HandleEvent();
    }
  }
}

void Server::AddThread(std::function<void()> func) {
  threadpool_->Add(func);
}