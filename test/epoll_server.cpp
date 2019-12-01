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
    const char *t = "HTTP/1.1 %u OK\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length:%u\r\n\r\n%s";
    const char a[] = "<html><head><title>404</title></ head><body><h1>文件不存在</h1><p>404错误</p></body></html>";
    char buff[2014];
    size_t n=sprintf(buff, t, 404, "text/html", sizeof(a)-1,a);
    println(n);
    printf("%s",buff);
    return 0;
}