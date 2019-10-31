#ifndef SERVER_H
#define SERVER_H

#include "connection.h"
#include "ioutils.h"
#include <cstring>
#include <limits>
#include <netinet/in.h>
#include <set>
#include <signal.h>
#include <string>
#include <sys/socket.h>

class Server
{
public:
  //public type and constance
  static const unsigned forever = std::numeric_limits<unsigned>::max();
  static const socklen_t socklen = sizeof(sockaddr_in);

  Server(std::string web_root_path = "../web", int listen_port = 8080);
  bool start();
  Connection wait_connection(unsigned time_out = forever);
  std::string get_error_mess(); //if an error occur, return the error message else return empty std::string
  void close();

  ~Server();

  static void catch_signal(int sign);

private:
  //private function
  int bind_and_listen();
  int accept(sockaddr_in &client_addr); // wreite client add info to client_addr
  int accept();

  //save all servers
  static std::set<Server *> all_servers;

  //private member
  std::string web_root_path;
  int listen_port;
  int server_socket;
  struct sockaddr_in server_addr;
  std::string error_message;
};

void test_server();
std::string get_ip_address(const sockaddr_in &client_addr);
std::string get_port(const sockaddr_in &client_addr);

#endif
