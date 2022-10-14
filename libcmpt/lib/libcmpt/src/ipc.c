/*
 * =====================================================================================
 *
 *       Filename:  ipc.c
 *
 *    Description:  Linux进程间通信方式相关封装
 *
 *        Version:  1.0
 *        Created:  2022年05月5日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  denghengli
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "log_printf.h"
#include "ipc.h"

/********************************************************************
*******************************信号量相关****************************
*********************************************************************/
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/**
 * @brief 打开/创建信号量集,并初始化semnums个信号灯的初值（如果已经存在，则打开；否则进行创建）
 *
 * @param sem_path 信号量关联的本地文件
 * @param sem_nums 信号量集中信号量的个数
 * @param init_value 信号量的初始值（创建时有意义）
 *
 * @return -1:failed; other:信号量ID
 */
int process_sem_open(const char *sem_path, int sem_nums, int init_value)
{
	int semid = 0;
	key_t key = IPC_PRIVATE;
	
	//获取信号量的键值
	if(sem_path)
	{
		if ((key = ftok(sem_path, 'a')) == -1)
		{
			logPrintfError(LOG_APP_ERROR, "open_sem() -> ftok fail");
			return -1;
		}
	}
	
	//创建信号量
	//参数1：键值；参数2：信号量集中信号量个数；参数3：flag：IPC_CREAT|IPC_EXCL| 等权限组合
	semid = semget(key, sem_nums, IPC_CREAT|0777);
	if(semid < 0){
		logPrintfError(LOG_APP_ERROR, "open_sem() -> semget fail");
		return -1;
	}
	
	//设置信号量初始值
	int i;
	union semun un;
    unsigned short *array = (unsigned short*)calloc(sem_nums, sizeof(unsigned short));
    for(i=0; i<sem_nums; i++){
        array[i] = init_value;
    }
    un.array = array;
	//参数1：信号量集id；参数2：semnum，0表示对所有的信号量操作；参数3：控制数据
    if(semctl(semid, 0, SETALL, un) < 0){
        logPrintfError(LOG_APP_ERROR, "open_sem() -> semctl fail");
		free(array);
        return -1;
    }
	
	free(array);
	
	return semid;
}

/**
 * @brief 获取信号量（访问互斥资源之前调用：可能引起阻塞，直到成功获取信号量）
 *
 * @param sem_id 已打开的信号量集ID
 * @param sem_num 信号量集中的信号灯编号（从0开始）
 *
 * @return 0:succes; -1:failed
 */
int process_sem_wait(int sem_id, int sem_num)
{
	/*
	  *定义sembuf类型的机构提数组,放置若干个结构体变量,对应要操作的信号量要做的P或V操作    
	*/
	struct sembuf ops[] = {{sem_num, -1, SEM_UNDO}};
	
	if(semop(sem_id, ops, sizeof(ops)/sizeof(struct sembuf)) < 0){
		logPrintfError(LOG_APP_ERROR, "wait_sem() fail");
		return -1;
	}
	return 0;
}

/**
 * @brief 释放信号量（访问互斥资源之后调用）
 *
 * @param sem_id 已打开的信号量集ID
 * @param sem_num 信号量集中的信号灯编号（从0开始）
 *
 * @return 0:success;-1:failed
 */
int process_sem_post(int sem_id, int sem_num)
{
	/*
     *定义sembuf类型的机构提数组,放置若干个结构体变量,对应要操作的信号量要做的P或V操作    
    */
	struct sembuf ops[] = {{sem_num, 1, SEM_UNDO}};
	
	if(semop(sem_id, ops, sizeof(ops)/sizeof(struct sembuf)) < 0){
		logPrintfError(LOG_APP_ERROR, "post_sem() fail");
		return -1;
	}
	return 0;
}

/**
* @brief 从系统中删除信号量
* 
* @param sem_id 已打开的信号量集ID
* 
* @return 0:success;-1:failed
*/
int process_sem_del(int sem_id)
{
	if(semctl(sem_id, 0, IPC_RMID, NULL) < 0)
	{
        logPrintfError(LOG_APP_ERROR, "del_sem() fail");
        return -1;
    }
	
	return 0;
}



