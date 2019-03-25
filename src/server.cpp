#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if(argc > 2)
    {
        int port = atoi(argv[2]);
    }
    else
    {
        printf("usage: server");
    }
    return 0;
}
