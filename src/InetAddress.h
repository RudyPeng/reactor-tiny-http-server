#pragma once

#include <arpa/inet.h>

struct InetAddress {

  struct sockaddr_in addr;
  socklen_t addr_len;

  /* C++11特性的委托构造函数 */
  InetAddress();
  InetAddress(const char *, int);
  InetAddress(in_addr_t, int);
  InetAddress(const InetAddress &) = delete;
  ~InetAddress(){};
};