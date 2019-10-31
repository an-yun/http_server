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
}

bool Server::start()
{
    if (bind_and_listen() == -1)
        return false;

    return true;
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
    conection.client_st = client_st;
    conection.client_ip = client_addr.sin_addr.s_addr;
    conection.clinet_port = client_addr.sin_port;
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
    in_addr_t ip = client_addr.sin_addr.s_addr;
    char ip_str[24];
    sprintf(ip_str, "%u.%u.%u.%u", ip & 0xff000000, ip & 0xff0000, ip & 0xff00, ip & 0xff);
    return ip_str;
}

void test_server()
{
    char buff[1028];
    Server server1;
    // Server server2;
    if (!server1.start())
    {
        println(server1.get_error_mess());
        return;
    }
    println("bind and listen ok");

    signal(SIGINT, Server::catch_signal);  /*捕捉SIGINT信号 Ctrl+C*/
    signal(SIGTERM, Server::catch_signal); //捕捉SIGINT信号 kill http_server
                                           //处理回复
    const char *response_head = "HTTP/1.1 %u OK\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length:%u\r\n\r\n";
    while (true)
    {
        Connection connection = server1.wait_connection();
        
        std::string file_path = server1.get_web_path();
        std::string file_name = connection.get_request().get_request_path();
        println(file_name);
        size_t response_code = 200;
        //the file can be read?
        if (file_name == "/")
            file_name = file_path + "/index.html";
        else
            file_name = file_path + file_name;
        if (access(file_name.c_str(), F_OK | R_OK) == -1)
        {
            file_name = file_path + "/404.html";
            response_code = 404;
        }
        //judge type
        const char *type;
        if (endwith(file_name, ".html"))
            type = "text/html";
        else if(endwith(file_name, ".txt"))
            type = "text/plain";
        else if(endwith(file_name, ".jpg"))
            type = "application/x-jpg";
        else type = ".*";
        struct stat t;
        stat(file_name.c_str(), &t);
        size_t content_size = t.st_size + 4;
        int send_len = sprintf(buff, response_head,response_code,type,content_size);
        connection.send(buff,send_len);
        //打开html文件
        FILE *html_file = fopen(file_name.c_str(), "rb");
        while((send_len = fread(buff, 1,1024, html_file)) > 0)
        {
            connection.send(buff,send_len);
        }
        connection.send("\r\n\r\n", 4);
    }
}