#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <set>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <signal.h>
#include "ioutils.h"

using std::set;
using std::string;


class Server
{
  private:
    static set<Server *> all_servers;

    string web_root_path;
    int listen_port;
    int server_socket;
    struct sockaddr_in server_addr;
    string error_message;

  public:
    static void catch_signal(int sign);

    Server(string web_root_path = "../web", int listen_port = 8080);
    int bind_and_listen();
    int accept(sockaddr_in &client_addr); // wreite client add info to client_addr
    int accept();
    string get_error_mess(); //if an error occur, return the error message else return empty string
    void close();
    
    ~Server();
};

void test_server();  
string get_ip_address(const sockaddr_in &client_addr);
string get_port(const sockaddr_in &client_addr);


#endif
