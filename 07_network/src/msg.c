#include "msg.h"
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

/*计算校验码*/
static unsigned char msg_check(myMSG *msg)
{
    unsigned char s = 0;
    int i;

    for(i=0; i<sizeof(msg->head); i++){
        s += msg->head[i];
    }
    return s;
}

/*发送一个基于自定义协议的msg，发送的数据存放在buffer中*/
extern int write_msg(int sockfd, char *buff, size_t len)
{
    myMSG message;
    memset(&message, 0, sizeof(message));
    strcpy(message.head, "denghengli-20210414");
    memcpy(message.buf, buff, len);
    message.checknum = msg_check(&message);

    if(write(sockfd, &message, sizeof(message)) != sizeof(message)){
        return -1;
    }

    return 0;
}

/*读取一个基于自定义协议的msg，读取的数据存放在buffer中*/
extern int read_msg(int sockfd, char *buff, size_t len)
{
    myMSG message;
    memset(&message, 0, sizeof(message));
    size_t size;
    if((size = read(sockfd, &message, sizeof(message))) < 0){
        return -1
    } else if (size == 0) {
        return 0;
    }

    /*校验码验证*/
}