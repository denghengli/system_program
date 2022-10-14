/*
 * =====================================================================================
 *
 *       Filename:  OMainLoop.h
 *
 *    Description:  进程事件主循环
 *
 *        Version:  1.0
 *        Created:  20220505
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  denghegnli
 *
 * =====================================================================================
 */

#ifndef __O_MAIN_LOOP_H__
#define __O_MAIN_LOOP_H__
#include <time.h>
#include <poll.h>

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

/************IO事件相关**************/
//监听可读事件的POLL选项
#define POLLIN_OPT (POLLIN|POLLERR|POLLHUP|POLLNVAL)
//IO事件回调函数类型
typedef void (* IO_EVENT_CALLBACK_FUNC_T)(int fd, unsigned int revents, void *arg);

/************定时器事件相关**************/
//定时类型
enum OTIMER_TYPE
{
	RUN_AT = 0,//某个时间点到期：到期时执行回调函数，并自动删除定时器
	RUN_AFTER = 1, //一段时间之后到期：到期时执行回调函数，并自动删除定时器
	RUN_EVERY = 2 //每隔一段时间到期一次：到期时执行回调函数，并开始下一个周期定时
};
//定时器ID类型
typedef long long OTIMER_ID;
//定时器回调函数类型(参数说明：timer_id定时器唯一标识;atime本次到期的理论时间(距离1970-01-01 00:00:00.000的毫秒数);func_arg添加定时器时指定的回传参数)
typedef void (* OTIMER_CALLBACK_FUNC_T)(OTIMER_ID timer_id, long long atime_Ms, void *arg);

/************事件主循环相关**************/
//前向声明
typedef struct _o_main_loop OMainLoop;
struct PRIVATE_DATA;

/**
 * @brief 初始化一个OMainLoop对象（内部会动态分配内存空间，使用完后必须使用destroyOMainLoopObj接口进行释放）
 *
 * @return OMainLoop对象指针
 */
OMainLoop *initOMainLoopObj(void);

/**
 * @brief 销毁OMainLoop对象,释放空间
 *
 * @param pOMainLoop OMainLoop指针
 */
void destroyOMainLoopObj(OMainLoop *pOMainLoop);

//事件主循环对象描述
struct _o_main_loop
{
	struct PRIVATE_DATA *privateData;//OMainLoop私有数据，外部无法操作;
	
	/**
	 * @brief 添加指定fd的poll事件到事件监听列表中
	 * 注：如果fd已经添加，则会修改fd监听事件类型（rawEvent|event，即追加新的event事件类型）
	 *
	 * @param pOMainLoop OMainLoop指针
	 * @param fd 文件描述符
	 * @param events POLL选项
	 * @param cb_func 事件回调函数
	 * @param cb_arg 事件回调函数的回传参数
	 *
	 * @return 0：success；-1：failed
	 */
	int (* ioEventAdd)(OMainLoop *pOMainLoop, int fd, unsigned int event, IO_EVENT_CALLBACK_FUNC_T cb_func, void *cb_arg);
	
	/**
	 * @brief 从poll事件监听列表中，删除指定fd的事件
	 *
	 * @param pOMainLoop OMainLoop指针
	 * @param fd 文件描述符
	 *
	 * @return 0:success;-1:failed
	 */
	int (* ioEventDel)(OMainLoop *pOMainLoop, int fd);
	
	/**
	 * @brief 打印当前所有监听的文件描述事件
	 *
	 * @param pOMainLoop OMainLoop指针
	 */
	void (* ioEventPrintAll)(OMainLoop *pOMainLoop);
	
	/**
	 * @brief 添加一个定时器(精度秒)
	 *
	 * @param pOMainLoop OMainLoop指针
	 * @param type 定时器类型
	 * @param time_s 定时器时间
	    1. RUN_AT类型时，表示到期的时间点(日历时间:距离1970-01-01 00:00:00的秒数); 
	    2. RUN_AFTER类型时，表示多长时间之后到期(秒数); 
	    3. T_RUN_EVERY类型时，表示每次到期的周期(单位秒))
	 * @param startTime_s 定时器启动的时间点
	 	1. 对于RUN_AT定时器无意义; 
		2. 当大于0时，则为日历时间定时器(距离1970-01-01 00:00:00的秒数)，作为定时器计时的起点;
	 * @param cb_func 定时器到期时的回调函数
	 * @param cb_arg 传递给回调函数的实参
	 *
	 * @return 新增定时器的唯一ID(大于0的整数) 
	 */
	OTIMER_ID (* timerAdd)(OMainLoop *pOMainLoop, enum OTIMER_TYPE type, time_t time_s, time_t startTime_s, OTIMER_CALLBACK_FUNC_T cb_func, void *cb_arg);
	
	/**
	 * @brief 添加一个定时器(精度毫秒)
	 *
	 * @param pOMainLoop OMainLoop指针
	 * @param type 定时器类型
	 * @param time_Ms 定时器时间
	    1. RUN_AT类型时，表示到期的时间点(日历时间:距离1970-01-01 00:00:00.000的毫秒数); 
	    2. RUN_AFTER类型时，表示多长时间之后到期(毫秒数); 
	    3. T_RUN_EVERY类型时，表示每次到期的周期(单位毫秒))
	 * @param startTime_s 定时器启动的时间点
	 	1. 对于RUN_AT定时器无意义; 
		2. 当大于0时，则为日历时间定时器(距离1970-01-01 00:00:00的秒数)，作为定时器计时的起点;
	 * @param cb_func 定时器到期时的回调函数
	 * @param cb_arg 传递给回调函数的实参
	 *
	 * @return 新增定时器的唯一ID(大于0的整数) 
	 */
	OTIMER_ID (* timerAddMs)(OMainLoop *pOMainLoop, enum OTIMER_TYPE type, long long time_Ms, time_t startTime_s, OTIMER_CALLBACK_FUNC_T cb_func, void *cb_arg);

	/**
	 * @brief 删除指定的定时器
	 *
	 * @param pOMainLoop OMainLoop指针
	 *
	 * @param timer_id 定时器ID
	 */
	void (* timerDel)(OMainLoop *pOMainLoop, OTIMER_ID timer_id);
	
	/**
	 * @brief 清除所有的定时器
	 *
	 * @param pOMainLoop OMainLoop指针
	 */
	void (* timerClear)(OMainLoop *pOMainLoop);

	/**
	 * @brief 打印当前所有的定时器状态--用于调试目的
	 *
	 * @param pOMainLoop OMainLoop指针
	 */
	void (* timerPrintAll)(OMainLoop *pOMainLoop);
	
	/**
	 * @brief @brief 事件主循环：循环监听并处理事件（IO事件和定时器事件）,该函数不会退出（注：处理事件时，会屏蔽掉信号）
	 *
	 * @param pOMainLoop OMainLoop指针
	 *
	 */
	void (* doEventLoop)(OMainLoop *pOMainLoop);
	
};

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif//__O_MAIN_LOOP_H__
