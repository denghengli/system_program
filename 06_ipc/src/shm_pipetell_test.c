#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shm_pipetell.h"

/*
 * 进程间通信(IPC)--共享内存 案例：
 * 共享内存写入、读取
 * 使用到的源文件：shm_pipetell.c/h,这里还没学到信号量，所以先用管道来实现两个进程之间的同步
*/
int main(void)
{
    //创建共享内存
    int shmid;
    if((shmid=shmget(IPC_PRIVATE, 1024, IPC_CREAT|IPC_EXCL|0777)) < 0){
        perror("shmget error");
        exit(1);
    }

    //管道初始化，利用管道实现父进程与子进程之间的同步
    pipe_init();

    pid_t pid;
    if((pid=fork()) < 0){
        perror("fork error");
        exit(1);
    }
    else if (pid > 0){ //父进程
        //进行共享内存的映射
        int *pi = (int*)shmat(shmid, 0, 0);
        if(pi == (int*)-1){
            perror("shmat error");
            exit(1);
        }
        //往共享内存写入数据
        *pi = 100;
        *(pi+1) = 200;
        //操作完毕接触共享内存的映射
        shmdt(pi);
        //通知子进程读取数据
        notify_pipe();

        destroy_pipe();
        wait(0);//回收子进程
    }
    else if (pid == 0){//子进程
        //子进程阻塞，等待父进程先往共享内存中写入数据
        wait_pipe();
        //进程共享内存的映射
        int *pi = (int*)shmat(shmid, 0, 0);
        if(pi == (int*)-1){
            perror("shmat error");
            exit(1);
        }
        //读取共享内存数据
        printf("start:%d, end:%d\n", *pi, *(pi+1));
        //操作完毕接触共享内存的映射
        shmdt(pi);

        //删除共享内存
        shmctl(shmid, IPC_RMID, NULL);

        destroy_pipe();
    }

    return 0;
}