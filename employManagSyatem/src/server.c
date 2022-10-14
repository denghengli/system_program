#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>

#define DATABASE "employ.db"

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

// 登录, 必服务器返回 "OK" 才表示登录成功
// 返回值：1 成功，-1 失败，0 其他
int do_login(int sockfd, MSG_T *msg, sqlite3 *db)
{
	char sql[128] = {0};
	char *errmsg = NULL, **result = NULL;
	int nrow, ncolumn;
	
	//sprintf(sql, "create table if not exists user(name text primary key, passwd text);");
	//sprintf(sql, "insert into user values('%s', '%s');", msg->name, msg->data);
	sprintf(sql, "select * from user where name='%s' and passwd='%s';", msg->name, msg->data);
	if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("sqlite3 get table ok! \n");
	}

	//查询成功，数据库中拥有此用户
	if (nrow == 1)
	{
		printf("client user(%s) login success\n", msg->name);
		memset(msg->data, 0, strlen(msg->data));
		strcpy(msg->data, "OK");
	}
	else //用户名或密码错误
	{
		printf("client user(%s) login fail! \n", msg->name);
		memset(msg->data, 0, strlen(msg->data));
		strcpy(msg->data, "user or passwd wrong! \n");
	}

	//返回应答
	if (send(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to send\n");
		return 0;
	}
	
	return 1;

}


int do_client(int acceptfd, sqlite3 *db)
{
	IPC_MSG_T recv_msg;

	// 默认是阻塞式接收
	// 如果客户端断开连接 或是 主动发送close关闭连接,recv会返回0
	// recv的返回值：<0 出错; =0 连接关闭; >0 接收到数据大小
	while (recv(acceptfd, &recv_msg, sizeof(MSG_T), 0) > 0)
	{
		switch(recv_msg.cmd)
		{
			case CMD_LOGIN:
				do_login(acceptfd, &recv_msg, db);
				break;

			default:
				printf("invalid data msg.\n");
		}
	}

	printf("client exit.\n");
	close(acceptfd);
	exit(0);

	return 0;
}

// ./server 192.168.0.107 10000
// 知识点：
//	1、在一个套接字被创建时，其默认是工作在阻塞模式下，即所有socket相关的数据发送、接收函数都会阻塞，
//	直到数据发送、接收成功，函数才继续执行
//	2、可以通过fcntl来设置成阻塞或非阻塞模式。https://blog.csdn.net/haoyu_linux/article/details/44306993
int main(int argc, const char *argv[])
{
	int sockfd, acceptfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	char ipv4_addr[16];//存放点分格式的ip地址 
	sqlite3 *db = NULL;
	char *errmsg = NULL;
	char sql[128] = {0};
	pid_t pid;
	
	if (argc != 3)
	{
		printf("usage: %s serverip port\n", argv[0]);
		return -1;
	}
	
	/*********************数据库操作*********************/
	if (sqlite3_open(DATABASE, &db) != SQLITE_OK) //打开数据库
	{
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	printf("sqlite3 open %s success.\n", DATABASE);
	sprintf(sql, "create table if not exists user(\
					name TEXT primary key, \
					passwd TEXT, \
					number TEXT, \
					phone TEXT, \
					addr TEXT, \
					age INTEGER, \
					salary INTEGER);");
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) //创建用户表
	{
		printf("%s\n", errmsg);
		return -1;
	}
	
	/*********************tcp server操作*********************/
	// 申请socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("fail to socket\n");
		return -1;
	}	
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
    bzero(&(server_addr.sin_zero), sizeof(server_addr.sin_zero));
    
    //绑定套接字
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("fail to bind.\n");
		return -1;
	}
	
	//将套接字设为监听模式
	if (listen(sockfd, 5) < 0)
	{
		perror("fail to listen.\n");
		exit(1);
	}
	printf("listen success.\n");
	
	//处理僵尸进程
	signal(SIGCHLD, SIG_IGN);
	
	while (1)
	{
		//接收客户端的连接请求
		if ((acceptfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen)) < 0)
		{
			perror("fail to accept\n");
			return -1;
		}
		if (inet_ntop(AF_INET, &client_addr.sin_addr, ipv4_addr, addrlen) < 0)
		{
			perror("fail to inet_ntop\n");
			return -1;
		}
		printf("client(%s:%d) is connected!\n", ipv4_addr, htons(client_addr.sin_port));

		//创建子进程
		if ((pid = fork()) < 0)
		{
			perror("fail to fork!\n");
			return -1;
		}
		else if (pid == 0) //子进程,处理客服端具体的消息
		{
			close(sockfd);
			do_client(acceptfd, db);
		}
		else //父进程,用来接收客户端的连接请求
		{
			close(acceptfd);//父进程不需要子进程的套接字
		}
	}
	
	return 0;
}