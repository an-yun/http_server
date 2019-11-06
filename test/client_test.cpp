#include <cstring>
#include <limits>
#include <netinet/in.h>
#include <set>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(8080);
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    socklen_t len = sizeof(client_addr);
    connect(client_fd, (struct sockaddr *)(&client_addr), len);
    unsigned count = 0;
    pid_t pid = getpid();
    char buff[128];
    while (true)
    {
        sleep(2);
        int n = sprintf(buff, "Message %u From client %d", ++count, pid);
        if(write(client_fd, buff, n) != n)
        {
            close(client_fd);
            return 0;
        }
    }
    return 0;
}