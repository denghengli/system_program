#include "includes.h"


//创建文件
int creatfile(const char *path)
{
	int fd = -1;

	fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if(fd < 0)
	{
		logPrintf(LOG_APP_ERROR, "open failed!\n");
		exit(-1);
	}
	return fd;
}

void poll_test(const char *path)
{
    int fd = -1, ret;
    struct pollfd poll_fds[1];
    char buf[512] = {0};
    
	fd = open(path, O_RDWR | O_CREAT | O_TRUNC);
	if(fd < 0)
	{
		logPrintf(LOG_APP_ERROR, "open failed!\n");
		exit(-1);
	}
            
	while(1)
	{
        poll_fds[0].fd = fd;
        poll_fds[0].events = POLLIN;//监视的事件
        poll_fds[0].revents = 0;//返回的事件
	    logPrintf(LOG_APP_ERROR, "poll start, revents=%d\n", poll_fds[0].revents);
		ret = poll(&poll_fds[0], 1, 1000);
		switch (ret) 
		{
		case 0:
			logPrintf(LOG_APP_TRACE, "read timeout ! \r\n");
			break;
		
		case -1:
			logPrintf(LOG_APP_ERROR, "read error \n");
			break;

		default:
		    logPrintf(LOG_APP_ERROR, "ret=%d, revents=%d", ret, poll_fds[0].revents);
        	if (read(fd, buf, sizeof(buf)) < 0)
        	{
        		logPrintf(LOG_APP_ERROR, "read failed!\n");
        		return;
        	}
        	logPrintf(LOG_APP_ERROR, "data:%s, len:%d", buf, strlen(buf));
            break;
        }
	}
}


/************************************定时器主循环相关**************************************/

OTIMER_ID timerEvery1, timerEvery2, timerEvery3, timerAfter1, timerAfter2; 
void everyCallback(OTIMER_ID timer_id, long long atime_Ms, void *arg)
{
    time_t rawtime = atime_Ms / 1000;
    struct tm *now;
    char buf[128] = {0};
    
    now = localtime(&rawtime);
    strftime(buf, 128, "%Y%m%d_%H%M%S", now);//以年月日_时分秒的形式表示当前时间
    
    logPrintf(LOG_APP_TRACE, "everyCallback(%d) timeout, atime_Ms=%s\n", (int)arg, buf);
}

void deleteTimer(OTIMER_ID timer_id, long long atime_Ms, void *arg)
{
    time_t rawtime = atime_Ms / 1000;
    struct tm *now;
    char buf[128] = {0};
    
    now = localtime(&rawtime);
    strftime(buf, 128, "%Y%m%d_%H%M%S", now);//以年月日_时分秒的形式表示当前时间

    logPrintf(LOG_APP_TRACE, "deleteTimer timeout, delete timerEvery1, atime_Ms=%s\n", buf);
    //删除循环定时器 1
    gMainLoop->timerDel(gMainLoop, timerEvery1);
    gMainLoop->timerPrintAll(gMainLoop);
}

void deleteAllTimer(OTIMER_ID timer_id, long long atime_Ms, void *arg)
{
    time_t rawtime = atime_Ms / 1000;
    struct tm *now;
    char buf[128] = {0};
    
    now = localtime(&rawtime);
    strftime(buf, 128, "%Y%m%d_%H%M%S", now);//以年月日_时分秒的形式表示当前时间
    
    logPrintf(LOG_APP_TRACE, "deleteAllTimer timeout, delete all timer, atime_Ms=%s\n", buf);
    //删除所有定时器
    gMainLoop->timerClear(gMainLoop);
    gMainLoop->timerPrintAll(gMainLoop);
}



/************************************IO事件主循环相关**************************************/
//创建fifo管道
int creatfifo(const char *path)
{
	int fd = -1;
	if(0 < mkfifo(path, O_CREAT))
	{
		logPrintf(LOG_APP_ERROR, "mkfifo failed!\n");
		exit(-1);
	}
	fd = open(path, O_RDONLY|O_NONBLOCK);
	if(fd < 0)
	{
		logPrintf(LOG_APP_ERROR, "open failed!\n");
		exit(-1);
	}
	return fd;
}

//事件回调函数
void eventsCallback(int fd, unsigned int revents, void *arg)
{
	char buf[512] = {0};
	if (read(fd, buf, sizeof(buf)) < 0)
	{
		logPrintf(LOG_APP_ERROR, "read failed!\n");
		return;
	}

	logPrintf(LOG_APP_ERROR, "data:%s", buf, strlen(buf));

	//从poll事件监听列表中，删除此描述符的事件
	gMainLoop->ioEventDel(gMainLoop, fd);
	//打印当前监听的描述符
	gMainLoop->ioEventPrintAll(gMainLoop);
	close(fd);
}

int OMainLoop_test(void)
{
    //poll_test("/home/dhl/winshare/systemProgram/libcmpt/bin/test1.txt");

    //创建fifo
    int fd1 = creatfifo("/home/dhl/winshare/systemProgram/libcmpt/bin/fifo1");
    int fd2 = creatfifo("/home/dhl/winshare/systemProgram/libcmpt/bin/fifo2");
    int fd3 = creatfifo("/home/dhl/winshare/systemProgram/libcmpt/bin/fifo3");
    int fd4 = creatfifo("/home/dhl/winshare/systemProgram/libcmpt/bin/fifo4");

    //添加监听可读事件
    gMainLoop->ioEventAdd(gMainLoop, fd1, POLLIN, eventsCallback, NULL);
    gMainLoop->ioEventAdd(gMainLoop, fd2, POLLIN, eventsCallback, NULL);
    gMainLoop->ioEventAdd(gMainLoop, fd3, POLLIN, eventsCallback, NULL);
    gMainLoop->ioEventAdd(gMainLoop, fd4, POLLIN, eventsCallback, NULL);
    //打印当前监听的描述符
    gMainLoop->ioEventPrintAll(gMainLoop);

    
    /************************************定时器主循环相关**************************************/
    //添加秒级 RUN_EVERY 定时器（每 10 秒执行一次）
    timerEvery1 = gMainLoop->timerAdd(gMainLoop, RUN_EVERY, 5, time(NULL)+10, everyCallback, (void *)1);
    //添加秒级 RUN_EVERY 定时器（每 10 秒执行一次）
    timerEvery2 = gMainLoop->timerAdd(gMainLoop, RUN_EVERY, 5, time(NULL)+10, everyCallback, (void *)2);
    //添加秒级 RUN_EVERY 定时器（每 10 秒执行一次）
    timerEvery3 = gMainLoop->timerAdd(gMainLoop, RUN_EVERY, 5, 0, everyCallback, (void *)3);
    //添加毫秒级 RUN_AFTER 定时器（30 秒后执行）
    timerAfter1 = gMainLoop->timerAddMs(gMainLoop, RUN_AFTER, 20000, 0, deleteTimer, NULL);
    //添加毫秒级 RUN_AFTER 定时器（60 秒后执行）
    timerAfter2 = gMainLoop->timerAddMs(gMainLoop, RUN_AFTER, 30000, 0, deleteAllTimer, NULL);
    //打印定时器超时事件
    gMainLoop->timerPrintAll(gMainLoop);
	
	return 0;
}


