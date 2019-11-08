# 进程间传送文件描述符

本项目目标是设计一个架构良好，高性能的HTTP服务器--多进程，<kbd>epoll</kbd>多路IO复用的高性能HTTP服务器，差两个关键的技术点：1.多进程，包含多进程通讯和同步，进程间传送文件描述符。本文档记录了如何在进程间传送文件描述符，以及这个过程中尝试过的方案和遇到的问题。

## 通过内存共享直接传送

采用内存映射，服务器代码如下
```C++
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>

struct fd_mutex_t
{
    int client_st;
    pthread_mutex_t mutex;
};

int main(int argc, char *argv[])
{
    const size_t max_len = 256, process_size = 2;
    char buff[max_len];
    pid_t pid[process_size];
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的

    //监听
    bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 8);

    //共享存储
    fd_mutex_t *fd_mutex = (fd_mutex_t*)mmap(NULL,sizeof(fd_mutex_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    pthread_mutex_init(&fd_mutex->mutex, NULL);
    fd_mutex->client_st = -1;

    srand(time(NULL));
    for (size_t i = 0; i < process_size; ++i)
    {
        if ((pid[i] = fork()) == 0) //chilr process
        {
            //change process name
            argv[0][6] = '0' + i;
            argv[0][7] = '\0';
            argv[0][8] = '\0';
            argv[0][9] = '\0';
            unsigned count = 0;
            while (true)
            {
                sleep(1 + rand() / ((RAND_MAX + 1u) / 8));
                if(fd_mutex->client_st != -1)
                {
                    pthread_mutex_lock(&fd_mutex->mutex);
                    if(fd_mutex->client_st != -1)
                    {
                        int n = sprintf(buff, "Hello,Clinet! This message %u from child process: %lu.\n",++count, i);
                        printf("Child process: %lu send message %u", i, count);
                        //向客户端套接字写入消息 
                        if(write(fd_mutex->client_st, buff, n) < n)
                            printf(".   send failed\n");
                        else
                            printf(".   send success\n");
                    }
                    pthread_mutex_unlock(&fd_mutex->mutex);
                }
                
            }
        }

    }
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_st = ::accept(server_socket, (struct sockaddr *)(&client_addr), &len);
    pthread_mutex_lock(&fd_mutex->mutex);
    printf("Parent process assign client fd %d\n", client_st);
    fd_mutex->client_st = client_st;
    pthread_mutex_unlock(&fd_mutex->mutex);
    for (size_t i = process_size; i > 0; --i)
        waitpid(pid[i - 1], NULL, 0);
    exit(0);
}
```
客户端代码如下：
```C++
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
    pid_t pid = getpid();
    char buff[128];
    while (true)
    {
        sleep(2);
        int n;
        if((n = read(client_fd, buff, 128)) <0);
        {
            close(client_fd);
            return 0;
        }
        else
        {
            buff[n] = '\0';
            printf("Get message from server:%s\n",buff);
        }
    }
    return 0;
}

```