#include "includes.h"

OMainLoop *gMainLoop = NULL;

extern int OMainLoop_test(void);
extern int OList_test(void);
extern int OBuffer_test(void);

int main(int argc, char *argv[])
{	
	//设置打印级别
	logPrintfSetLogLevel(LOG_APP_TRACE);
	
	//实例化一个事件主循环对象
	gMainLoop = initOMainLoopObj();

	OBuffer_test();
	
    //1、OList测试
	//OList_test();
    //2、事件主循环测试
    OMainLoop_test();
    
	//进入事件主循环，永不返回
	gMainLoop->doEventLoop(gMainLoop);
	
	destroyOMainLoopObj(gMainLoop);
	
	while(1)
	{
		logPrintf(LOG_APP_ERROR, "LOG_APP_ERROR\n");
		logPrintf(LOG_APP_NOTICE, "LOG_APP_NOTICE\n");
		logPrintf(LOG_APP_TRACE, "LOG_APP_TRACE\n");
		sleep(1);
	}
	return 0;
}

