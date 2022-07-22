#include "Channel.h"
#include "Epoll.h"
#include "Handler.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define READ_BUFFER 1024

void showNewConnection(InetAddress &ia, int sock);

// 
// old main
//
// int main() {
//   InetAddress address(INADDR_ANY, 0);
//   int sock = Socket::create();
//   Socket::bind(&address, sock);
//   Socket::listen(sock);
//   Epoll *epoll = new Epoll();
//   Socket::setnonblocking(sock);
//   Channel *serv_ch = new Channel(epoll, sock);
//   serv_ch->activate(EPOLLIN | EPOLLRDHUP);
//   InetAddress cli_addr; /* 对端地址信息的临时存储 */
//   while (true) {
//     auto events = epoll->poll();
//     int n = events.size();
//     for (int i = 0; i < n; i++) {
//       int fd = events[i]->getFd();
//       if (fd == sock) { /* new connection */
//         int cli_sock = Socket::accept(&cli_addr, sock);
//         showNewConnection(cli_addr, cli_sock);
//         Socket::setnonblocking(cli_sock);
//         Channel *ch = new Channel(epoll, cli_sock);
//         ch->activate(EPOLLIN | EPOLLET);
//       } else if (events[i]->getRevents() & EPOLLRDHUP) {
//         close(fd);
//       } else if (events[i]->getRevents() & EPOLLIN) {
//         Handler h(fd);
//         h.start();
//       }
//     }
//   }
//   delete epoll;
//   close(sock);
//   return 0;
// }

// void showNewConnection(InetAddress &cli_addr, int sock) {
//   printf("new client fd %d ip: %s port: %d\n", sock,
//          inet_ntoa(cli_addr.addr.sin_addr), ntohs(cli_addr.addr.sin_port));
// }
