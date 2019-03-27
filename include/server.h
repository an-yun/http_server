#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

using std::string;

string get_ip_address(const sockaddr_in &client_addr);
string get_port(const sockaddr_in &client_addr);

class Server
{
private:
  string web_root_path;
  int listen_port;

  int server_socket;

public:
  Server(string web_root_path="../web", int listen_port = 8080);
  int bind_and_listen();
  int accept(sockaddr_in &client_addr);// wreite client add info to client_addr
  int accept();
  string get_error_mess();//if an error occur, return the error message else return empty string
  void close();
  ~Server();
};

Server::Server(string web_root_path, int listen_port)
    :web_root_path(web_root_path),listen_port(listen_port),server_socket(-1)
{
}

void Server::close()
{

}

Server::~Server()
{
    close();
}
