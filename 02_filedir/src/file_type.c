#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "usage: %s files\n", argv[0]);
		exit(1);
	}
	
	int i = 1;
	struct stat buff;
	for(i=1; i<argc; i++)
	{
		memset(&buff, 0, sizeof(buff));
		if(lstat(argv[i], &buff) < 0) {
			perror("lstat error");
			continue;
		}
		printf("%-20s", argv[i]);
		//判断文件的类型
		if(S_ISREG(buff.st_mode)){
			printf("normal file\n");
		} else if (S_ISDIR(buff.st_mode)){
			printf("directory\n");
		} else if (S_ISBLK(buff.st_mode)){
			printf("block device\n");
		} else if (S_ISCHR(buff.st_mode)){
			printf("character device\n");
		} else if (S_ISSOCK(buff.st_mode)){
			printf("sock device\n");
		} else if (S_ISFIFO(buff.st_mode)){
			printf("named pipe\n");
		} else if (S_ISLNK(buff.st_mode)){
			printf("link file\n");
		} else {
			printf("unknow file\n");
		}
	}
	
	return 0;
}


















