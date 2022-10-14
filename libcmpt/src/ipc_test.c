#include "includes.h"

//对共享资源进行保护（2选1）
//使用进程信号量的方式，这种是用于进程间通讯的，在这里并不合适，process_sem_post在获取不到时会将调用的进程挂起
#define MUTEX_PROTECT_PROCESS_SEM 1 
//使用线程信号量的方式，用于在进程内部多线程的方式，比较合适
#define MUTEX_PROTECT_PTHREAD_SEM 0 

sem_t io_event_sem; /* 线程信号量，互斥访问信号量 */
int io_event_semid; /* 进程信号量，互斥访问信号量 */

void ipc_p(void)
{
#if(MUTEX_PROTECT_PTHREAD_SEM)
	sem_wait(&io_event_sem);
#endif
#if(MUTEX_PROTECT_PROCESS_SEM)
	process_sem_wait(io_event_semid, 0);
#endif
}

void ipc_v(void)
{
#if(MUTEX_PROTECT_PTHREAD_SEM)
        sem_post(&io_event_sem);
#endif
#if(MUTEX_PROTECT_PROCESS_SEM)
        process_sem_post(io_event_semid, 0);
#endif
}

void ipc_creat(void)
{
	FILE *fp;
	char buf[512] = {0};

    //初始化信号量，用于互斥
#if(MUTEX_PROTECT_PTHREAD_SEM)
	sem_init(&io_event_sem, 0, 1);
#endif

#if(MUTEX_PROTECT_PROCESS_SEM)
	fp = popen("pwd", "r");
	memset(buf, 0, sizeof(buf));
	fgets(buf, sizeof(buf), fp);
	pclose(fp);
	buf[strlen(buf) - 1] = '\0';
	
	io_event_semid = process_sem_open(buf, 1, 1);
	if (io_event_semid < 0)
	{
		logPrintf(LOG_APP_ERROR, "ipc_creat fail!\n");
    }
#endif
}

