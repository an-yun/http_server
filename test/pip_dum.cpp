#include <stdio.h>
#include <unistd.h>

int main()
{
    const size_t max_len = 256;
    pid_t pid;
    int fd[2];
    if(pipe(fd) < 0)
    {
        printf("pip error!");
        return 0;
    }
    if ((pid = fork()) == 0) //chilr process
    {
        dup2(fd[0], STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execl("./add.bin", "", (char *)0);
        printf("pip error!");
        _exit(-1);
    }
    else
    {
        /* code */
        char buff[max_len];
        while (fgets(buff, max_len, stdin))
        {
            printf("read add number from parent:");
            size_t n = read(STDIN_FILENO, buff, max_len);
            write(fd[1], buff, n);
            n = read(fd[0], buff, max_len);
            write(STDOUT_FILENO, buff, n);
            // wirte(fd[1],buff,);
            // fgets(buff, max_len, add_pip);
            // fputs(buff, stdout);
        }
    }
}