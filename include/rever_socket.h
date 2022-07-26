#pragma once

#include <sys/socket.h>

class InetAddress;

/* 将socket抽象成一个工具类,只作为一个中转站作用 */
class Socket {
public:
  static int create();
  static void bind(InetAddress *, int);
  static void listen(int, int backlog = SOMAXCONN);
  static int accept(InetAddress *, int);
  static void setnonblocking(int);
};