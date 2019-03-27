#include <netinet/in.h>
#include <set>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <signal.h>

using std::set;
using std::string;

void catch_Signal(int Sign);
int signal1(int signo, void (*func)(int));
string get_ip_address(const sockaddr_in &client_addr);
string get_port(const sockaddr_in &client_addr);


class Server
{
  private:
    static set<Server *> all_servers;

    string web_root_path;
    int listen_port;
    int server_socket;

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



set<Server *> Server::all_servers;


Server::Server(string web_root_path, int listen_port)
    : web_root_path(web_root_path), listen_port(listen_port), server_socket(-1)
{
    all_servers.insert(this);
}

void Server::close()
{
    printf("server %s:%d closed \n", web_root_path.c_str(), listen_port);
}

Server::~Server()
{
    close();
    all_servers.erase(this);
}

int signal1(int signo, void (*func)(int))
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    return sigaction(signo, &act, &oact);
}

void Server::catch_signal(int sign)
{
    switch (sign)
    {
    case SIGINT:
    case SIGTERM:
        for(auto s:all_servers)
            s->close();
        printf("all server closed\n");
        exit(0);
        break;
    case SIGPIPE:
        //printf("signal SIGPIPE\n");
        break;
    case SIGALRM:
        //printf("signal SIGALRM\n");
        break;
    default:
        break;
    }
}