#include "InetAddress.h"
#include <cstring>

InetAddress::InetAddress() : addr_len(sizeof(addr)) {
  memset(&addr, 0, sizeof(addr));
}

InetAddress::InetAddress(const char *ip, int port) : InetAddress() {
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
}

InetAddress::InetAddress(in_addr_t add, int port) : InetAddress() {
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = htonl(add);
  addr.sin_port = htons(port);
}