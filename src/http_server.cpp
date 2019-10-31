
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

#include "http_server.h"

int main(int argc, char *argv[])
{

    if (argc > 2)
    {
        Server server(argv[1], atoi(argv[2]));
        // Server server2;
        if (!server.start())
        {
            println(server.get_error_mess());
            return -1;
        }
        println("bind and listen ok");

        signal(SIGINT, Server::catch_signal);  /*捕捉SIGINT信号 Ctrl+C*/
        signal(SIGTERM, Server::catch_signal); //捕捉SIGINT信号 kill http_server

        char buff[1028];
        const char *response_head = "HTTP/1.1 %u OK\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length:%u\r\n\r\n";
        while (true)
        {
            Connection connection = server.wait_connection();

            std::string file_path = server.get_web_path();
            std::string file_name = connection.get_request().get_request_path();
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
            println(connection.get_client_ip(), ":", file_name);
            //judge type
            const char *type;
            if (endwith(file_name, ".html"))
                type = "text/html";
            else if (endwith(file_name, ".txt") || endwith(file_name, ".cpp"))
                type = "text/plain";
            // else if(endwith(file_name, ".jpg"))
            //     type = "application/x-jpg";
            else
                type = ".*";
            struct stat t;
            stat(file_name.c_str(), &t);
            // size_t content_size = t.st_size + 4;
            int send_len = sprintf(buff, response_head, response_code, type, t.st_size);
            connection.send(buff, send_len);
            //打开html文件
            FILE *html_file = fopen(file_name.c_str(), "rb");
            while ((send_len = fread(buff, 1, 1024, html_file)) > 0)
                connection.send(buff, send_len);
            // connection.send("\r\n\r\n", 4);
        }
    }
    else
    {
        printf("usage: ./http_server web_pathserver port");
    }
}
