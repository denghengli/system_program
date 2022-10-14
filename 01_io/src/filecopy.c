#include "io.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fdin, fdout;
	
	if (argc != 3) {
		fprintf(stderr, "usage : %s srcfile destfile\n", argv[0]);
		exit(1);
	}
	
	//以只读方式打开待读取文件
	fdin = open(argv[1], O_RDONLY);
	if (fdin < 0) {
		fprintf(stderr, "open %s error : %s\n", argv[1], strerror(errno));
		exit(1);
	}
	printf("%s size is : %d\n", argv[1], lseek(fdin, 0, SEEK_END));//通过偏移量获取文件大小
	lseek(fdin, 0, SEEK_SET);//重新将偏移量指到文件开始
	
	//打开一个待写入的文件
	fdout = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fdout < 0) {
		fprintf(stderr, "open %s error : %s\n", argv[2], strerror(errno));
		exit(1);
	}
	
	//文件复制
	mycopy(fdin, fdout);
	lseek(fdout, 0, SEEK_SET);//重新将偏移量指到文件开始
	printf("%s size is : %d\n", argv[2], lseek(fdout, 0, SEEK_END));//通过偏移量获取文件大小
	
	//释放
	close(fdin);
	close(fdout);
}



