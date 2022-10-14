/*
 * =====================================================================================
 *
 *       Filename:  OList.c
 *
 *    Description:  链表容器的实现
 *
 *        Version:  1.0
 *        Created:  20220507
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  denghengli
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>

#include "log_printf.h"
#include "OList.h"
#include "OMainLoop.h"

//io事件节点数据
struct io_event_data
{
    int fd;
    unsigned int event;
    IO_EVENT_CALLBACK_FUNC_T cb_func;
    void *cb_arg;
};

//定时器超时事件节点数据
struct timer_data
{
    int id;
    enum OTIMER_TYPE type;
    time_t time_s;
    time_t startTime_s;
    OTIMER_CALLBACK_FUNC_T cb_func;
    void *cb_arg;
};

//私有数据
struct PRIVATE_DATA
{
    OList *ioEventList;
    OList *timerList;
};

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
static int ioEventAdd(OMainLoop *pOMainLoop, int fd, unsigned int event, IO_EVENT_CALLBACK_FUNC_T cb_func, void *cb_arg)
{
    struct io_event_data event_data;
    struct io_event_data *event_data_ptr;
    OList *list = pOMainLoop->privateData->ioEventList;
    OListIterator iter;

    //检查fd是否已经添加过
    for(iter=list->begin(list); iter!=list->end(list); iter=list->next(list,iter))
    {
        event_data_ptr = (struct io_event_data *)list->value(list, iter);
        if(event_data_ptr->fd == fd)
        {
            event_data_ptr->event = event;
            event_data_ptr->cb_func = cb_func;
            event_data_ptr->cb_arg = cb_arg;
            logPrintf(LOG_APP_ERROR, "Add fd(%d) event(%d) success.\n", fd, event);
            return 0;
        }
    }

    //插入新的节点
    memset(&event_data, 0, sizeof(event_data));
    event_data.fd = fd;
    event_data.event = event;
    event_data.cb_func = cb_func;
    event_data.cb_arg = cb_arg;
    list->append(list, &event_data);
    logPrintf(LOG_APP_ERROR, "Add fd(%d) event(%d) success.\n", fd, event);
    
    return 0;
}

/**
 * @brief 从poll事件监听列表中，删除指定fd的事件
 *
 * @param pOMainLoop OMainLoop指针
 * @param fd 文件描述符
 *
 * @return 0:success;-1:failed
 */
static int ioEventDel(OMainLoop *pOMainLoop, int fd)
{
    struct io_event_data *event_data_ptr;
    OList *list = pOMainLoop->privateData->ioEventList;
    OListIterator iter;

    //检查fd是否已经添加过
    for(iter=list->begin(list); iter!=list->end(list); iter=list->next(list,iter))
    {
        event_data_ptr = (struct io_event_data *)list->value(list, iter);
        if(event_data_ptr->fd == fd)
        {
            logPrintf(LOG_APP_ERROR, "Delete poll event(%d) of fd(%d)\n", event_data_ptr->event, fd);
            list->erase(list, iter);
            return 0;
        }
    }

    logPrintf(LOG_APP_ERROR, "Delete poll of fd(%d) fail!, fd not exist\n", fd);   
    
    return -1;
}

/**
 * @brief 打印当前所有监听的文件描述事件
 *
 * @param pOMainLoop OMainLoop指针
 */
static void ioEventPrintAll(OMainLoop *pOMainLoop)
{
    struct io_event_data *event_data_ptr;
    OList *list = pOMainLoop->privateData->ioEventList;
    OListIterator iter;
    int i = 0;
    
    logPrintf(LOG_APP_ERROR, "Begin: Print listen list of ioEvent ...\n");
    for(iter=list->begin(list); iter!=list->end(list); iter=list->next(list,iter))
    {
        event_data_ptr = (struct io_event_data *)list->value(list, iter);
        logPrintf(LOG_APP_ERROR, "ioEvent%d: fd=%d, events=%d\n", ++i, event_data_ptr->fd, event_data_ptr->event);
    }
    logPrintf(LOG_APP_ERROR, "End: Print listen list of ioEvent\n");
}

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
static OTIMER_ID timerAdd(OMainLoop *pOMainLoop, enum OTIMER_TYPE type, time_t time_s, time_t startTime_s, OTIMER_CALLBACK_FUNC_T cb_func, void *cb_arg)
{
    struct timer_data timer_data;
    OList *list = pOMainLoop->privateData->timerList;
    int timerfd = -1;
    struct itimerspec its = {};

    //创建定时器
    if((timerfd = timerfd_create(CLOCK_REALTIME, 0)) == -1) //TFD_NONBLOCK | TFD_CLOEXEC
    {
        logPrintf(LOG_APP_TRACE, "Timer timerfd_create fail!\n");
        return -1;
    }

    //设置定时器时间参数
    memset(&its, 0, sizeof(struct itimerspec));
    if(type == RUN_AT)
    {
        if(time_s > time(NULL)) {
            its.it_value.tv_sec = time_s - time(NULL);
        }
    }
    else if (type == RUN_AFTER)
    {
        if(time_s) {
            its.it_value.tv_sec = time_s;
        }
        if(startTime_s > time(NULL)) {
            its.it_value.tv_sec += startTime_s - time(NULL);
        }
    }
    else if(type == RUN_EVERY)
    {
        if(time_s) {
            its.it_value.tv_sec = time_s;
            its.it_interval.tv_sec = time_s;
        }
        if(startTime_s > time(NULL)) {
            its.it_value.tv_sec += startTime_s - time(NULL);
        }
    }
    //启动定时器
    if(timerfd_settime(timerfd, 0, &its,  NULL) == -1)
    {
        logPrintf(LOG_APP_TRACE, "Timer timerfd_settime fail!\n");
        return -1;
    }

    //插入新的节点
    memset(&timer_data, 0, sizeof(timer_data));
    timer_data.id = timerfd;
    timer_data.type = type;
    timer_data.time_s = time_s;
    timer_data.startTime_s = startTime_s;
    timer_data.cb_func = cb_func;
    timer_data.cb_arg = cb_arg;
    list->append(list, &timer_data);
    logPrintf(LOG_APP_ERROR, "Timer(id=%d) add success.\n", timerfd);
    
    return timerfd;
}

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
static OTIMER_ID timerAddMs(OMainLoop *pOMainLoop, enum OTIMER_TYPE type, long long time_Ms, time_t startTime_s, OTIMER_CALLBACK_FUNC_T cb_func, void *cb_arg)
{
    struct timer_data timer_data;
    OList *list = pOMainLoop->privateData->timerList;
    int timerfd = -1;
    struct itimerspec its = {};
    struct timespec tp;
    
    //创建定时器
    if((timerfd = timerfd_create(CLOCK_REALTIME, 0)) == -1) //TFD_NONBLOCK | TFD_CLOEXEC
    {
        logPrintf(LOG_APP_TRACE, "Timer timerfd_create fail!\n");
        return -1;
    }

    //设置定时器时间参数
    memset(&its, 0, sizeof(struct itimerspec));
    clock_gettime(CLOCK_REALTIME, &tp);
    if(type == RUN_AT)
    {
        if((time_Ms / 1000) > tp.tv_sec) {
            its.it_value.tv_sec = time_Ms / 1000 - tp.tv_sec;
        }
    }
    else if (type == RUN_AFTER)
    {
        if(time_Ms) {
            its.it_value.tv_sec = time_Ms / 1000;
            its.it_value.tv_nsec = (time_Ms % 1000) * 1000 * 1000;
        }
        if(startTime_s > tp.tv_sec) {
            its.it_value.tv_sec += startTime_s - tp.tv_sec;
        }
    }
    else if(type == RUN_EVERY)
    {
        if(time_Ms) {
            its.it_value.tv_sec = time_Ms / 1000;
            its.it_value.tv_nsec = (time_Ms % 1000) * 1000 * 1000;
            
            its.it_interval.tv_sec = its.it_value.tv_sec;
            its.it_interval.tv_nsec = its.it_value.tv_nsec;
        }
        if(startTime_s > tp.tv_sec) {
            its.it_value.tv_sec += startTime_s - tp.tv_sec;
        }
    }
    //启动定时器
    if(timerfd_settime(timerfd, 0, &its,  NULL) == -1)
    {
        logPrintf(LOG_APP_TRACE, "Timer timer_settime fail!\n");
        return -1;
    }

    //插入新的节点
    memset(&timer_data, 0, sizeof(timer_data));
    timer_data.id = timerfd;
    timer_data.type = type;
    timer_data.time_s = time_Ms / 1000;
    timer_data.startTime_s = startTime_s;
    timer_data.cb_func = cb_func;
    timer_data.cb_arg = cb_arg;
    list->append(list, &timer_data);
    logPrintf(LOG_APP_ERROR, "Timer(id=%d) add success.\n", timerfd);
    
    return 0;
}

/**
 * @brief 删除指定的定时器
 *
 * @param pOMainLoop OMainLoop指针
 *
 * @param timer_id 定时器ID
 */
static void timerDel(OMainLoop *pOMainLoop, OTIMER_ID timer_id)
{
    struct timer_data *timer_data_ptr;
    OList *list = pOMainLoop->privateData->timerList;
    OListIterator iter;
    struct itimerspec itspec;
    
    //检查fd是否已经添加过
    for(iter=list->begin(list); iter!=list->end(list); iter=list->next(list,iter))
    {
        timer_data_ptr = (struct timer_data *)list->value(list, iter);
        if(timer_data_ptr->id == timer_id)
        {
            //关闭定时器
            memset(&itspec, 0, sizeof(struct itimerspec));
            timerfd_settime(timer_data_ptr->id, 0, &itspec,  NULL);
            list->erase(list, iter);
            logPrintf(LOG_APP_ERROR, "Delete timer: id=%d\n", timer_id);
            break;
        }
    }
}

/**
 * @brief 清除所有的定时器
 *
 * @param pOMainLoop OMainLoop指针
 */
static void timerClear(OMainLoop *pOMainLoop)
{
    struct timer_data *timer_data_ptr;
    OList *list = pOMainLoop->privateData->timerList;
    OListIterator iter;
    struct itimerspec itspec;
    
    //关闭定时器
    memset(&itspec, 0, sizeof(struct itimerspec));
    for(iter=list->begin(list); iter!=list->end(list); iter=list->next(list,iter))
    {
        timer_data_ptr = (struct timer_data *)list->value(list, iter);
        timerfd_settime(timer_data_ptr->id, 0, &itspec,  NULL);
    }

    list->clear(list);
}

/**
 * @brief 打印当前所有的定时器状态--用于调试目的
 *
 * @param pOMainLoop OMainLoop指针
 */
static void timerPrintAll(OMainLoop *pOMainLoop)
{
    struct timer_data *timer_data_ptr;
    OList *list = pOMainLoop->privateData->timerList;
    OListIterator iter;
    int i = 0;
    
    logPrintf(LOG_APP_ERROR, "Begin: Print listen list of timer ...\n");
    for(iter=list->begin(list); iter!=list->end(list); iter=list->next(list,iter))
    {
        timer_data_ptr = (struct timer_data *)list->value(list, iter);
        logPrintf(LOG_APP_ERROR, "Timer%d: id=%d\n", ++i, timer_data_ptr->id);
    }
    logPrintf(LOG_APP_ERROR, "End: Print listen list of timer\n");
}

/**
 * @brief @brief 事件主循环：循环监听并处理事件（IO事件和定时器事件）,该函数不会退出（注：处理事件时，会屏蔽掉信号）
 *
 * @param pOMainLoop OMainLoop指针
 *
 */
static void doEventLoop(OMainLoop *pOMainLoop)
{
    OList *ioEventList = pOMainLoop->privateData->ioEventList;
    OList *timerList = pOMainLoop->privateData->timerList;
    struct io_event_data *io_event_data_ptr;
    struct timer_data *timer_data_ptr;
    struct pollfd poll_fds[1024];
    int fd_num = 0, i, j, ret;
    int match_ok;
    long long exp;

    //信号屏蔽
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);
    
    while(1)
    {
        fd_num = 0;
        //io事件监听
        for(i=0; i<ioEventList->size(ioEventList); i++)
        {
            io_event_data_ptr = (struct io_event_data *)ioEventList->at(ioEventList, i);
            poll_fds[fd_num].fd = io_event_data_ptr->fd;
            poll_fds[fd_num].events = io_event_data_ptr->event;//监视的事件
            poll_fds[fd_num].revents = 0;//返回的事件
            fd_num += 1;
        }
        //定时器超时事件监听
        for(i=0; i<timerList->size(timerList); i++)
        {
            timer_data_ptr = (struct timer_data *)timerList->at(timerList, i);
            poll_fds[fd_num].fd = timer_data_ptr->id;
            poll_fds[fd_num].events = POLLIN;//监视的事件
            poll_fds[fd_num].revents = 0;//返回的事件
            fd_num += 1;
        }

        if(!fd_num)
        {
            sleep(1);
        }
        else
        {
            ret = poll(&poll_fds[0], fd_num, 1000);
    		switch (ret) 
    		{
    		case 0:
    		    //logPrintf(LOG_APP_ERROR, "doEventLoop read timeout!\n");
    			break;
    		
    		case -1:
    		    logPrintf(LOG_APP_ERROR, "doEventLoop read error!\n");
    			break;

    		default:
    		    for(i=0; i<fd_num; i++)
    		    {
    		        if(poll_fds[i].revents)
    		        {
                        match_ok = 0;
        		        //匹配io事件
                        for(j=0; j<ioEventList->size(ioEventList); j++)
                        {
                            io_event_data_ptr = (struct io_event_data *)ioEventList->at(ioEventList, j);
                            if(poll_fds[i].fd == io_event_data_ptr->fd)
                            {
                                logPrintf(LOG_APP_ERROR, "Detected IO event: fd=%d, revents=%d, wait events=%d !\n", poll_fds[i].fd, poll_fds[i].revents, poll_fds[i].events);
                                if(io_event_data_ptr->cb_func)
                                {
                                    io_event_data_ptr->cb_func(poll_fds[i].fd, poll_fds[i].revents, io_event_data_ptr->cb_arg);
                                }
                                match_ok = 1;
                                break;
                            }
                        }
                        //匹配定时器超时事件
                        if(!match_ok)
                        {
                            for(j=0; j<timerList->size(timerList); j++)
                            {
                                timer_data_ptr = (struct timer_data *)timerList->at(timerList, j);
                                if(poll_fds[i].fd == timer_data_ptr->id)
                                {
                                    //logPrintf(LOG_APP_ERROR, "Detected Timer timeout event: id=%d, revents=%d, wait events=%d !\n", poll_fds[i].fd, poll_fds[i].revents, poll_fds[i].events);
                                    read(poll_fds[i].fd, &exp, sizeof(long long));
                                    if(timer_data_ptr->cb_func)
                                    {
                                        timer_data_ptr->cb_func(poll_fds[i].fd, time(NULL)*1000, timer_data_ptr->cb_arg);
                                    }
                                    match_ok = 1;
                                    break;
                                }
                            }
                        }
    		        }
    		    }
                break;
            }
        }
    }
}



/**
 * @brief 初始化一个OMainLoop对象（内部会动态分配内存空间，使用完后必须使用destroyOMainLoopObj接口进行释放）
 *
 * @return OMainLoop对象指针
 */
OMainLoop *initOMainLoopObj(void)
{
    OMainLoop *tmpOMainLoop = NULL; 
    FILE *fp;
	char buf[512] = {0};

    //获取进程可以同时打开的最大文件句柄数
	fp = popen("ulimit -n", "r");
	memset(buf, 0, sizeof(buf));
	fgets(buf, sizeof(buf), fp);
	pclose(fp);
    logPrintf(LOG_APP_ERROR, "The system max fd limit is %s", buf);

    tmpOMainLoop = calloc(1, sizeof(OMainLoop));
    if(!tmpOMainLoop)
    {
        logPrintf(LOG_APP_ERROR, "Init OMainLoop object fail!\n");
        return NULL;
    }

    tmpOMainLoop->privateData = calloc(1, sizeof(struct PRIVATE_DATA));
    if(!tmpOMainLoop->privateData)
    {
        logPrintf(LOG_APP_ERROR, "Init OMainLoop object fail!\n");
        free(tmpOMainLoop);
        return NULL;
    }
    //操作方法赋值
    tmpOMainLoop->ioEventAdd = ioEventAdd;
    tmpOMainLoop->ioEventDel = ioEventDel;
    tmpOMainLoop->ioEventPrintAll = ioEventPrintAll;
    tmpOMainLoop->timerAdd = timerAdd;
    tmpOMainLoop->timerAddMs = timerAddMs;
    tmpOMainLoop->timerDel = timerDel;
    tmpOMainLoop->timerClear = timerClear;
    tmpOMainLoop->timerPrintAll = timerPrintAll;
    tmpOMainLoop->doEventLoop = doEventLoop;
    
    //初始化链表对象
    tmpOMainLoop->privateData->ioEventList = initOListObj(sizeof(struct io_event_data));
    tmpOMainLoop->privateData->timerList = initOListObj(sizeof(struct timer_data));
    
    logPrintf(LOG_APP_ERROR, "Init OMainLoop object successed.\n");

    return tmpOMainLoop;
}

/**
 * @brief 销毁OMainLoop对象,释放空间
 *
 * @param pOMainLoop OMainLoop指针
 */
void destroyOMainLoopObj(OMainLoop *pOMainLoop)
{
    destroyOListObj(pOMainLoop->privateData->ioEventList);
    destroyOListObj(pOMainLoop->privateData->timerList);

    free(pOMainLoop->privateData);
    pOMainLoop->privateData = NULL;
    free(pOMainLoop);
    pOMainLoop = NULL;

    logPrintf(LOG_APP_ERROR, "destroy OMainLoop object successed.\n");
}


