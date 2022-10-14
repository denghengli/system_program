#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include "sem_pv.h"

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

//创建信号量集，并初始化semnums个信号灯/信号量的值为value
int sem_I(int semnums, int value)
{
    int semid = semget(IPC_PRIVATE, semnums, IPC_CREAT|IPC_EXCL|0777);
    if (semid < 0){
        return -1;
    }
    
    int i;
    unsigned short *array = (unsigned short*)calloc(semnums, sizeof(unsigned short));
    for(i=0; i<semnums; i++){
        array[i] = value;
    }
    
    //初始化信号量集中所有信号灯的初值
    union semun un;
    un.array = array;
    if(semctl(semid, 0, SETALL, un) < 0){
        perror("semctl error");
        return -1;
    }

    free(array);

    return semid;
}

//对信号量集(smid)中的信号灯(semnum)做P减(value)操作
void sem_P(int semid, int semnum, int value)
{
    assert(value >= 0);

    /*
     *定义sembuf类型的机构提数组,放置若干个结构体变量,对应要操作的信号量要做的P或V操作    
    */
   struct sembuf ops[] = {{semnum, -value, SEM_UNDO}};
   if(semop(semid, ops, sizeof(ops)/sizeof(struct sembuf)) < 0){
       perror("semop error");
   }
}

//对信号量集(smid)中的信号灯(semnum)做V加(value)操作
void sem_V(int semid, int semnum, int value)
{
    assert(value >= 0);

    /*
     *定义sembuf类型的机构提数组,放置若干个结构体变量,对应要操作的信号量要做的P或V操作    
    */
   struct sembuf ops[] = {{semnum, value, SEM_UNDO}};
   if(semop(semid, ops, sizeof(ops)/sizeof(struct sembuf)) < 0){
       perror("semop error");
   }
}

//销毁信号量集
void sem_D(int semid)
{
    if(semctl(semid, 0, IPC_RMID, NULL) < 0){
        perror("semctl error");
    }
}