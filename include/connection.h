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
  friend class Server;
  // friend class Worker;

public:
  typedef in_addr_t ip_t;
  static const size_t fail_size = static_cast<size_t>(-1);
  static const size_t max_len = 1024;

  Connection(Connection &&con);
  Connection &operator=(Connection &&con) = default;
  const Request &get_request();
  size_t send(const char *content, size_t len);
  size_t send(const std::string &content);
  std::string receive(size_t len = max_len);
  std::string get_client_ip() const;
  std::string get_request_path() const;

  ~Connection();

private:
  Request request;

  int client_st;
  std::string request_path;
  sockaddr_in client_address;
  std::string buff;

  //private construction
  Connection();

  bool close();
};

#endif