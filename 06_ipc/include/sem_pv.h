#ifndef __SEM_PV__
#define __SEM_PV__

//创建信号量集，并初始化semnums个信号灯/信号量的值为value
extern int sem_I(int semnums, int value);

//对信号量集(smid)中的信号灯(semnum)做P减(value)操作
extern void sem_P(int semid, int semnum, int value);

//对信号量集(smid)中的信号灯(semnum)做V加(value)操作
extern void sem_V(int semid, int semnum, int value);

//销毁信号量集
extern void sem_D(int semid);

#endif