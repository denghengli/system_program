#ifndef __MSG_H__
#define __MSG_H__

typedef struct 
{
    /*协议头部*/
    char head[10];
    char checknum;//校验码

    //协议体部
    char buffer[512];
}myMSG;

/*发送一个基于自定义协议的msg，发送的数据存放在buffer中*/
extern int write_msg(int sockfd, char *buff, size_t len);
/*读取一个基于自定义协议的msg，读取的数据存放在buffer中*/
extern int read_msg(int sockfd, char *buff, size_t len);

#endif
