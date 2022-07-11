#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <fcntl.h>

int Socket::create() {
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  errIf(sock == -1, "socket(): ");
  return sock;
}

void Socket::bind(InetAddress *ia, int fd) {
  bool flag = ia->addr.sin_port == 0;
  errIf(::bind(fd, (struct sockaddr *)&ia->addr, ia->addr_len) == -1,
        "bind(): ");
  /* 在以端口号为0调用bind后，使用getsockname返回内核分配的本地端口号； */
  if (flag) {
    socklen_t len = sizeof(struct sockaddr_in);
    errIf(getsockname(fd, (struct sockaddr *)&ia->addr, &len) == -1,
          "getsockname(): ");
  }
  printf("httpd running on port %d\n", ntohs(ia->addr.sin_port));
}

void Socket::listen(int fd, int backlog) {
  errIf(::listen(fd, backlog) == -1, "listen(): ");
}

int Socket::accept(InetAddress *ia, int fd) {
  int clnt_sock = ::accept(fd, (struct sockaddr *)&ia->addr, &ia->addr_len);
  return clnt_sock;
}

void Socket::setnonblocking(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}