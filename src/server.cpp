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

Server::Server(std::string web_root_path, int listen_port)
    : web_root_path(web_root_path), listen_port(listen_port), server_socket(-1)
{
    //surpport ipv4 and use tcp protocol
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址
    //println(get_ip_address(server_addr));
    all_servers.insert(this);
    if(!endwith(this->web_root_path,"/"))
        this->web_root_path.push_back('/');
}

bool Server::start()
{
    if (bind_and_listen() == -1)
        return false;
    //create epoll 

    return true;
}

void Server::dispatch_request()
{
    
    
}

Connection Server::wait_connection(unsigned time_out)
{
    Connection conection;
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_st;
    char buff[max_len];
    while ((client_st = ::accept(server_socket, (struct sockaddr *)(&client_addr), &len)) == -1)
    {
        sprintf(buff, "listen error:%s(error: %d)\n", strerror(errno), errno);
        error_message = buff;
    }
    size_t n = recv(client_st, buff, max_len, 0);
    conection.request.parse_request(buff, n);
    conection.request_path = web_root_path + conection.request.get_request_path();
    conection.client_st = client_st;
    conection.client_address = client_addr;
    return conection;
}

int Server::bind_and_listen()
{
    int result_code = bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr));
    if (result_code == -1)
    {
        char buff[256];
        sprintf(buff, "bind error:%s(error: %d)\n", strerror(errno), errno);
        error_message = buff;
        return result_code;
    }
    result_code = listen(server_socket, 8);
    if (result_code == -1)
    {
        char buff[256];
        sprintf(buff, "listen error:%s(error: %d)\n", strerror(errno), errno);
        error_message = buff;
        return result_code;
    }
    return result_code;
}

int Server::accept()
{
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_st = ::accept(server_socket, (struct sockaddr *)(&client_addr), &len);
    if (client_st == -1)
    {
        char buff[256];
        sprintf(buff, "listen error:%s(error: %d)\n", strerror(errno), errno);
        error_message = buff;
    }
    return client_st;
}

const std::string &Server::get_web_path()
{
    return web_root_path;
}

std::string Server::get_error_mess()
{
    return error_message;
}

void Server::close()
{
    ::close(server_socket);
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

std::set<Server *> Server::all_servers;

std::string get_ip_address(const sockaddr_in &client_addr)
{
    return inet_ntoa(client_addr.sin_addr);;
}

