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
    int client_fd;
    pthread_mutex_t mutex;
};

int main(int argc, char *argv[])
{
    const size_t max_len = 256, process_size = 4;
    char buff[max_len];
    pid_t pid[process_size];
    int fd[process_size][2][2]; //pip fd
    
    //初始化socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的

    //监听
    if(bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        printf("bind error!\n");
    if(listen(server_socket, 8)<0)
        printf("listen error!\n");
    //共享存储
    fd_mutex_t *fd_mutex = (fd_mutex_t*)mmap(NULL,sizeof(fd_mutex_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    pthread_mutex_init(&fd_mutex->mutex, NULL);
    fd_mutex->client_fd = -1;
    
    for (size_t i = 0; i < process_size; ++i)
    {
        if (pipe(fd[i][0]) < 0 || pipe(fd[i][1]) < 0)
        {
            printf("pip error!");
            exit(-1);
        }
        if ((pid[i] = fork()) == 0) //chilr process
        {
            //change process name
            argv[0][6] = '0' + i;
            argv[0][7] = '\0';
            argv[0][8] = '\0';
            argv[0][9] = '\0';
            //close ununsed fd
            close(fd[i][0][1]);
            close(fd[i][1][0]);
            // srand(time(NULL));
            unsigned count = 0;
            bool has_client = false;
            int client = -1;
            while (true)
            {
                sleep(1 + rand() / ((RAND_MAX + 1u) / process_size));
                if(!has_client && fd_mutex->client_fd != -1)
                {
                    pthread_mutex_lock(&fd_mutex->mutex);
                    if(fd_mutex->client_fd != -1)
                    {
                        printf("Child process %lu accept request %d\n",i,  fd_mutex->client_fd);
                        client = dup(fd_mutex->client_fd);
                        close(fd_mutex->client_fd);
                        fd_mutex->client_fd = -1;
                        has_client = true;
                    }
                    pthread_mutex_unlock(&fd_mutex->mutex);
                }
                if(has_client)
                {
                    printf("get client\n");
                    if(faccessat(client, ".", R_OK | W_OK, 0))
                    {
                        printf("Child process %lu client closed!\n", i);
                        close(client);
                        has_client = false;
                        continue;
                    }
                    int n = read(client, buff, 256);
                    if(n > 0)
                        printf("Child process %lu receive message:%s\n", i, buff);
                    else printf("Child process %lu receive message failed\n", i);
                    n = sprintf(buff, "Hello,Clinet! This message %u from child process: %lu.\n",++count, i);
                    printf("Child process %lu send message %u", i, count);
                    if(write(client, buff, n) < n)
                        printf(".   send failed\n");
                    else
                        printf(".   send success\n");
                    
                }
            }
            exit(0);
        }
        close(fd[i][0][0]);
        close(fd[i][1][1]);
    }
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    while(true)
    {
        int client_st = ::accept(server_socket, (struct sockaddr *)(&client_addr), &len);
        while(fd_mutex->client_fd != -1)
            sleep(1);
        pthread_mutex_lock(&fd_mutex->mutex);
        printf("Parent process accept request %d\n",  client_st);
        fd_mutex->client_fd = client_st;
        pthread_mutex_unlock(&fd_mutex->mutex);
    }

    exit(0);
}



