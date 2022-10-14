#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shm_account.h"
#include "sem_pv.h"

/*
 * 进程间通信(IPC)--共享内存、信号量 实现互斥案例：
 * 银行存取款
 * 使用到的源文件：sem_pv.c/h shm_account.c/h
*/
int main(void)
{
    //在共享内存中创建银行账户
    int shmid;
    if((shmid = shmget(IPC_PRIVATE, sizeof(Account), IPC_CREAT|IPC_EXCL|0777)) < 0){
        perror("shmget error");
        exit(1);
    }

    //进程共享内存映射(这里跟在父、子 进程中分别进行映射效果是一样的)
    Account *a = (Account*)shmat(shmid, 0, 0);
    if (a == (Account*)-1){
        perror("shmat error");
        exit(1);
    }

    a->code = 10086;
    a->balance = 10000;
    a->semid = sem_I(1, 1);//创建信号量集并初始化(1个信号量/信号灯，初值为1)
    if(a->semid < 0){
        perror("sem_I error");
        exit(1);
    }
    printf("balance: %f\n", a->balance);

    pid_t pid;
    if((pid=fork()) < 0){
        perror("fork error");
        exit(1);
    }
    else if (pid > 0){//父进程
        //父进程执行取款操作
        double amt = withdraw(a, 1000);
        printf("pid(%d) withdraw %f from code %d\n", getpid(), amt, a->code);
        printf("balance: %f\n", a->balance);

        wait(0);//等待子进程回收资源

        sem_D(a->semid);//销毁信号量集
        shmdt(a);//解除映射
        shmctl(shmid, IPC_RMID, NULL);//删除共享内存
    }
    else if (pid == 0){//子进程
        //子进程执行取款操作
        double amt = withdraw(a, 1000);
        printf("pid(%d) withdraw %f from code %d\n", getpid(), amt, a->code);
        printf("balance: %f\n", a->balance);
        shmdt(a);//解除映射
    }

    return 0;
}


