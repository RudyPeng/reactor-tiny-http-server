#include "Epoll.h"
#include "Handler.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
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
  if (fd != -1) {
    printf("client fd: %d disconnected\n", fd);
    close(fd);
  }
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
  int content_length = 0;
  bool flag = true;
  if (method == "GET") {
    int n = 1;
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
  std::string tmp;
  char c = '\0';
  if (method == "POST") {
    for (int i = 0; i < content_length; ++i) {
      recv(fd, &c, 1, 0);
      tmp.push_back(c);
    }
  }
  dup2(fd, 1);
  if (content_length > 0 && method == "POST") {
    std::vector<std::string> strs;
    for (char &c : tmp) {
      if (c == '&')
        c = ' ';
    }
    std::string t;
    std::istringstream is(tmp);
    while (is >> t)
      strs.push_back(t);
    printf("Content-type:text/html\n\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>POST</title>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h2> Your POST data: </h2>\n");
    printf("<ul>\n");
    for (std::string &param : strs) {
      printf("<li>%s</li>\n", param.c_str());
    }
    printf("</ul>\n");
    printf("</body>\n");
    printf("</html>\n");
    // 能够正确发送，但是在发送万之后就卡住了
  } else {
    printf("Content-type:text/html\n\n");
    printf("not found cgi\n");
  }
  close(fd);
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