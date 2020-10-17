#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include <sys/shm.h>

#include "fd_transfer.h"

struct fd_mutex_t
{
    int process_num;
    pthread_mutex_t mutex;
};

int main(int argc, char *argv[])
{
    const size_t max_len = 256, process_size = 8;
    char buff[max_len];
    pid_t pid[process_size];
    int fd[process_size][2]; //pip fd
    
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
    int shmid = shmget(IPC_PRIVATE,sizeof(fd_mutex_t),0666|IPC_CREAT);
    fd_mutex_t *fd_mutex = (fd_mutex_t *)shmat(shmid, 0, 0);
    pthread_mutex_init(&fd_mutex->mutex, NULL);
    fd_mutex->process_num = -1;
    
    for (size_t i = 0; i < process_size; ++i)
    {
        if (fd_pipe(fd[i]) < 0)
        {
            printf("pip error!");
            exit(-1);
        }
        if ((pid[i] = fork()) == 0) //chilr process
        {
            //close ununsed fd
            close(fd[i][0]);
            //change process name
            argv[0][6] = '0' + i;
            //set rand
            srand(time(NULL)+i);
            //
            unsigned count = 0;
            bool has_client = false;
            int client = -1;
            while (true)
            {
                // usleep(800000-i);
                sleep(1 + rand() / ((RAND_MAX + 1u) / 4));
                printf("Process %zu client %d\n",i, client);
                if(!has_client && fd_mutex->process_num == -1)
                {
                    pthread_mutex_lock(&fd_mutex->mutex);
                    // printf("1-%d\n", fd_mutex->process_num);
                    if (fd_mutex->process_num == -1)
                    {
                        printf("%d==Child process %lu get request\n",fd_mutex->process_num, i);
                        fd_mutex->process_num = i;
                        // msync(fd_mutex,sizeof(fd_mutex_t),MS_SYNC);
                        // printf("==%d\n",fd_mutex->process_num);
                        has_client = true;
                    }
                    // printf("2-%d\n", fd_mutex->process_num);
                    pthread_mutex_unlock(&fd_mutex->mutex);
                }
                // printf("3-%d\n", fd_mutex->process_num);

                if(has_client)
                {
                    if(client == -1)
                    {
                        client = recv_fd(fd[i][1], write);
                        printf("Child process %lu receive client %d\n",i, client);
                    }
                    int n = read(client, buff, 256);
                    buff[n] = '\0';
                    if (n > 0)
                        printf("Child process %lu receive message:%s\n", i, buff);
                    else
                    {
                        printf("Child process %lu receive message failed.\n", i);
                        close(client);
                        client = -1;
                        has_client = false;
                    }
                    n = sprintf(buff, "Hello,Clinet! This message %u from child process: %lu.",++count, i);
                    printf("Child process %lu send message %u", i, count);
                    if(write(client, buff, n) < n)
                    {
                        printf(", send failed!\n");
                        close(client);
                        client = -1;
                        has_client = false;
                    }
                    else
                        printf(", send success!\n");
                    
                }
            }
            exit(0);
        }
        close(fd[i][1]);
    }
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    while(true)
    {
        int client_fd = ::accept(server_socket, (struct sockaddr *)(&client_addr), &len);
        while(fd_mutex->process_num == -1)
            sleep(1);
        pthread_mutex_lock(&fd_mutex->mutex);
        int process_num = fd_mutex->process_num;
        printf("Parent process assign request %d to process %d.\n", client_fd, process_num);
        if(send_fd(fd[process_num][0], client_fd)< 0)
        {
            printf("send fd error!\n");
            exit(0);
        }
        close(client_fd);
        fd_mutex->process_num = -1;
        // msync(fd_mutex,sizeof(fd_mutex_t),MS_SYNC);
        pthread_mutex_unlock(&fd_mutex->mutex);
    }

    exit(0);
}
