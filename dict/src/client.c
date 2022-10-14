#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define NAME_LEN 32

#define MSGTYPE_R 11 //user - register
#define MSGTYPE_L 22 //user - login
#define MSGTYPE_Q 33 //user - query
#define MSGTYPE_H 44 //user - history

// 通信双方的信息结构体
typedef struct {
	int type;
	char name[NAME_LEN];
	char data[256];
}MSG_T;

/*
struct sockaddr
{
    uint8_t        sa_len;
    sa_family_t    sa_family;
    char           sa_data[14];
};
/* members are in network byte order */
/*
struct sockaddr_in
{
    uint8_t        sin_len;
    sa_family_t    sin_family;
    in_port_t      sin_port;
    struct in_addr sin_addr;
#define SIN_ZERO_LEN 8
    char            sin_zero[SIN_ZERO_LEN];
};

struct in_addr
{
    in_addr_t s_addr;
};
*/

// 注册
int do_register(int sockfd, MSG_T *msg)
{
	printf("register ... \n");
	
	memset(msg, 0, sizeof(MSG_T));
	msg->type = MSGTYPE_R;
	printf("input name:"); scanf("%s", msg->name); getchar();
	printf("input passwd:"); scanf("%s", msg->data); getchar();
	
	if (send(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to send register msg.\n");
		return -1;
	}
	
	if (recv(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to register.\n");
		return -1;
	}
	
	//ok !  或者 user already exist.
	printf("%s\n", msg->data);
	
	return 1;
}

// 登录, 必服务器返回 "OK" 才表示登录成功
// 返回值：1 成功，-1 失败，0 其他
int do_login(int sockfd, MSG_T *msg)
{
	printf("login ...\n");
	
	memset(msg, 0, sizeof(MSG_T));
	msg->type = MSGTYPE_L;
	printf("input name:"); scanf("%s", msg->name); getchar();
	printf("input passwd:"); scanf("%s", msg->data); getchar();
	
	if (send(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to send register msg.\n");
		return -1;
	}
	
	if (recv(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to login.\n");
		return -1;
	}
	
	//登录成功
	if (strncmp(msg->data, "OK", 3) == 0)
	{
		printf("login ok! \n");
		return 1;
	}
	else
	{
		printf("%s\n", msg->data);
	}
	
	return 0;
}

// 单词查询
int do_query(int sockfd, MSG_T *msg)
{
	printf("query ...\n");
	
	msg->type = MSGTYPE_Q;
	while(1)
	{
		printf("input word:"); scanf("%s", msg->data); getchar();
		
		// 输入是"#"表示退出本次查询
		if (strncmp(msg->data, "#", 1) == 0)
			break;
		
		//将要查询的单词发送给服务器
		if (send(sockfd, msg, sizeof(MSG_T), 0) < 0)
		{
			printf("fail to send.\n");
			return -1;
		}
		
		//等待服务器，传递回来的单次的注释信息
		if (recv(sockfd, msg, sizeof(MSG_T), 0) < 0)
		{
			printf("fail to recv.\n");
			return -1;
		}
		printf("%s\n", msg->data);
	}
	
	return 1;
}

// 历史记录查询
int do_history(int sockfd, MSG_T *msg)
{
	printf("history ...\n");
	
	msg->type = MSGTYPE_H;

	//将消息发送给服务器
	if (send(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to send.\n");
		return -1;
	}
		
	while(1)
	{
		//等待服务器，传递回来的单次的注释信息
		if (recv(sockfd, msg, sizeof(MSG_T), 0) < 0)
		{
			printf("fail to recv.\n");
			return -1;
		}

		if (msg->data[0] == '\0')
		    break;

		//输出历史记录信息    
		printf("%s\n", msg->data);
	}
	
	return 1;

}

// ./client 192.168.0.107 10000
// 知识点：
//	1、在一个套接字被创建时，其默认是工作在阻塞模式下，即所有socket相关的数据发送、接收函数都会阻塞，
//	直到数据发送、接收成功，函数才继续执行
//	2、可以通过fcntl来设置成阻塞或非阻塞模式。https://blog.csdn.net/haoyu_linux/article/details/44306993
int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in server_addr;
	int input_nbr;
	MSG_T send_msg;
	
	if (argc != 3)
	{
		printf("usage: %s serverip port\n", argv[0]);
		return -1;
	}
	
	// 申请socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("fail to socket\n");
		return -1;
	}	
	
	server_addr.sin_family  = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
    bzero(&(server_addr.sin_zero), sizeof(server_addr.sin_zero));
    
    /*连接到服务器*/
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("fail to connect\n");
		return -1;
	}
	
	while (1)
	{
		printf("***********************************************\n");
		printf("*******  1.register   2.login   3.quit  *******\n");
		printf("***********************************************\n");
		printf("please choose:");
		
		scanf("%d", &input_nbr);
		getchar();//回收垃圾字符
		
		// 一级菜单
		switch (input_nbr)
		{
			case 1:
				do_register(sockfd, &send_msg);
				break;
				
			case 2:
				if (do_login(sockfd, &send_msg) == 1)
				{
					goto _login;
				}
				break;
				
			case 3:
				close(sockfd);
				exit(0);
				break;
			
			default:
				printf("Invalid data cmd. \n");
				break;
		}
	}
	
	//二级菜单,登录后进行单词查询
_login:
	while(1)
	{
		printf("***********************************************\n");
		printf("*****1.query_word  2.history_record  3.quit****\n");
		printf("***********************************************\n");
		printf("please choose:");
		
		input_nbr = 0;
		scanf("%d", &input_nbr);
		getchar();//回收垃圾字符
		
		
		switch (input_nbr)
		{
			case 1:
				do_query(sockfd, &send_msg);
				break;
				
			case 2:
				do_history(sockfd, &send_msg);
				break;
				
			case 3:
				close(sockfd);
				exit(0);
				break;
			
			default:
				printf("Invalid data cmd. \n");
				break;
		}
	}
	
	return 0;
}