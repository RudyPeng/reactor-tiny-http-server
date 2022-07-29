#include "rever_server.h"
#include "util.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <glog/logging.h>

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "server started";
  Server server;
  server.Loop();
}

/*
 * v1. 将LT模式的Acceptor修改为ET模式，accept中添加了循环判断是否一次性取完所有的新连接
 * v2. 修复了tcp-keepalive报文在四次挥手发出FIN报文时没有正确处理的BUG
 * v3. 添加了线程池，解决了channle内存泄漏
 * v4. 将execute_cgi的进程版改为了线程版(改动失败、维持原版)
 */