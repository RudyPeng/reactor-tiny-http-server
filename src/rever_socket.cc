#include "rever_socket.h"
#include <fcntl.h>
#include "rever_inet_address.h"
#include "util.h"

int Socket::Create() {
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  ERRIF(sock == -1, "socket(): ");
  LOG(INFO) << "socket create fd " << sock;
  return sock;
}

void Socket::Bind(InetAddress* ia, int fd) {
  bool flag = ia->addr.sin_port == 0;
  ERRIF(::bind(fd, (struct sockaddr*)&ia->addr, ia->addr_len) == -1,
        "bind(): ");
  /* 在以端口号为0调用bind后，使用getsockname返回内核分配的本地端口号； */
  if (flag) {
    socklen_t len = sizeof(struct sockaddr_in);
    ERRIF(getsockname(fd, (struct sockaddr*)&ia->addr, &len) == -1,
          "getsockname(): ");
  }
  printf("httpd running on port %d\n", ntohs(ia->addr.sin_port));
  LOG(INFO) << "httpd running on port " << ntohs(ia->addr.sin_port);
}

void Socket::Listen(int fd, int backlog) {
  ERRIF(::listen(fd, backlog) == -1, "listen(): ");
  LOG(INFO) << fd << " is listening";
}

int Socket::Accept(InetAddress* ia, int fd) {
  int clnt_sock = ::accept(fd, (struct sockaddr*)&ia->addr, &ia->addr_len);
  return clnt_sock;
}

void Socket::SetNonBlocking(int fd) {
  LOG(INFO) << "set fd " << fd << " to non blocing";
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}