#include "includes.h"

//数据匹配函数
int matchFunc(const void *pBufData, int dataLen, int *validDataOffset, int *validDataLen)
{
	//自定义规则匹配函数 取$$到**之间的数据
	char *begin = NULL, *end = NULL;
	char tmp[4096] = {0};
	snprintf(tmp, dataLen+1, "%s", (char *)pBufData);
	if((begin = strstr(tmp, "$$")) == NULL)
	{
		return -1;
	}
	if((end = strstr(begin, "**")) == NULL)
	{
		return -1;
	}
	*validDataOffset = strlen(tmp) - strlen(begin);
	*validDataLen = strlen(begin) - strlen(end) + 2;
	return 0;
}

int OBuffer_test(void)
{
	char buf[1024];
	char tmp[4096];
	char input_buf[32];
	int input;
	MATCH_RULE_T matchRule;
	BYTES outBytes;
	
	//设置打印级别&库里的打印级别
	logPrintfSetLogLevel(LOG_APP_TRACE);
	
	//初始化一个buffer缓存区
	OBuffer * buffer = initOBufferObj(1024);

	while(1)
	{
		//获取键盘输入的值
		logPrintf(LOG_APP_ERROR, "\033[32m 请输入下列选项进入对应的规则测试\033[0m\n");
		logPrintf(LOG_APP_ERROR, "\033[32m 0:没有规则,取走buffer中已存在的所有数据\033[0m\n");
		logPrintf(LOG_APP_ERROR, "\033[32m 1:满足指定包头和包尾的数据帧\033[0m\n");
		logPrintf(LOG_APP_ERROR, "\033[32m 2:满足指定包头和长度的数据帧\033[0m\n");
		logPrintf(LOG_APP_ERROR, "\033[32m 3:满足指定长度的数据帧\033[0m\n");
		logPrintf(LOG_APP_ERROR, "\033[32m 4:满足指定包尾的数据帧\033[0m\n");
		logPrintf(LOG_APP_ERROR, "\033[32m 5:定制规则,由上层定义匹配函数matchFunc\033[0m\n->");
		
		scanf("%s", input_buf);
		input = atoi(input_buf);
		
		//设置匹配规则
		memset(&matchRule, 0, sizeof(matchRule));
		if(input == R_NO_RULE)//无匹配规则
		{
			matchRule.type = R_NO_RULE;
			logPrintf(LOG_APP_TRACE, "\033[32m ============ bGetDataByRule()=>(match type:R_NO_RULE:%d)===========\033[0m\n", R_NO_RULE); 
		}
		else if(input == R_HEAD_TAIL)//有头有尾
		{
			logPrintf(LOG_APP_TRACE, "\033[32m ============ bGetDataByRule()=>(match type:R_HEAD_TAIL:%d)===========\033[0m\n", R_HEAD_TAIL); 
			matchRule.type = R_HEAD_TAIL;
			char *head = "dd";
			char *tail = "ll";
			matchRule.head.bArray = head;
			matchRule.head.len = strlen(head);
			matchRule.tail.bArray = tail;
			matchRule.tail.len = strlen(tail);
			logPrintf(LOG_APP_TRACE, "\033[32m pack head content:%s\033[0m\n", matchRule.head.bArray); 
			logPrintf(LOG_APP_TRACE, "\033[32m pack tail content:%s\033[0m\n", matchRule.tail.bArray);
		}
		else if(input == R_HEAD_LEN)//有头有长度
		{
			logPrintf(LOG_APP_TRACE, "\033[32m ============ bGetDataByRule()=>(match type:R_HEAD_LEN:%d)===========\033[0m\n", R_HEAD_LEN); 
			matchRule.type = R_HEAD_LEN;
			char *head = "dd";
			matchRule.head.bArray = head;
			matchRule.head.len = strlen(head);
			matchRule.len = 10;
			logPrintf(LOG_APP_TRACE, "\033[32m pack head content:%s\033[0m\n", matchRule.head.bArray); 
			logPrintf(LOG_APP_TRACE, "\033[32m data len:%d\033[0m\n", matchRule.len); 
		}
		else if(input == R_LEN_ONLY)//只有长度
		{
			logPrintf(LOG_APP_TRACE, "\033[32m ============ bGetDataByRule()=>(match type:R_LEN_ONLY:%d)===========\033[0m\n", R_LEN_ONLY);
			matchRule.type = R_LEN_ONLY;
			matchRule.len = 10;
			logPrintf(LOG_APP_TRACE, "\033[32m data len:%d\033[0m\n", matchRule.len); 
		}
		else if(input == R_TAIL_ONLY)//仅有包尾
		{
			logPrintf(LOG_APP_TRACE, "\033[32m ============ bGetDataByRule()=>(match type:R_TAIL_ONLY:%d)===========\033[0m\n", R_TAIL_ONLY);
			matchRule.type = R_TAIL_ONLY;
			char *tail = "ll";
			matchRule.tail.bArray = tail;
			matchRule.tail.len = strlen(tail);
			logPrintf(LOG_APP_TRACE, "\033[32m pack tail content:%s\033[0m\n", matchRule.tail.bArray); 
		}
		else if(input == R_CUSTOM_RULE)//自定义匹配规则
		{
			logPrintf(LOG_APP_TRACE, "\033[32m ============ bGetDataByRule()=>(match type:R_CUSTOM_RULE:%d)===========\033[0m\n", R_CUSTOM_RULE);
			matchRule.type = R_CUSTOM_RULE;
			matchRule.matchFunc = matchFunc;
			logPrintf(LOG_APP_TRACE, "\033[32m pack match rule:取$$到**之间的数据\033[0m\n"); 
		}
		while(1)
		{
			//追加到buffer队列
			memset(buf, 0, sizeof(buf));
			scanf("%s", buf);
			BYTES inBytes;
			inBytes.bArray = buf;
			inBytes.len = strlen(buf);
			buffer->bAppend(buffer, &inBytes);
			
			//获取当前缓冲区内容
			memset(buf, 0, sizeof(buf));
			snprintf(tmp, buffer->bGetDataLen(buffer)+1, "%s", buffer->bGetDataPtr(buffer));
			logPrintf(LOG_APP_ERROR, "current buffer len:%d, content:%s\n", buffer->bGetDataLen(buffer), tmp);
			
			//获取符合匹配规则的数据
			if(0 > buffer->bGetDataByRule(buffer, &matchRule, &outBytes))
			{
				logPrintf(LOG_APP_ERROR, "Warning:don't get right packet\n");
				continue;
			}
			logPrintf(LOG_APP_ERROR, "get right packet:%s\n", outBytes.bArray);
			break;
		}
		//获取当前缓冲区剩余内容
		snprintf(tmp, buffer->bGetDataLen(buffer)+1, "%s", buffer->bGetDataPtr(buffer));
		logPrintf(LOG_APP_ERROR, "current buffer remain len:%d, content:%s\n", buffer->bGetDataLen(buffer), tmp);
		buffer->bClear(buffer);
		
		//释放BYTES空间
		if(outBytes.bArray != NULL)
		{
			free(outBytes.bArray);
			outBytes.bArray = NULL;
		}
	}
	
	return 0;
}



