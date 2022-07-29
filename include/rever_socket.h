#pragma once

#include <sys/socket.h>

class InetAddress;

/* 将socket抽象成一个工具类,只作为一个中转站作用 */
class Socket {
public:
  static int Create();
  static void Bind(InetAddress *, int);
  static void Listen(int, int backlog = SOMAXCONN);
  static int Accept(InetAddress *, int);
  static void SetNonBlocking(int);
};