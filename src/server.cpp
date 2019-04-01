#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "server.h"

void test_server()
{
    Server server;
    signal(SIGINT, Server::catch_signal);  //捕捉SIGINT信号
}

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
