#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define USER_TYPE_ADMIN 100		//管理员
#define USER_TYPE_GENERAL 101	//普通员工

#define CMD_LOGIN	1	//登录
#define CMD_QUIT	2	//退出

#define CMD_ADMIN_ADD_USER		1	//增加用户信息
#define CMD_ADMIN_DEL_USER		2	//删除用户信息
#define CMD_ADMIN_MOD_USER		3	//修改用户信息
#define CMD_ADMIN_QUERY_USER	4	//查询用户信息
#define CMD_ADMIN_MOD			5	//修改管理员自己信息

#define CMD_GENERAL_QUERY		1	//查询信息
#define CMD_GENERAL_MOD			2	//修改信息

// 员工信息结构体
typedef struct 
{
	int user_type;		//用户类型，普通用户、管理员
	char name[64];		//用户名
	char passwd[64];	//密码
	char number[128];	//员工编号
	char phone[64];		//联系电话
	char addr[256];		//住址
	int age;			//年龄
	int salary;			//薪资
	
}EMPLOYEE_INFO_T;

// 通信双方的信息结构体
typedef struct 
{
	int cmd;	//操作命令
	EMPLOYEE_INFO_T employ_info;	//员工信息
	
}IPC_MSG_T;

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

// 登录
// 返回值：0 成功，-1 失败
int do_login(int sockfd, IPC_MSG_T *msg)
{
	printf("login ...\n");
	
	memset(msg, 0, sizeof(IPC_MSG_T));
	msg->cmd = CMD_LOGIN;
	printf("input name:"); scanf("%s", msg->employ_info.name); getchar();
	printf("input passwd:"); scanf("%s", msg->employ_info.passwd); getchar();
	
	if (send(sockfd, msg, sizeof(IPC_MSG_T), 0) < 0)
	{
		printf("fail to send login msg.\n");
		return -1;
	}
	
	if (recv(sockfd, msg, sizeof(IPC_MSG_T), 0) < 0)
	{
		printf("fail to login.\n");
		return -1;
	}
	
	//登录成功
	printf("login ok! \n");
	
	return 0;
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
	IPC_MSG_T send_msg;
	
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
		printf("*******         1.登录   2.退出         *******\n");
		printf("***********************************************\n");
		printf("please choose:");
		
		scanf("%d", &input_nbr);
		getchar();//回收垃圾字符
		
		// 一级菜单
		switch (input_nbr)
		{
			case CMD_LOGIN:
				if (do_login(sockfd, &send_msg) == 0)
				{
					if (send_msg.employ_info.user_type == USER_TYPE_ADMIN) goto _admin;
					if (send_msg.employ_info.user_type == USER_TYPE_GENERAL) goto _general;
					printf("The user name does not exist. \n");
				}
				break;
				
			case CMD_QUIT:
				close(sockfd);
				exit(0);
				break;
			
			default:
				printf("Invalid data cmd. \n");
				break;
		}
	}
	
	//管理员二级菜单
_admin:
	while(1)
	{
		printf("*******************************管理员*********************************\n");
		printf("*****  1.增加员工信息  2.删除员工信息  3.修改员工信息   4.查询员工信息\n");
		printf("*****  5.修改管理员信息  6.退出\n");
		printf("***********************************************\n");
		printf("please choose:");
		
		input_nbr = 0;
		scanf("%d", &input_nbr);
		getchar();//回收垃圾字符
		
		
		switch (input_nbr)
		{
			case CMD_ADMIN_ADD_USER:
				break;
				
			case CMD_ADMIN_DEL_USER:
				break;
				
			case CMD_ADMIN_MOD_USER:
				break;
				
			case CMD_ADMIN_QUERY_USER:
				break;
				
			case CMD_ADMIN_MOD:
				break;
				
			case 6:
				close(sockfd);
				exit(0);
				break;
			
			default:
				printf("Invalid data cmd. \n");
				break;
		}
	}
	
	//普通员工二级菜单
_general:
	while(1)
	{
		printf("*******************************员工********************************\n");
		printf("*******************  1.查询  2.修改  3.退出 ***********************\n");
		printf("*******************************************************************\n");
		printf("please choose:");
		
		input_nbr = 0;
		scanf("%d", &input_nbr);
		getchar();//回收垃圾字符
		
		
		switch (input_nbr)
		{
			case CMD_GENERAL_QUERY:
				break;
				
			case CMD_GENERAL_MOD:
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