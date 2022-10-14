#ifndef __MSTDIO_H__
#define __MSTDIO_H__
#include <sys/types.h>

#define MEOF -1

enum mode{READ, WRITE, APPEND};

typedef struct
{
	int _fd;		//文件描述符
	char *_buffer;	//指向文件数据缓冲区
	char *_nextc;	//指向文件数据缓冲区中的任意一个字符
	int _mode;		//文件打开的模式
	off_t _left;	//读模式时:缓存中剩余可读的大小,初始时为0
					//写模式时:缓存中剩余可写的大小,初始时为BUFFER_LEN
}MFILE;

extern MFILE *mfdopen(int fd, const char * const mode);
extern MFILE* mfopen(const char * const pathname, const char * const mode);
extern int mfclose(MFILE *fp);
extern void mfflush(MFILE *fp);

extern int mfgetc(MFILE *fp);
extern int mfputc(int character, MFILE *fp);
extern int mungetc(int character, MFILE *fp);
extern char * mfgets(char *buff, int size, MFILE *fp);
extern int mfputs(char *buff, MFILE *fp);
extern size_t mfread(void *buff, size_t size, size_t counter, MFILE *fp);
extern size_t mfwrite(void *buff, size_t size, size_t counter, MFILE *fp);

#endif
