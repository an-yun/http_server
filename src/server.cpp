#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
void catch_Signal(int Sign);
int signal1(int signo, void (*func)(int));

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        int port = atoi(argv[2]);                 // 解析端口
        int st = socket(AF_INET, SOCK_STREAM, 0); //建立TCP的socket描述符
        //设置监听地址
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr)); //可以去掉
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址
        //将套接字绑定在指定的地址上
        if (bind(st, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
            printf("bind error:%s(error: %d)\n", strerror(errno), errno);
        //监听客户端连接
        if (listen(st, 8) == -1)
            printf("listen error:%s(error: %d)\n", strerror(errno), errno);
        signal1(SIGINT, catch_Signal);	//捕捉SIGINT信号	
	    signal1(SIGPIPE, catch_Signal);
	    signal1(SIGALRM, catch_Signal);
	    signal1(SIGTERM, catch_Signal);
        const size_t MAX_LEN = 2048;
        char buff[MAX_LEN];
        while (true)
        {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_st = accept(st, (struct sockaddr *)&client_addr, &addr_len); //阻塞接受连接
            if (client_st == -1 && errno != 4)
            {
                printf("accept error:%s(error: %d)\n", strerror(errno), errno);
                break; //accept错误，循环break
            }
            else
            {
                //打印客户端ip
                unsigned char *ip = (unsigned char *)&client_addr.sin_addr.s_addr;
                printf("客户端ip:%u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
                //打印客户端请求头
                size_t n = recv(client_st, buff, MAX_LEN,0);
                buff[n] = 0;
                printf("请求头:\n %s\n", buff);
                //处理回复
                const char *response_head = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length:%u\r\n\r\n";
                //打开html文件
                struct stat t;
                const char *filename = "../web/index.html";
                stat(filename, &t);
                int file_size = t.st_size+4;
                char file_buff[MAX_LEN];
                FILE *html_file = fopen(filename, "rb");
                fread(file_buff, t.st_size, 1, html_file);
                file_buff[file_size-4] = '\r';
                file_buff[file_size-3] = '\n';
                file_buff[file_size-2] = '\r';
                file_buff[file_size-1] = '\n';
                sprintf(buff, response_head, file_size);//设置响应头
                size_t headlen = strlen(buff);
                memcpy(buff + headlen, file_buff, file_size);
                if(send(client_st, buff, headlen+file_size,0) == -1)
                    perror("send error");
                close(client_st);
            }
        }
        close(st);
    }
    else
    {
        printf("usage: server");
    }
    return 0;
}

void catch_Signal(int Sign)
{
	switch (Sign)
	{
	case SIGINT:
		printf("signal SIGINT\n");
		break;
	case SIGPIPE:
		printf("signal SIGPIPE\n");
	case SIGALRM:
		printf("signal SIGALRM\n");
		break;	
	case SIGTERM:
		printf("signal SIGTERM\n");
		printf("myhttp end\n");
		exit(0);
		break;
	default:
		break;

	}
}
int signal1(int signo, void (*func)(int))
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	return sigaction(signo, &act, &oact);
}