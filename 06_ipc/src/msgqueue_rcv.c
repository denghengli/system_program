#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>

/*自定义消息*/
typedef struct 
{
    long type;  //消息类型,必须有

    //消息数据,类型可以自行定义
    int start;  
    int end;    
}MSG;


/*
 * 消息队列接受消息案例
*/
int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("usage: %s key type\n", argv[0]);
        exit(1);
    }

    key_t key = atoi(argv[1]);
    long type = atoi(argv[2]);

    //打开消息队列
    int msq_id;
    msq_id = msgget(key, 0777);
    if(msq_id < 0){
        perror("msgget error");
    }
    printf("msq id: %d\n", msq_id);

    //从消息队列中接受指定类型的消息，非阻塞式获取
    MSG msg;
    if(msgrcv(msq_id, &msg, sizeof(MSG)-sizeof(long), type, IPC_NOWAIT) < 0){
        perror("msgrcv error");
    }
    printf("type:%ld, start:%d end:%d\n", msg.type, msg.start, msg.end);

    exit(0);
}
