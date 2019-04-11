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

void test_server()
{
    Server server1, server2;
    signal(SIGINT, Server::catch_signal);  /*捕捉SIGINT信号 Ctrl+C*/ 
    signal(SIGTERM, Server::catch_signal); //捕捉SIGINT信号 kill http_server
    if(server1.bind_and_listen()) println(server1.get_error_mess());
    else println("bind and listen ok");
}
    Server::Server(string web_root_path, int listen_port)
    : web_root_path(web_root_path), listen_port(listen_port), server_socket(-1)
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址
    //println(get_ip_address(server_addr));    
    all_servers.insert(this);
}


int Server::bind_and_listen()
{
    int result_code = bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr));
    // to do
    char buff[256];
    if(result_code == -1)
    {
        sprintf(buff, "bind error:%s(error: %d)\n", strerror(errno), errno);
        error_message = buff;
        return result_code;
    }
    result_code = listen(server_socket, 8);
    if(result_code == -1)
    {
        sprintf(buff, "listen error:%s(error: %d)\n", strerror(errno), errno);
        error_message = buff;
        return result_code;
    }
    return result_code;
}

string Server::get_error_mess()
{
    return error_message;
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
