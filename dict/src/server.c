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

#define NAME_LEN 32

#define MSGTYPE_R 11 //user - register
#define MSGTYPE_L 22 //user - login
#define MSGTYPE_Q 33 //user - query
#define MSGTYPE_H 44 //user - history

#define DATABASE "my.db"

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
int do_register(int sockfd, MSG_T *msg, sqlite3 *db)
{
	char sql[128] = {0};
	char *errmsg = NULL;

	//sprintf(sql, "create table if not exists user(name text primary key, passwd text);");
	sprintf(sql, "insert into user values('%s', '%s');", msg->name, msg->data);
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
		memset(msg->data, 0, strlen(msg->data));
		sprintf(msg->data, "user(%s) already exist.!", msg->name);
	}
	else
	{
		printf("client user(%s) register success\n", msg->name);
		memset(msg->data, 0, strlen(msg->data));
		strcpy(msg->data, "register ok !");
	}

	//返回应答
	if (send(sockfd, msg, sizeof(MSG_T), 0) < 0)
	{
		printf("fail to send\n");
		return 0;
	}
	
	return 1;
}

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

//根据单词从dirt.txt文本中查找对应的注释信息
//返回值: -1 文件打开失败; 0 单词未找到; 1 查找成功
int do_searchword(MSG_T *msg, char *word)
{
    FILE *fp = NULL;
    int word_len = strlen(word);
    char row_data[512] = {'\0'};
    int res = 0;
    char *p; //指向注释
    
    //打开文件
    if ((fp = fopen("dict.txt", "r")) == NULL)
    {
        perror("fail to open dict.txt.\n");
        return -1;
    }

    //打印客户端要查询的单词
    word_len = strlen(word);
    printf("%s, len = %d\n", word, word_len );

    //读取文件 行数据(一行一行读取),对比要查询的单词
    //如果成功，该函数返回相同的 str 参数。
    //如果到达文件末尾或者没有读取到任何字符，str 的内容保持不变，并返回一个空指针。
    //如果发生错误，返回一个空指针。
    while (fgets(row_data, 512, fp) != NULL)
    {
        res = strncmp(row_data, word, word_len);//每行对比前word_len个字节

        if (res != 0)
            continue;

        if (row_data[word_len] != ' ') //单词跟注释之间没有空格
            goto _end;

        // 找到了单词,跳过所有的空格
        p = row_data + word_len;
        while (*p == ' ')
        {
            p++;
        }

        strcpy(msg->data, p);
        fclose(fp);
        return 1;
    }

_end:
    fclose(fp);
    return 0; //文件对比完,单词未找到
}

//获取系统时间
void get_date(char *data)
{
    time_t rowtime; //typedef long     time_t;
//    struct tm {
//                   int tm_sec;    /* Seconds (0-60) */
//                   int tm_min;    /* Minutes (0-59) */
//                   int tm_hour;   /* Hours (0-23) */
//                   int tm_mday;   /* Day of the month (1-31) */
//                   int tm_mon;    /* Month (0-11) */
//                   int tm_year;   /* Year - 1900 */
//                   int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
//                   int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
//                   int tm_isdst;  /* Daylight saving time */
//               };
    struct tm *info;

    // rowtime = the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)
    time(&rowtime);

    //进行时间格式转换
    info = localtime(&rowtime);

    sprintf(data, "%d-%d-%d %d:%d:%d", info->tm_year + 1900, info->tm_mon + 1, info->tm_mday,
                                       info->tm_hour, info->tm_min, info->tm_sec);

    printf("get date is : %s\n", data);                                   
}

// 单词查询
int do_query(int sockfd, MSG_T *msg, sqlite3 *db)
{
	char sql[128] = {0};
	char word[64] = {0};
	int found = 0;
    char date[128] = {0};
    char *errmsg;

    printf("\n");//显示换行
    
    //单词查找
	strcpy(word, msg->data);
	found = do_searchword(msg, word);

	if (found == 1)// 找到了单词，需要将 name,date,word 插入到历史记录表中去
	{
	    get_date(date);//获取系统时间

        //sprintf(sql, "insert into user values('%s', '%s');", msg->name, msg->data);
	    sprintf(sql, "insert into record values('%s', '%s', '%s')", msg->name, date, word);
	    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	    {
	        printf("%s\n", errmsg);
	        return -1;
	    }
	    else
	    {
	        printf("sqlite3 insert record done.\n");
	    }
	}
    else if (found == 0)//没有找到
    {
        memset(msg->data, 0, strlen(msg->data));
        strcpy(msg->data, "Not found!\n");
    }
    else if (found == -1)//dict.txt代开失败
    {
        memset(msg->data, 0, strlen(msg->data));
        strcpy(msg->data, "fail to open dict.txt.");
    }

	//将查询的结果发送给客户端
	send(sockfd, msg, sizeof(MSG_T), 0);
    
    return 0;

}


// 得到查询结果，并且需要将历史记录发送给客户端
int history_callback(void* arg,int colCount,char** colValue,char** colName)
{
	// record :  name, date, word 
	int acceptfd;
	MSG_T msg;

	acceptfd = *((int *)arg);
	sprintf(msg.data, "%s , %s", colValue[1], colValue[2]);
	send(acceptfd, &msg, sizeof(MSG_T), 0);

	return 0;
}

// 历史记录查询
int do_history(int sockfd, MSG_T *msg, sqlite3 *db)
{
	char sql[128] = {0};
	char *errmsg;

	//查询数据库
	//会先执行*sql对应的功能命令，然后将结果传递给回调函数，回调函数根据结果再进一步执行
	//关于回调函数详细可参考https://blog.csdn.net/u012351051/article/details/90382391
    sprintf(sql, "select * from record where name = '%s'", msg->name);
	if(sqlite3_exec(db, sql, history_callback,(void *)&sockfd, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("sqlite3 query record done.\n");
	}

	// 所有的记录查询发送完毕之后，给客户端发出一个结束信息
	msg->data[0] = '\0';

	send(sockfd, msg, sizeof(MSG_T), 0);
	
	return 0;
}

int do_client(int acceptfd, sqlite3 *db)
{
	MSG_T recv_msg;

	// 默认是阻塞式接收
	// 如果客户端断开连接 或是 主动发送close关闭连接,recv会返回0
	// recv的返回值：<0 出错; =0 连接关闭; >0 接收到数据大小
	while (recv(acceptfd, &recv_msg, sizeof(MSG_T), 0) > 0)
	{
		switch(recv_msg.type)
		{
			case MSGTYPE_R:
				do_register(acceptfd, &recv_msg, db);
				break;

			case MSGTYPE_L:
				do_login(acceptfd, &recv_msg, db);
				break;

			case MSGTYPE_Q:
				do_query(acceptfd, &recv_msg, db);
				break;

			case MSGTYPE_H:
				do_history(acceptfd, &recv_msg, db);
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
	char ipv4_addr[16];//存放电分格式的ip地址 
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
	sprintf(sql, "create table if not exists user(name text primary key, passwd text);");
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) //创建用户表
	{
		printf("%s\n", errmsg);
		return -1;
	}
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "create table if not exists record(name text, date text, word text);");
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)//创建记录表
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