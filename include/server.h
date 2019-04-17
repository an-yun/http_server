#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <set>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <signal.h>
#include "ioutils.h"


class Server
{
  private:
    static std::set<Server *> all_servers;

    std::string web_root_path;
    int listen_port;
    int server_socket;
    struct sockaddr_in server_addr;
    std::string error_message;

  public:
    static void catch_signal(int sign);

    Server(std::string web_root_path = "../web", int listen_port = 8080);
    int bind_and_listen();
    int accept(sockaddr_in &client_addr); // wreite client add info to client_addr
    int accept();
    std::string get_error_mess(); //if an error occur, return the error message else return empty std::string
    void close();
    
    ~Server();
};

void test_server();  
std::string get_ip_address(const sockaddr_in &client_addr);
std::string get_port(const sockaddr_in &client_addr);


#endif
