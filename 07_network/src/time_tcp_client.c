#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <signal.h>
#include <time.h>

int sockfd;

void sig_handler(int signo)
{
    if(signo == SIGINT)
    {
        printf("server close\n");
        close(sockfd);
        exit(1);
    }
}

/*
 *tcp client案例
 *./time_tcp_client 192.168.1.20 1000
*/
int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("usage: %s ip port\n", argv[0]);
        exit(1);
    }

    //注册ctrl+c的信号,用于终止服务器
    if(signal(SIGINT, sig_handler) == SIG_ERR){
        perror("signal sigint error");
        exit(1);
    }

    /*步骤1：创建socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /*步骤2：调用connect连接服务器*/
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);

    memset(&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &clientaddr.sin_addr.s_addr);

    if(connect(sockfd, (struct sockaddr *)&clientaddr, clientaddr_len) < 0) {
        perror("connect error");
        exit(1);
    }

    printf("client connect success\n");

    /*步骤3：调用IO函数和连接的服务器进行双向的通信*/
    pid_t pid;
    if((pid=fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid == 0)//子进程，处理服务器端应答的消息
    {
        char recv_buffer[1024] = {0};
        size_t recv_size = 0;
        while(1)
        {
            if((recv_size = read(sockfd, recv_buffer, sizeof(recv_buffer))) < 0){
                perror("read error");
            }
            printf("recv data:\n");
            if(write(STDOUT_FILENO, recv_buffer, recv_size) != recv_size){
                perror("write error");
            }
            printf("\n\n");
        }
    }
    else //父进程，接受客户端终端输入的信息，并发送给服务器
    {
        char input_buffer[1024] = {0};
        size_t input_size = 0;
        char *prompt = ">";
        while(1)
        {
            // printf("please input send data:");
            // memset(input_buffer, 0, sizeof(input_buffer));
            // scanf("%s", &input_buffer[0]);
            // getchar();//回收垃圾字符
            // write(sockfd, input_buffer, strlen(input_buffer));
            // sleep(1);

            //从终端获取输入数据
            memset(input_buffer, 0, sizeof(input_buffer));
            write(STDOUT_FILENO, ">", 1);
            input_size = read(STDIN_FILENO, input_buffer, sizeof(input_buffer));
            if(input_size < 0){
                continue;
            }
            input_buffer[input_size - 1] = '\0';

            //发送给服务器
            if(write(sockfd, input_buffer, input_size) < 0){
                perror("write error");
                continue;
            }
            usleep(10000);
        }
    }

    close(sockfd);

    return 0;

}