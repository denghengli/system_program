/*
 * =====================================================================================
 *
 *       Filename:  ipc.h
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
#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <sys/stat.h>

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
*******************************信号量相关****************************
*********************************************************************/
/**
 * @brief 打开/创建信号量集,并初始化semnums个信号灯的初值（如果已经存在，则打开；否则进行创建）
 *
 * @param sem_path 信号量关联的本地文件
 * @param sem_nums 信号量集中信号量的个数
 * @param init_value 信号量的初始值（创建时有意义）
 *
 * @return -1:failed; other:信号量ID
 */
int process_sem_open(const char *sem_path, int sem_nums, int init_value);

/**
 * @brief 获取信号量（访问互斥资源之前调用：可能引起阻塞，直到成功获取信号量）
 *
 * @param sem_id 已打开的信号量集ID
 * @param sem_num 信号量集中的信号灯编号（从0开始）
 *
 * @return 0:succes; -1:failed
 */
int process_sem_wait(int sem_id, int sem_num);

/**
 * @brief 释放信号量（访问互斥资源之后调用）
 *
 * @param sem_id 已打开的信号量集ID
 * @param sem_num 信号量集中的信号灯编号（从0开始）
 *
 * @return 0:success;-1:failed
 */
int process_sem_post(int sem_id, int sem_num);

/**
* @brief 从系统中删除信号量
* 
* @param sem_id 已打开的信号量集ID
* 
* @return 0:success;-1:failed
*/
int process_sem_del(int sem_id);


#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif
