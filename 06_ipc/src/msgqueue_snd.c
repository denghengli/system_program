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
 * 消息队列发送消息案例
*/
int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("usage: %s key\n", argv[0]);
        exit(1);
    }

    key_t key = atoi(argv[1]);
    printf("key: %d\n", key);

    //创建消息队列
    int msq_id;
    msq_id = msgget(key, IPC_CREAT|IPC_EXCL|0777);
    if(msq_id < 0){
        perror("msgget error");
    }
    printf("msq id: %d\n", msq_id);

    //定义要发送的消息
    MSG m1 = {1, 1, 100};
    MSG m2 = {2, 1, 200};
    MSG m3 = {3, 1, 300};
    MSG m4 = {4, 1, 400};
    MSG m5 = {4, 1, 500};

    //发送消息到消息队列，非阻塞式发送
    if(msgsnd(msq_id, &m1, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0){
        perror("msgsnd error");
    }
    if(msgsnd(msq_id, &m2, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0){
        perror("msgsnd error");
    }
    if(msgsnd(msq_id, &m3, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0){
        perror("msgsnd error");
    }
    if(msgsnd(msq_id, &m4, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0){
        perror("msgsnd error");
    }
    if(msgsnd(msq_id, &m5, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0){
        perror("msgsnd error");
    }

    //发送后去获得消息队列中消息的总数
    struct msqid_ds ds;
    msgctl(msq_id, IPC_STAT, &ds);
    printf("msg total: %ld \n", ds.msg_qnum);
    
    exit(0);
}


