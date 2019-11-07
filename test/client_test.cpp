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
    if(connect(client_fd, (struct sockaddr *)(&client_addr), sizeof(client_addr)) < 0)
    {
        printf("connect error!\n");
        close(client_fd);
        return 0;
    }
    unsigned count = 0;
    pid_t pid = getpid();
    char buff[128];
    while (true)
    {
        sleep(2);
        int n = read(client_fd,buff,128);
        printf("Receive massage from server: %s", buff);
        n = sprintf(buff, "Message %u From client %d", ++count, pid);
        printf("Client %d send message %u", pid, count);
        if(write(client_fd, buff, n) != n)
        {
            close(client_fd);
            return 0;
        }
    }
    return 0;
}