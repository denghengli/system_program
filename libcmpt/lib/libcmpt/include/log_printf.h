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
#ifndef __LOG_PRINTF_H__
#define __LOG_PRINTF_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

//日志级别
typedef enum
{
	LOG_DUMMY = -1,//空日志级别，不输出日志消息(一般不用)
	LOG_APP_ERROR,    //应用程序错误日志
	LOG_APP_NOTICE,   //应用程序通知日志
	LOG_APP_TRACE,    //应用程序调试日志
	LOG_LEVEL_MAX     
}LOG_LEVEL_T;

//日志输出方式
typedef enum
{
	/*日志输出方式*/
	LOG_OUTPUT_ALL = 0xFF,//日志输出到所有方式(默认设置)
	LOG_OUTPUT_NONE = 0x00,//日志不输出
	LOG_OUTPUT_STDOUT = 0x01,//日志输出到标准输出
	LOG_OUTPUT_SYSLOG = 0x02,//日志输出到系统日志
}LOG_OUTPUT_T;

/**
 * @brief 设置日志输出方式
 *
 * @param log_output
 */
extern void logPrintfSetLogOutput(LOG_OUTPUT_T log_output);

/**
 * @brief 设置日志的打印级别
 *
 * @param log_level
 */
extern void logPrintfSetLogLevel(LOG_LEVEL_T log_level);

/**
 * @brief 设置日志的打印级别
 *
 * @param log_level_str "dummy/release/debug/develop/LOG_APP_ERROR/.../LOG_APP_TRACE"
 */
extern void logPrintfSetLogLevelByStr(const char *log_level_str);

/**
 * @brief 获取当前日志的打印级别
 *
 * @return LOG_LEVEL_T
 */
extern LOG_LEVEL_T logPrintfGetLogLevel(void);

/**
 * @brief 打印日志
 *
 * @param log_level 日志级别
 */
extern void logPrintf(LOG_LEVEL_T log_level, const char *format, ...);

/**
 * @brief 打印日志,不带前面提示的版本
 *
 * @param log_level 日志级别
 */
extern void logPrintfNoPrompt(LOG_LEVEL_T log_level, const char *format, ...);

/**
 * @brief 以十六进制形式打印内存区域内容
 *
 * @param log_level 日志级别
 * @param data 数据指针
 * @param len 数据长度
 */
extern void logPrintfHex(LOG_LEVEL_T log_level, const unsigned char *data, int len);

/**
 * @brief 打印最近C库调用的错误描述
 *
 * @param log_level 日志级别
 * @param str 错误信息前打印的信息
 */
extern void logPrintfError(LOG_LEVEL_T log_level, const char *str);

/* Simple for application log */
#define log_error(format, ...) logPrintf(LOG_APP_ERROR, format, ##__VA_ARGS__)
#define log_notice(format, ...) logPrintf(LOG_APP_NOTICE, format, ##__VA_ARGS__)
#define log_trace(format, ...) logPrintf(LOG_APP_TRACE, format, ##__VA_ARGS__)
/* Simple for application hex log */
#define logh_error(data, len) logPrintfHex(LOG_APP_ERROR, data, len)
#define logh_notice(data, len) logPrintfHex(LOG_APP_NOTICE, data, len)
#define logh_trace(data, len) logPrintfHex(LOG_APP_TRACE, data, len)
/* Simple for libc error log */
#define logc_error(str) logPrintfError(LOG_APP_ERROR, str)
#define logc_notice(str) logPrintfError(LOG_APP_NOTICE, str)
#define logc_trace(str) logPrintfError(LOG_APP_TRACE, str)

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif
