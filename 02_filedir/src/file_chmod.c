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

#define MODE S_IRWXU | S_IRWXG | S_IRWXO
#define UMASK S_IXUSR | S_IWGRP | S_IXGRP | S_IRWXO

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "usage: %s files\n", argv[0]);
		exit(1);
	}
	
	//设置掩码
	umask(UMASK);
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, MODE);
	if(fd < 0){
		perror("open error");
		exit(1);
	}
	
	close(fd);
	
	return 0;
}


















