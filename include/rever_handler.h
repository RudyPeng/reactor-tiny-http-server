#pragma once
#include "util.h"
#include <condition_variable>
#include <mutex>
#include <string>
#include <sys/stat.h>

void Unimplemented(int client);
void NotFound(int client);
void CannotExecute(int client);
void SendHeader(int client);
void BadRequest(int client);

class Epoll;
class Handler {
private:
  int fd_;
  std::string buf_;
  std::string query_string_;
  bool cgi_ = false;
  // std::mutex mtx;
  // std::condition_variable cv;

private:
  int GetRequestHeader();
  void SendFile();    /* 返回静态文件 */
  void Execute();     /* 执行cgi动态解析 */
  void Cat(FILE *fp); /* 发送文件 */
  int Peek();         /* v2.窥探数据，以防是keep-alive的fin报文 */

public:
  Handler(int); /* 接收一个fd */
  ~Handler();

  Handler(const Handler &) = delete;
  Handler &operator=(const Handler &) = delete;

  int GetLine();
  void Start();       /* 处理请求的入口 */
  bool IsImpl();      /* 是否能够处理请求 */
  void FileProcess(); /* 文件处理 */
  void ReadAll();     /* 读取剩下所有 */

// TODO may change to protected
public:
  /* HTTP请求头信息 */
  std::string method_;   /* 记录请求的方法 */
  std::string url_;      /* 记录请求的文件路径 */
  std::string protocol_; /* 记录请求的HTTP协议 */
};
