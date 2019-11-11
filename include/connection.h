#ifndef CONNECTION_H
#define CONNECTION_H
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "request.h"

class Connection
{
  #ifdef TEST
    friend class Server;
  #endif
  // friend class Worker;

public:
  typedef in_addr_t ip_t;
  static const size_t fail_size = static_cast<size_t>(-1);
  static const size_t max_len = 1024;

  #ifdef TEST
    Connection() = default;
  #endif

  Connection(const std::string &request_path, int client_st, const sockaddr_in &client_address);
  Connection(const Connection &) = delete;
  Connection& operator=(Connection &) = delete;
  Connection(Connection &&con);
  Connection &operator=(Connection &&con) = default;
  const Request &get_request();
  size_t send(const char *content, size_t len);
  size_t send(const std::string &content);
  size_t receive(size_t len = max_len);
  std::string get_client_ip() const;
  std::string get_request_path() const;

  ~Connection();

private:
  int client_st;
  sockaddr_in client_address;
  std::string buff;
  Request request;

  std::string request_path;
  bool close();
};

#endif