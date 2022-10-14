/*
 * =====================================================================================
 *
 *       Filename:  OBuffer.c
 *
 *    Description:  用户空间输入缓冲区实现
 *
 *        Version:  1.0
 *        Created:  2022年05月10日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  denghegnli
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>
	
#include "log_printf.h"
#include "OBuffer.h"

//私有数据
struct PRIVATE_DATA
{
    char *buf;  //数据缓存区
	int size;   //数据缓冲区大小
	int len;    //缓冲区中数据长度
};


/**
 * @brief 向pBuf缓冲区中追加数据
 *
 * @param pBuf buffer指针
 * @param data 追加的数据
 */
static void bAppend(OBuffer *pBuf, const BYTES *data)
{
    int wlen,remain_len;
	int need_space;
    char *ptr;

    //缓存区总长度小于追加的数据长度：只追加数据包的后缓存区大小个数据，
    //也就是数据包前（数据包长度 - 缓存区长度）的数据被踢除
    if(data->len > pBuf->privateData->size)
    {
        wlen = pBuf->privateData->size;
        ptr = data->bArray + (data->len - wlen);
        memcpy(pBuf->privateData->buf, ptr, wlen); 
        pBuf->privateData->len = wlen;
    }
    else
    {
_copy:
        remain_len = pBuf->privateData->size - pBuf->privateData->len;
        //当前剩余缓存区的空间，不足够存储当前追加的数据，会将缓存区中较早的数据踢除
        if(remain_len < data->len)
        {
            need_space = data->len - remain_len;
			//数据往前移
			memcpy(pBuf->privateData->buf, pBuf->privateData->buf + need_space, pBuf->privateData->len - need_space);
			pBuf->privateData->len -= need_space;
			goto _copy;
        }
		else
		{
			ptr = pBuf->privateData->buf + pBuf->privateData->len;
			memcpy(ptr, data->bArray, data->len);
			pBuf->privateData->len += data->len;
		}
    }
}

/**
 * @brief 删除pBuf缓冲区中前len个长度的数据
 *
 * @param pBuf buffer指针
 * @param len 删除数据的长度
 */
static void bRemove(OBuffer *pBuf, int len)
{
	char *ptr;
	
	if(len > pBuf->privateData->len)
	{
		memset(pBuf->privateData->buf, 0, pBuf->privateData->len);
		pBuf->privateData->len = 0;
	}
	else
	{
		ptr = pBuf->privateData->buf + len;
		memcpy(pBuf->privateData->buf, ptr, pBuf->privateData->len - len);
		pBuf->privateData->len -= len;
	}
}

/**
 * @brief 清空pBuf缓冲区
 *
 * @param pBuf buffer指针
 */
static void bClear(OBuffer *pBuf)
{
    memset(pBuf->privateData->buf, 0, pBuf->privateData->size);
    pBuf->privateData->len = 0;
}

/**
 * @brief 获取当前pBuf缓冲区中数据的长度
 *
 * @param pBuf buffer指针
 *
 * @return 数据长度
 */
static int bGetDataLen(OBuffer *pBuf)
{
    return pBuf->privateData->len;
}

/**
 * @brief 获取当前pBuf缓冲区中数据指针
 *
 * @param pBuf buffer指针
 *
 * @return 数据指针
 */
static const char *bGetDataPtr(OBuffer *pBuf)
{
    return pBuf->privateData->buf;
}

/**
 * @brief 按照匹配规则查找指定字符串,并读取到out中，然后删除buf中已读的数据
 *      注意：内部会给out分配空间，外部使用完后必须主动释放
 *
 * @param pBuf buffe指针
 * @param rule 匹配规则
 * @param out 目标字符串的存放指针
 *
 * @return -1: 没有符合规则的字符串；0：成功提取
 */
static int bGetDataByRule(OBuffer *pBuf, MATCH_RULE_T *rule, BYTES *out)
{
    int ret = -1;
    char *ptr = NULL, *head_ptr = NULL, *tail_ptr = NULL;
    int validDataOffset;
    int validDataLen;
    int cpy_len = 0, remain_len = 0;
    
	//没有规则：取走buffer中已存在的所有数据
    if(rule->type == R_NO_RULE)
    {
        out->bArray = OBUFFER_CALLOC(1, pBuf->privateData->len);
        memcpy(out->bArray, pBuf->privateData->buf, pBuf->privateData->len);
        out->len = pBuf->privateData->len;
		pBuf->privateData->len = 0;
		memset(pBuf->privateData->buf, 0, pBuf->privateData->size);
        ret = 0;
    }
    //满足指定包头和包尾的数据帧
    else if(rule->type == R_HEAD_TAIL)
    {
        head_ptr = strstr(pBuf->privateData->buf, rule->head.bArray);
        if(head_ptr)
        {
            tail_ptr = strstr(head_ptr + rule->head.len, rule->tail.bArray);
            if(tail_ptr)
            {
                cpy_len = tail_ptr - head_ptr + rule->tail.len;
                out->bArray = OBUFFER_CALLOC(1, cpy_len);
                memcpy(out->bArray, head_ptr, cpy_len);
                out->len = cpy_len;
				ret = 0;
				//缓存区剩余数据前移
				pBuf->privateData->len -= (tail_ptr - pBuf->privateData->buf + rule->tail.len);
				memcpy(pBuf->privateData->buf, tail_ptr + rule->tail.len, pBuf->privateData->len);
            }
        }
    }
    //满足指定包头和长度的数据帧
    else if(rule->type == R_HEAD_LEN)
    {
        ptr = strstr(pBuf->privateData->buf, rule->head.bArray);
        if(ptr)
        {
            remain_len = pBuf->privateData->len - (ptr - pBuf->privateData->buf);
            if(remain_len >= rule->len)
            {
                out->bArray = OBUFFER_CALLOC(1, rule->len);
                memcpy(out->bArray, ptr, rule->len);
                out->len = rule->len;
				ret = 0;
				//缓存区剩余数据前移
				pBuf->privateData->len -= (ptr - pBuf->privateData->buf + rule->len);
				memcpy(pBuf->privateData->buf, ptr + rule->len, pBuf->privateData->len);
            }
        }
    }
    //满足指定包尾的数据帧
    else if(rule->type == R_TAIL_ONLY)
    {
        ptr = strstr(pBuf->privateData->buf, rule->tail.bArray);
        if(ptr)
        {
            cpy_len = ptr - pBuf->privateData->buf + rule->tail.len;
            out->bArray = OBUFFER_CALLOC(1, cpy_len);
            memcpy(out->bArray, pBuf->privateData->buf, cpy_len);
			ret = 0;
			//缓存区剩余数据前移
			pBuf->privateData->len -= (ptr - pBuf->privateData->buf + rule->tail.len);
			memcpy(pBuf->privateData->buf, ptr + rule->tail.len, pBuf->privateData->len);
        }
    }
    //满足指定长度的数据帧
    else if(rule->type == R_LEN_ONLY)
    {
        if(pBuf->privateData->len >= rule->len)
        {
            out->bArray = OBUFFER_CALLOC(1, rule->len);
            memcpy(out->bArray, pBuf->privateData->buf, rule->len);
            out->len = rule->len;
            ret = 0;
			//缓存区剩余数据前移
			pBuf->privateData->len -= rule->len;
			memcpy(pBuf->privateData->buf, pBuf->privateData->buf + rule->len, pBuf->privateData->len);
        }
    }
    //定制规则，由上层定义匹配函数matchFunc
    else if(rule->type == R_CUSTOM_RULE)
    {
        ret = rule->matchFunc(pBuf->privateData->buf, pBuf->privateData->len, &validDataOffset, &validDataLen);
        if(ret == 0)
        {
            out->bArray = OBUFFER_CALLOC(1, validDataLen);
            memcpy(out->bArray, pBuf->privateData->buf + validDataOffset, validDataLen);
            out->len = validDataLen;
			ret = 0;
			//缓存区剩余数据前移
			pBuf->privateData->len -= (validDataOffset + validDataLen);
			memcpy(pBuf->privateData->buf, pBuf->privateData->buf + validDataOffset + validDataLen, pBuf->privateData->len);
        }
    }

    return ret;
}


/**
 * @brief 初始化一个缓冲区对象（内部会动态分配内存空间，使用完后必须使用destroyOBufferObj接口进行释放）
 *
 * @param bufferSize 缓冲区大小（如果<=0，则默认为1024）
 *
 * @return 缓冲区对象指针；NULL：初始化失败
 */
OBuffer *initOBufferObj(int bufferSize)
{
    struct _o_buffer *buffer = NULL;

    buffer = OBUFFER_CALLOC(1, sizeof(struct _o_buffer));
    if(!buffer)
    {
		logPrintf(LOG_APP_ERROR, "Init OBuffer object fail!\n");
		return NULL;
	}

	buffer->privateData = OBUFFER_CALLOC(1, sizeof(struct PRIVATE_DATA));
	if(!buffer->privateData)
	{
		logPrintf(LOG_APP_ERROR, "Init OBuffer object fail!\n");
		return NULL;
	}

    buffer->privateData->size = bufferSize < 0 ? 1024 : bufferSize;
	buffer->privateData->buf = OBUFFER_CALLOC(1, sizeof(char) * buffer->privateData->size);
	if(!buffer->privateData->buf)
	{
		logPrintf(LOG_APP_ERROR, "Init OBuffer object fail!\n");
		return NULL;
	}

    buffer->bAppend = bAppend;
    buffer->bRemove = bRemove;
    buffer->bClear = bClear;
    buffer->bGetDataLen = bGetDataLen;
    buffer->bGetDataPtr = bGetDataPtr;
    buffer->bGetDataByRule = bGetDataByRule;
    
	return buffer;
}

/**
 * @brief 清除buffer对象,释放空间
 *
 * @param pBuf
 */
void destroyOBufferObj(OBuffer *pBuf)
{
    OBUFFER_FREE(pBuf->privateData->buf);
    pBuf->privateData->buf = NULL;
    
    OBUFFER_FREE(pBuf->privateData);
    pBuf->privateData = NULL;

    OBUFFER_FREE(pBuf);
    pBuf = NULL;
}

