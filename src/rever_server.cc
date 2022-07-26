#include "rever_server.h"
#include "rever_acceptor.h"
#include "rever_channel.h"
#include "rever_epoll.h"
#include "rever_handler.h"
#include "rever_inet_address.h"
#include "rever_socket.h"
#include "rever_thread_pool.h"
#include <errno.h>
#include <unistd.h>

void showNewConnection(InetAddress &cli_addr, int sock);

Server::Server()
    : epoll(new Epoll()), acceptor(nullptr), threadpool(new ThreadPool()) {
  acceptor = new Acceptor(epoll);
  std::function<void(int)> cb =
      std::bind(&Server::newConnection, this, std::placeholders::_1);
  acceptor->SetNewConnectionCallback(cb);
}

Server::~Server() {
  delete epoll;
  delete acceptor;
  delete threadpool;
}

void Server::newConnection(int sock) {
  InetAddress cli_addr;
  while (true) {
    /* 针对ET模式下的一次性取出所有待连接的方法 */
    int cli_sock = Socket::accept(&cli_addr, sock);
    if (cli_sock < 0 && errno == EAGAIN)
      break;
    // showNewConnection(cli_addr, cli_sock);
    Socket::setnonblocking(cli_sock);
    Channel *ch = new Channel(epoll, cli_sock);
    std::function<void()> sock_cb =
        std::bind(&Server::handleReadEvent, this, cli_sock);
    std::function<void()> add_cb = std::bind(&Server::addThread, this, sock_cb);
    ch->setCallback(add_cb);
    epoll->addToMap(cli_sock, ch);
    ch->activate(EPOLLIN | EPOLLET | EPOLLRDHUP);
  }
}

void Server::handleReadEvent(int sock) {
  Handler h(sock);
  h.start();
  /* done connection */
  epoll->deleteChannel(sock);
  close(sock);
}

void showNewConnection(InetAddress &cli_addr, int sock) {
  printf("new client fd %d ip: %s port: %d\n", sock,
         inet_ntoa(cli_addr.addr.sin_addr), ntohs(cli_addr.addr.sin_port));
}

void Server::loop() {
  while (true) {
    auto events = epoll->poll();
    int n = events.size();
    for (int i = 0; i < n; i++) {
      events[i]->handleEvent();
    }
  }
}

void Server::addThread(std::function<void()> func) { threadpool->add(func); }