#include "Handler.h"
#include "Epoll.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#define DEBUG 1
#define DEBUGLOG(x, param)                                                     \
  do {                                                                         \
    if (DEBUG) {                                                               \
      printf(x, param);                                                        \
      fflush(stdout);                                                          \
    }                                                                          \
  } while (0);

void Handler::readAll() {
  char c = '\0';
  while (true) {
    int n = recv(fd, &c, 1, 0);
    if (n > 0)
      buf.push_back(c);
    else if (n = -1 && errno == EAGAIN) {
      break;
    } else if (n == 0) {
      close(fd);
      break;
    }
  }
}

Handler::Handler(int _fd) : fd(_fd), buf(""), query_string("") {}

Handler::~Handler() {
  // if (fd != -1) {
  //   // printf("client fd: %d disconnected\n", fd);
  //   close(fd);
  // }
}

int Handler::getline() {
  buf.clear();
  char c = '\0';
  while (c != '\n') {
    int n = recv(fd, &c, 1, 0);
    if (n > 0) {
      if (c == '\r') {
        n = recv(fd, &c, 1, MSG_PEEK);
        if (n > 0 && c == '\n')
          recv(fd, &c, 1, 0);
        else
          c = '\n';
      }
      buf.push_back(c);
    } else
      c = '\n';
  }
  return buf.size();
}

int Handler::getRequestHeader() {
  int n = this->getline();
  // ERRIF(n == 0, "Empty Message: ");
  if (n == 0)
    return 0;
  errIf(n == -1, "Error Connection: ");
  std::istringstream is(buf);
  is >> method;
  is >> url;
  is >> protocol;
  if (method == "GET") {
    int pos = -1;
    if ((pos = url.find('?')) != std::string::npos) {
      query_string = url.substr(pos);
      url.erase(pos);
      cgi = true;
    }
  }
  return n;
}

/* 目前仅处理GET AND POST */
bool Handler::isImpl() {
  if (method != "GET" && method != "POST") {
    unimplemented(fd);
    return false;
  }
  return true;
}

/* 处理url */
void Handler::fileProcess() {
  url = "../htdocs" + url;
  /* 如果是/表示请求根目录，则重定位至index.html */
  if (url[url.size() - 1] == '/') {
    url += "index.html";
  }
  struct stat st;
  if (stat(url.c_str(), &st) == -1) {
    /*
     * -1表示文件不存在
     * 每个TCP连接在内核中都有缓冲区(窗口)，
     * 读取完这些缓冲区并丢弃
     */
    readAll();
    not_found(fd);
  } else {
    /* 存在 */
    if (S_ISDIR(st.st_mode)) {
      url += "/index.html";
    }
    if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) ||
        (st.st_mode & S_IXOTH)) {
      /* 若文件可执行 */
      cgi = true;
    }
    if (cgi)
      execute();
    else
      sendFile();
  }
}

void Handler::sendFile() {
  FILE *fp = nullptr;
  int n = 1;
  // while (n > 0 && buf != "\n") {
  //   /* 读取完所有的请求头信息 */
  //   n = getline();
  // }
  readAll();
  fp = fopen(url.c_str(), "r");
  if (fp == nullptr) {
    not_found(fd);
  } else {
    send_header(fd);
    cat(fp);
  }
  fclose(fp);
}

void Handler::cat(FILE *fp) {
  char buf[1024];
  fgets(buf, sizeof(buf), fp);
  while (!feof(fp)) {
    send(this->fd, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), fp);
  }
}

void Handler::execute() {
  /* 父子进程版，没问题后再修改为thread版 */
  // 有问题！修改 broken pipe
  int cgi_output[2];
  int cgi_input[2];
  pid_t pid;
  int status;
  int content_length = 0;
  bool flag = true;
  if (method == "GET") {
    int n = 1;
    // while (n > 0 && buf != "\n")
    //   n = getline();
    readAll();
  } else {
    int n = 1;
    while (n > 0) {
      n = getline();
      if (buf == "\n")
        break;
      if (flag && buf.size() >= 15 && buf.substr(0, 15) == "Content-Length:") {
        content_length = stoi(buf.substr(16));
        flag = false; /* 这样由于短路运算，不会再进行第二个的判断 */
      }
    }
    if (content_length == -1) {
      bad_request(fd);
      return;
    }
  }
  char temp[128];
  sprintf(temp, "HTTP/1.0 200 OK\r\n");
  send(fd, temp, strlen(temp), 0);
  if (pipe(cgi_output) < 0) {
    cannot_execute(fd);
    return;
  }
  if (pipe(cgi_input) < 0) {
    cannot_execute(fd);
    return;
  }
  if ((pid = fork()) < 0) {
    cannot_execute(fd);
    return;
  }
  if (pid == 0) {
    char method_env[255];
    char query_env[255];
    char length_env[255];
    close(cgi_output[0]);
    close(cgi_input[1]);
    dup2(cgi_output[1], 1); /* 标准输出的内容会送到写通道中 */
    dup2(cgi_input[0], 0);  /* 读通道的内容等同于送到标准输入 */
    sprintf(method_env, "REQUEST_METHOD=%s", method.c_str());
    putenv(method_env);
    if (this->method == "GET") {
      sprintf(query_env, "QUERY_STRING=%s", query_string.c_str());
      putenv(query_env);
    } else {
      /* POST */
      sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
      putenv(length_env);
    }
    execl(url.c_str(), query_string.c_str(), NULL);
    exit(0);
  } else {
    close(cgi_output[1]);
    close(cgi_input[0]);
    char c = '\0';
    if (method == "POST") {
      for (int i = 0; i < content_length; ++i) {
        recv(fd, &c, 1, 0);
        write(cgi_input[1], &c, 1);
      }
    }
    while (read(cgi_output[0], &c, 1) > 0) {
      send(fd, &c, 1, 0);
    }
    close(cgi_output[0]);
    close(cgi_input[1]);
    waitpid(pid, &status, 0);
  }
}

int Handler::peek() {
  char c = '\0';
  int n = recv(fd, &c, 1, MSG_PEEK);
  return n;
}

void Handler::start() {
  // 窥探到没有数据(空字符，返回)
  if (peek() <= 0)
    return;
  getRequestHeader();
  if (!isImpl())
    return;
  fileProcess();
  /* Handler生命周期结束了 */
}