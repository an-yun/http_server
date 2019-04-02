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


set<Server *> Server::all_servers;

string get_ip_address(const sockaddr_in &client_addr)
{
    unsigned char *ip = (unsigned char *)&client_addr.sin_addr.s_addr;
    char ip_str[24];
    sprintf(ip_str, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return ip_str;
}

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
        printf("\n");
        for (auto s : all_servers)
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
