#pragma once
#include "util.h"
#include <condition_variable>
#include <mutex>
#include <string>
#include <sys/stat.h>

void unimplemented(int client);
void not_found(int client);
void cannot_execute(int client);
void send_header(int client);
void bad_request(int client);

class Epoll;
class Handler {
private:
  int fd;
  std::string buf;
  std::string query_string;
  bool cgi = false;
  // std::mutex mtx;
  // std::condition_variable cv;

private:
  int getRequestHeader();
  void sendFile();    /* 返回静态文件 */
  void execute();     /* 执行cgi动态解析 */
  void cat(FILE *fp); /* 发送文件 */
  int peek();         /* v2.窥探数据，以防是keep-alive的fin报文 */

public:
  Handler(int); /* 接收一个fd */
  ~Handler();

  Handler(const Handler &) = delete;
  Handler &operator=(const Handler &) = delete;

  int getline();
  void start();       /* 处理请求的入口 */
  bool isImpl();      /* 是否能够处理请求 */
  void fileProcess(); /* 文件处理 */
  void readAll();     /* 读取剩下所有 */

public:
  /* HTTP请求头信息 */
  std::string method;   /* 记录请求的方法 */
  std::string url;      /* 记录请求的文件路径 */
  std::string protocol; /* 记录请求的HTTP协议 */
};
