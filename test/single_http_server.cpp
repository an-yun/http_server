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
        println("bind and listen on:", argv[2]);
        signal(SIGINT, Server::catch_signal);  /*捕捉SIGINT信号 Ctrl+C*/
        signal(SIGTERM, Server::catch_signal); //捕捉SIGINT信号 kill http_server
        while (true)
        {
            Connection connection = server.wait_connection();
            std::string file_path = connection.get_request_path();
            println(connection.get_client_ip(), ":", file_path);
            connection.response_to_client();
        }
    }
    else
    {
        printf("usage: ./http_server.bin web_pathserver port");
    }
}
