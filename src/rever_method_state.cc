#include <cstring>
#include <stdio.h>
#include <sys/socket.h>

#define SERVER_STRING "Server: ppphttpd/0.0.1\r\n"
#define SENDBUF(client, buf) send(client, buf, strlen(buf), 0);

void unimplemented(int client) {
  char buf[1024];
  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  SENDBUF(client, buf);
  sprintf(buf, SERVER_STRING);
  SENDBUF(client, buf);
  sprintf(buf, "Content-Type: text/html\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "</BODY></HTML>\r\n");
  SENDBUF(client, buf);
}

void not_found(int client) {
  char buf[1024];
  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  SENDBUF(client, buf);
  sprintf(buf, SERVER_STRING);
  SENDBUF(client, buf);
  sprintf(buf, "Content-Type: text/html\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "your request because the resource specified\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "</BODY></HTML>\r\n");
  SENDBUF(client, buf);
}

void cannot_execute(int client) {
  char buf[1024];
  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "Content-type: text/html\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
  SENDBUF(client, buf);
}

void send_header(int client) {
  char buf[1024];
  sprintf(buf, "HTTP/1.0 200 OK\n");
  SENDBUF(client, buf);
  sprintf(buf, SERVER_STRING);
  SENDBUF(client, buf);
  sprintf(buf, "Content-Type: text/html\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "\r\n");
  SENDBUF(client, buf);
}

void bad_request(int client) {
  char buf[1024];
  sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "Content-type: text/html\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<HTML><TITLE>Bad Request</TITLE>\r\n");
  SENDBUF(client, buf);
  sprintf(buf, "<BODY><P>bad request</P></BODY></HTML>\r\n");
  SENDBUF(client, buf);
}