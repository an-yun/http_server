#ifndef SERVER_H
#define SERVER_H

#include <cstring>
#include <limits>
#include <netinet/in.h>
#include <set>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "ioutils.h"
#include "connection.h"

class Server
{
public:
  //public type and constance
  static const unsigned forever = std::numeric_limits<unsigned>::max();
  static const size_t max_len = 1024;
  
  #ifdef TEST
    static void test_connection();
  #endif

  Server(std::string web_root_path = "../web", int listen_port = 8080);
  //启动服务器，绑定并监听
  bool start();
  void dispatch_request();
  //阻塞等待连接
  Connection wait_connection(unsigned time_out = forever);
  //获取web根目录
  const std::string & get_web_path();
  std::string get_error_mess(); //if an error occur, return the error message else return empty std::string
  void close();

  ~Server();

  static void catch_signal(int sign);
  static void set_404_path(const std::string &default_404);

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
