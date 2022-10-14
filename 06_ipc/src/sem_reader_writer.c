#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

//读者和写着的共享资源
typedef struct 
{
    int val;
    int semid;
}Storage;

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int rwer_init(Storage *s)
{
    assert(s != NULL);

    //创建信号量集（包含2个信号量）
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT|IPC_EXCL|0777);
    if (semid < 0){
        return -1;
    }
    s->semid = semid;

    //对信号量进行初始化,2个信号量的初值设置为0
    union semun un;
    unsigned short array[2] = {0,0};
    un.array = array;
    if(semctl(s->semid, 0, SETALL, un) < 0){
        perror("semctl error");
        exit(1);
    }

    return 0;
}

void rwer_destroy(Storage *s)
{
    assert(s != NULL);

    if(semctl(s->semid, 0, IPC_RMID, NULL) < 0){
        perror("semctl error");
        exit(1);
    }
}

void rwer_write(Storage *s, int val)
{
    s->val = val;
    printf("%d write %d\n", getpid(), val);

    //设置0号信号量(s1)作V加操作
    struct sembuf ops_v[1] = {{0, 1, SEM_UNDO}};
    //设置1号信号量(s2)作P减操作
    struct sembuf ops_p[1] = {{1, -1, SEM_UNDO}};

    //V加(s1)
    if(semop(s->semid, ops_v, 1) < 0){
       perror("semop error");
   }
   //P减(s2)
    if(semop(s->semid, ops_p, 1) < 0){
       perror("semop error");
   }
}

void rwer_read(Storage *s)
{
    //设置0号信号量(s1)作P减操作
    struct sembuf ops_p[1] = {{0, -1, SEM_UNDO}};
    //设置1号信号量(s2)作V加操作
    struct sembuf ops_v[1] = {{1, 1, SEM_UNDO}};

    //P减(s1)
    if(semop(s->semid, ops_p, 1) < 0){
       perror("semop error");
   }
    printf("%d read %d\n", getpid(), s->val);
   //V加(s2)
    if(semop(s->semid, ops_v, 1) < 0){
       perror("semop error");
   }
}

/*
 * 进程间通信(IPC)--共享内存、信号量 实现同步案例：
 * 信号量实现读者与写者
*/
int main(void)
{
    //将共享资源创建在共享内存中
    int shmid;
    if((shmid=shmget(IPC_PRIVATE, sizeof(Storage), IPC_CREAT|IPC_EXCL|0777)) < 0){
        perror("shmget error");
        exit(1);
    }

    //父进程进行共享内存映射
    Storage *s = (Storage*)shmat(shmid, 0, 0);
    if(s == (Storage*)-1){
        perror("shmat error");
        exit(1);
    }

    //创建信号量集并初始化
    rwer_init(s);

    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    }
    else if(pid > 0){ //父进程(写者)
        int i = 1;
        for(; i<=100; i++){
            rwer_write(s, i);
            usleep(200000);
        }

        wait(0);//等待子进程接受回收资源

        rwer_destroy(s);//删除信号量集
        shmdt(s);//解除映射
        shmctl(shmid, IPC_RMID, NULL);//删除共享内存
    }
    else if (pid == 0){//子进程(读者)
        int i = 1;
        for(; i<=100; i++){
            rwer_read(s);
            usleep(200000);
        }
    }

    return 0;
}