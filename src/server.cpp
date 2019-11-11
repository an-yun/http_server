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
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_st =::accept(server_socket, (struct sockaddr *)(&client_addr), &len);
    while (!time_out  && client_st == -1)
    {
        error_message = std::string("listen error:")+ strerror(errno) + std::to_string(errno);
        client_st = ::accept(server_socket, (struct sockaddr *)(&client_addr), &len);
        --time_out;
    }
    return Connection(web_root_path, client_st, client_addr);
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

#ifdef TEST
void Server::test_connection()
{
    Connection con;
    con.buff = "buff";
    con.client_st = 1;
    println(con.client_st, con.buff);
    Connection con1 = std::move(con);
    println(con.client_st, con.buff);
    println(con1.client_st, con1.buff);
    std::move(con1);
    println(con1.client_st, con1.buff);
}
  #endif