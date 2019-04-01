#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H


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

int signal1(int signo, void (*func)(int));
void catch_Signal(int Sign);

#endif