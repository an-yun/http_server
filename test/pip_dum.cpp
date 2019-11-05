#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    const size_t max_len = 256, process_size = 8;
    char buff[max_len];
    pid_t pid[process_size];
    int fd[process_size][2];
    for (size_t i = 0; i < process_size; ++i)
    {
        if (pipe(fd[i]) < 0)
        {
            printf("pip error!");
            exit(-1);
        }
        if ((pid[i] = fork()) == 0) //chilr process
        {
            //change process name
            argv[0][9] = '0' + i;
            argv[0][10] = '\0';
            argv[0][11] = '\0';
            argv[0][12] = '\0';
            //
            close(fd[i][0]);
            int n = sprintf(buff, "Hello,parent process! This message from child process: %lu.\n", i);
            sleep(10 + process_size - i);
            write(fd[i][1], buff, n);
            exit(0);
        }
        else //parent process
            close(fd[i][1]);
    }

    //parent process
    for (size_t i = process_size; i > 0; --i)
    {
        // print all message
        int n = read(fd[i - 1][0], buff, max_len);
        write(STDOUT_FILENO, buff, n);
        waitpid(pid[i - 1], NULL, 0);
    }
    exit(0);
}



