/*
 * =====================================================================================
 *
 *       Filename:  OBuffer.h
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
#ifndef _O_BUFFER_
#define _O_BUFFER_

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

#define OBUFFER_MALLOC(size)   malloc(size);
#define OBUFFER_CALLOC(n,size) calloc(n,size);
#define OBUFFER_FREE(p)        free(p);

//字节串描述
typedef struct _bytes
{
	char *bArray;//字节串缓冲区
	int len;//字节串长度
}BYTES;

//匹配规则类型
enum RULE_TYPE_T{
	R_NO_RULE = 0,	    //没有规则,取走buffer中已存在的所有数据
	R_HEAD_TAIL = 1,  	//满足指定包头和包尾的数据帧
	R_HEAD_LEN = 2,		//满足指定包头和长度的数据帧
	R_LEN_ONLY = 3,		//满足指定长度的数据帧
	R_TAIL_ONLY = 4,	//满足指定包尾的数据帧
	R_CUSTOM_RULE = 5   //定制规则，由上层定义匹配函数matchFunc
};

//匹配规则
typedef struct
{
	enum RULE_TYPE_T type;
	BYTES head;
	BYTES tail;
	int len;
	/**
	 * @brief 自定义匹配函数
	 *
	 * @param pBufData 缓冲区中的数据指针
	 * @param dataLen 缓冲区中的数据长度
	 * @param validDataOffset 出参：有效数据包的位置偏移
	 * @param validDataLen 出参：有效数据包的长度
	 *
	 * @return 0:匹配到有效的数据包；-1:未匹配到有效的数据包
	 */
	int (*matchFunc)(const void *pBufData, int dataLen, int *validDataOffset, int *validDataLen);
}MATCH_RULE_T;

//前向声明
typedef struct _o_buffer OBuffer;
struct PRIVATE_DATA;

/**
 * @brief 初始化一个缓冲区对象（内部会动态分配内存空间，使用完后必须使用destroyOBufferObj接口进行释放）
 *
 * @param bufferSize 缓冲区大小（如果<=0，则默认为1024）
 *
 * @return 缓冲区对象指针；NULL：初始化失败
 */
OBuffer *initOBufferObj(int bufferSize);

/**
 * @brief 清除buffer对象,释放空间
 *
 * @param pBuf
 */
void destroyOBufferObj(OBuffer *pBuf);

//OBuffer结构体定义
struct _o_buffer
{
	struct PRIVATE_DATA *privateData;//缓冲区私有数据，外部无法操作
	
	/**
	 * @brief 向pBuf缓冲区中追加数据
	 *
	 * @param pBuf buffer指针
	 * @param data 追加的数据
	 */
	void (* bAppend)(OBuffer *pBuf, const BYTES *data);
	
	/**
	 * @brief 删除pBuf缓冲区中前len个长度的数据
	 *
	 * @param pBuf buffer指针
	 * @param len 删除数据的长度
	 */
	void (* bRemove)(OBuffer *pBuf, int len);

    /**
	 * @brief 清空pBuf缓冲区
	 *
	 * @param pBuf buffer指针
	 */
	void (* bClear)(OBuffer *pBuf);
	
	/**
	 * @brief 获取当前pBuf缓冲区中数据的长度
	 *
	 * @param pBuf buffer指针
	 *
	 * @return 数据长度
	 */
	int (* bGetDataLen)(OBuffer *pBuf);
	
	/**
	 * @brief 获取当前pBuf缓冲区中数据指针
	 *
	 * @param pBuf buffer指针
	 *
	 * @return 数据指针
	 */
	const char *(* bGetDataPtr)(OBuffer *pBuf);
	
	/**
	 * @brief 按照匹配规则查找指定字符串,并读取到out中,然后删除buf中已读的数据
	 * 		注意：(1)内部会给out分配空间,外部使用完后必须主动释放
	 *            (2)未读的数据还会继承存留在缓存中,如果没有用了需要手动bClear清空掉
	 *
	 * @param pBuf buffe指针
	 * @param rule 匹配规则
	 * @param out 目标字符串的存放指针
	 *
	 * @return -1: 没有符合规则的字符串；0：成功提取
	 */
	int (* bGetDataByRule)(OBuffer *pBuf, MATCH_RULE_T *rule, BYTES *out);
};

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif

