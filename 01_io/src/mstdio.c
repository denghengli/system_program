#include "mstdio.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_LEN 4096

MFILE *mfdopen(int fd, const char * const mode)
{
	MFILE *fp = (MFILE*)malloc(sizeof(MFILE));
	assert(fp != NULL);
	
	fp->_buffer = (char*)malloc(BUFFER_LEN);
	assert(fp->_buffer != NULL);
	fp->_fd = fd;
	fp->_nextc = fp->_buffer;
	
	if(!strcmp(mode, "r"))
	{
		fp->_mode = READ;
		fp->_left = 0;
	} 
	else if(!strcmp(mode, "w"))
	{
		fp->_mode = WRITE;
		fp->_left = BUFFER_LEN;
	}
	else if(!strcmp(mode, "a"))
	{
		fp->_mode = APPEND;
		fp->_left = BUFFER_LEN;
	}
	
	return fp;
}

MFILE* mfopen(const char * const pathname, const char * const mode)
{
	int fd;
	
	if (!strcmp(mode, "r")) //读模式
	{
		fd = open(pathname, O_RDONLY);
	} 
	else if (!strcmp(mode, "w")) //写模式
	{
		fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	} 
	else if (!strcmp(mode, "a")) //追加模式
	{
		fd = open(pathname, O_WRONLY | O_CREAT | O_APPEND, 0777);
	} 
	else 
	{
		return NULL;
	}
	
	if (fd < 0) return NULL;
	
	return mfdopen(fd, mode);
}

int mfclose(MFILE *fp)
{
	mfflush(fp);
	int res = close(fp->_fd);
	free(fp->_buffer);
	free(fp);
	return res;
}

/*刷新缓存*/
void mfflush(MFILE *fp)
{
	if (fp->_mode == READ)
	{
		fp->_nextc = fp->_buffer;
		fp->_left = 0;
	}
	else //WRITR or APPEND
	{
		write(fp->_fd, fp->_buffer, BUFFER_LEN - fp->_left);
		fp->_nextc = fp->_buffer;
		fp->_left = BUFFER_LEN;
	}
}

/*从文件中读取一个字节*/
int mfgetc(MFILE *fp)
{
	char c;
	
	assert(fp->_mode == READ);
	
	/*当缓存中的数据已读取完毕,再从文件中读取一批新的数据放入到缓存中*/
	if(fp->_left == 0)
	{
		ssize_t size = read(fp->_fd, fp->_buffer, BUFFER_LEN);
		assert(size >= 0);
		if(size == 0) return MEOF;
		fp->_nextc = fp->_buffer;
		fp->_left = size;
	}
	
	c = *(fp->_nextc);
	fp->_nextc++;
	fp->_left--;
	
	return c;
}

int mfputc(int character, MFILE *fp)
{
	assert(fp->_mode == WRITE || fp->_mode == APPEND);
	
	/*若缓存满,将缓存中的数据写入到文件中*/
	if(fp->_left == 0)
	{
		if(write(fp->_fd, fp->_buffer, BUFFER_LEN) != BUFFER_LEN)
			return 0;
		
		fp->_nextc = fp->_buffer;
		fp->_left = BUFFER_LEN;
	}
	
	*(fp->_nextc) = (char)character;
	fp->_nextc++;
	fp->_left--;
	
	return 1;
}

int mungetc(int character, MFILE *fp)
{
	
}

char * mfgets(char *buff, int size, MFILE *fp)
{
	char c;
	char *ptr = buff;
	
	assert(fp->_mode == READ);
	
	for(int i=0; i<size; buff++,i++)
	{
		c = mfgetc(fp);
		if(c == MEOF) return NULL;
		*buff = c;
	}
	
	return ptr;
}

int mfputs(char *buff, MFILE *fp)
{
	assert(fp->_mode == WRITE || fp->_mode == APPEND);
	
	for(; *buff!='\0'; buff++)
	{
		mfputc(*buff, fp);
		
		if(*buff == '\n')
		{
			mfflush(fp);
		}
	}
	
	return 1;
}

size_t mfread(void *buff, size_t size, size_t counter, MFILE *fp)
{
	char c;
	int i,j;
	char *ptr = (char*)buff;
	assert(fp->_mode == READ);
	
	for(i=0; i<counter; i++)
	{
		for (j=0; j<size; j++,ptr++)
		{
			c = mfgetc(fp);
			if(c == MEOF) return counter;
			*ptr = c;
		}
	}
	
	return counter;
}

size_t mfwrite(void *buff, size_t size, size_t counter, MFILE *fp)
{
	int i,j;
	char *ptr = (char*)buff;
	assert(fp->_mode == WRITE || fp->_mode == APPEND);
	
	for(i=0; i<counter; i++)
	{
		for (j=0; j<size; j++,ptr++)
		{
			mfputc(*ptr, fp);
			
			if(*ptr == '\n')
			{
				mfflush(fp);
			}
		}
	}
	
	return counter;
}















