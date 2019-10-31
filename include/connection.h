#ifndef CONNECTION_H
#define CONNECTION_H
#include "server.h"
#include "request.h"

class Connection{
    friend class Server;
public:
  typedef in_addr_t ip_t;
  bool response();


private:
  int client_st;
  ip_t client_ip;
  int clinet_port;

  Request request;
  //private construction
  Connection();

};

#endif