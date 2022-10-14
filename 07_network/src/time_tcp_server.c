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

/*处理客服端的数据*/
int do_client(int clientfd)
{
    char recv_buf[100] = {0};
    int recv_len = 0;
    long t;
    char *s;

    // 默认是阻塞式接收
	// 如果客户端断开连接 或是 主动发送close关闭连接,recv会返回0
	// recv的返回值：<0 出错; =0 连接关闭; >0 接收到数据大小
	while ((recv_len=recv(clientfd, &recv_buf[0], sizeof(recv_buf), 0)) > 0)
	{
        //获得系统时间,并返回给客户端
        t = time(0);
        s = ctime(&t);
        write(clientfd, s, strlen(s));

        //将接受的数据一并返回给客户端
		send(clientfd, recv_buf, recv_len, 0);
	}

	printf("client exit.\n");
	close(clientfd);
	exit(0);

	return 0;
}

/*输出连接上来的客服端相关信息*/
void out_addr(struct sockaddr_in *clientaddr)
{
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char ipv4_addr[16] = {0};//存放点分格式的ip地址 

    if (inet_ntop(AF_INET, &clientaddr->sin_addr, ipv4_addr, addrlen) < 0)
    {
        perror("fail to inet_ntop\n");
        return;
    }
    printf("client(%s:%d) is connected!\n", ipv4_addr, ntohs(clientaddr->sin_port));
}

/*
 *tcp server案例
 *./time_tcp_server 1000
*/
int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("usage: %s #port\n", argv[0]);
        exit(1);
    }

    //注册ctrl+c的信号,用于终止服务器
    if(signal(SIGINT, sig_handler) == SIG_ERR){
        perror("signal sigint error");
        exit(1);
    }
    //处理僵尸进程
    if(signal(SIGCHLD, SIG_IGN) == SIG_ERR){
        perror("signal sigint error");
        exit(1);
    }

    /*步骤1：创建socket
     *注：socket创建在内核中，是一个结构体
     *AF_INET:IPV4
     *SOCK_STREAM:TCP协议
    */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /*步骤2：调用bind函数将socket和地址（ip、port）进行绑定*/
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;//主机字节序
    serveraddr.sin_port = htons(atoi(argv[1]));//网络字节序
    //inet_pton(AF_INET, "192.168.0.10", &serveraddr.sin_addr.s_addr)
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0){
        perror("bind error");
        exit(1);
    }

    /*步骤3：调用listen启动监听（指定port监听）
     *通知系统去接收来自客户端的连接请求(将接受到的客户端连接请求放置到对应的队列中)
     *第二个参数：指定队列的长度
    */
    if(listen(sockfd, 10) < 0){
        perror("listen error");
        exit(1);
    }

    /*步骤4：调用accept函数从队列中获得一个客户端的请求连接,并返回新的socket描述符*/
    struct sockaddr_in  clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    while(1)
    {
        /*该函数会一直阻塞，直到获得一个客户端的连接*/
        int fd = accept(sockfd, (struct sockaddr*)&clientaddr, &clientaddr_len);
        if (fd < 0){
            perror("accept error");
            continue;
        }
        //解析客户端ip地址
        out_addr(&clientaddr);

        /*步骤5：调用IO函数和连接的客户端进行双向的通信*/
        pid_t pid;
        if((pid=fork()) < 0)
        {
            perror("fork error");
            exit(1);
        }
        else if (pid == 0)//子进程，处理客户端的具体消息,不需要父进程的套接字
        {
            close(sockfd);
            do_client(fd);
            break;
        }
        else //父进程，不需要子进程的套接字
        {
            close(fd);
        }
    }

    return 0;
}