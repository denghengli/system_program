/*
 * =====================================================================================
 *
 *       Filename:  log_printf.h
 *
 *    Description:  日志打印模块
 *
 *        Version:  1.0
 *        Created:  2022年03月20日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  denghengli
 *
 * =====================================================================================
 */
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include "log_printf.h"
 
static LOG_OUTPUT_T logOutput = LOG_OUTPUT_ALL; //日志输出方式
static LOG_LEVEL_T logLevel = LOG_APP_NOTICE; //日志输出级别

struct log_level_identify
{
	LOG_LEVEL_T level;
	char *identify;
};

const struct log_level_identify log_level_identify_desc[] = 
{
	{LOG_APP_ERROR, "**"},
	{LOG_APP_NOTICE, "$$"},
	{LOG_APP_TRACE, "##"},
};
	
/**
 * @brief 获取系统时间
 *
 * @param buf 系统时间(出参)，格式为20222032-182130.100，时间精确到毫秒
 * @param len buf的长度(入参)
 *
 * @return 0:success <0:fail
 */
static int getSysTime(char *buf, int len)
{
	//time_t nowtime = time(0);
	struct tm time;
	struct timeval tv;
	
	memset(&time, 0, sizeof(time));
	memset(&tv, 0, sizeof(tv));
	
	if (gettimeofday(&tv, NULL) < 0)
	{
		perror("gettimeofday() error");
		return -1;
	}
	
	localtime_r(&tv.tv_sec, &time);
	snprintf(buf, len, "%04d%02d%02d-%02d%02d%02d.%03d", (time.tm_year + 1900), (time.tm_mon + 1), time.tm_mday,
						time.tm_hour, time.tm_min, time.tm_sec, (int)(tv.tv_usec/1000));
						
	return 0;
}

/**
 * @brief 获取当前程序的名称
 *
 * @param buf 程序名称(出参)
 * @param len buf的长度(入参)
 *
 * @return 0:success <0:fail
 */
static int getCurAppName(char *buf, int len)
{
	char path[1024] = {0};
	
	//以下连接为 /proc/self/exe 的介绍
	//https://blog.csdn.net/cjdgg/article/details/119860355
	if (readlink("/proc/self/exe", path, sizeof(path)) <= 0)
	{
		perror("readlink() error");
		return -1;
	}
	//获取路径中最后一个/字符的位置
	snprintf(buf, len, "%s", strrchr(path, '/'));
	
	return 0;
}	

 /**
 * @brief 设置日志输出方式
 *
 * @param log_output
 */
void logPrintfSetLogOutput(LOG_OUTPUT_T log_output)
{
	logOutput = log_output;
}

/**
 * @brief 设置日志的打印级别
 *
 * @param log_level
 */
void logPrintfSetLogLevel(LOG_LEVEL_T log_level)
{
	char appName[512] = {0};
	
	logLevel = log_level;
	
	if(!getCurAppName(appName, sizeof(appName)))
	{
		openlog(appName, LOG_PID, LOG_USER);
	}
}

/**
 * @brief 设置日志的打印级别
 *
 * @param log_level_str "dummy/release/debug/develop/LOG_APP_ERROR/.../LOG_APP_TRACE"
 */
void logPrintfSetLogLevelByStr(const char *log_level_str)
{
	if (!strcmp(log_level_str, "dummy")) logLevel = LOG_DUMMY;
	else if (!strcmp(log_level_str, "release")) logLevel = LOG_APP_ERROR;
	else if (!strcmp(log_level_str, "debug")) logLevel = LOG_APP_NOTICE;
	else if (!strcmp(log_level_str, "develop")) logLevel = LOG_APP_TRACE;
	else if (!strcmp(log_level_str, "LOG_APP_ERROR")) logLevel = LOG_APP_ERROR;
	else if (!strcmp(log_level_str, "LOG_APP_NOTICE")) logLevel = LOG_APP_NOTICE;
	else if (!strcmp(log_level_str, "LOG_APP_TRACE")) logLevel = LOG_APP_TRACE;
}
	
/**
 * @brief 获取当前日志的打印级别
 *
 * @return LOG_LEVEL_T
 */
LOG_LEVEL_T logPrintfGetLogLevel(void)
{
	return logLevel;
}

/**
 * @brief 打印日志
 *
 * @param log_level 日志级别
 */
void logPrintf(LOG_LEVEL_T log_level, const char *format, ...)
{
	char buf[4096] = {0};
	char appName[512] = {0};
	char time[512] = {0};
	va_list args;
	
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	
	if (log_level > LOG_DUMMY && log_level <= logLevel)
	{
		if(!getSysTime(time, sizeof(time)) && !getCurAppName(appName, sizeof(appName)))
		{
			if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) {
				printf("%s[%s][%d][%s]:%s", log_level_identify_desc[log_level].identify, appName, getpid(), time, buf);
			}
			if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) {
				syslog(LOG_INFO, "%s[%s][%d][%s]:%s", log_level_identify_desc[log_level].identify, appName, getpid(), time, buf);
			}
		}
	}
}

/**
 * @brief 打印日志,不带前面提示的版本
 *
 * @param log_level 日志级别
 */
void logPrintfNoPrompt(LOG_LEVEL_T log_level, const char *format, ...)
{
	char buf[4096] = {0};
	va_list args;
	
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	
	if (log_level > LOG_DUMMY && log_level <= logLevel)
	{
		if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) {
			printf("%s", buf);
		}
		if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) {
			syslog(LOG_INFO, "%s", buf);
		}
	}
}

/**
 * @brief 以十六进制形式打印内存区域内容
 *
 * @param log_level 日志级别
 * @param data 数据指针
 * @param len 数据长度
 */
void logPrintfHex(LOG_LEVEL_T log_level, const unsigned char *data, int len)
{
	int i,j;
	#define WIDTH_SIZE 16
	
	if (log_level > LOG_DUMMY && log_level <= logLevel)
	{
		if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) printf("\n");
		if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) syslog(LOG_INFO, "\n");
		for(i=0; i<len; i+=WIDTH_SIZE)
		{
			if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) printf("%04X-%04x: ", i, i+WIDTH_SIZE);
			if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) syslog(LOG_INFO, "%04X-%04x: ", i, i+WIDTH_SIZE);
			for(j=0; j<WIDTH_SIZE; j++)
			{
				if(i+j < len)
				{
					if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) printf("%02X ", data[i+j]);
					if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) syslog(LOG_INFO, "%02X ", data[i+j]);
				}
				else
				{
					if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) printf("  ");
					if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) syslog(LOG_INFO, "  ");
				}
				if((j+1) % 8 == 0)
				{
					if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_STDOUT) printf(" ");
					if (logOutput == LOG_OUTPUT_ALL || logOutput == LOG_OUTPUT_SYSLOG) syslog(LOG_INFO, " ");
				}
			}
		}
	}
}

/**
 * @brief 打印最近C库调用的错误描述
 *
 * @param log_level 日志级别
 * @param str 错误信息前打印的信息
 */
void logPrintfError(LOG_LEVEL_T log_level, const char *str)
{
	if (log_level > LOG_DUMMY && log_level <= logLevel)
	{
		perror(str);
	}
}

