#include "io.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if (argc < 4) {
		printf("usage: %s content file lock|unlock\n",argv[0]);
		exit(1);
	}
	
	printf("\n");//美观
	
	ssize_t size = strlen(argv[1]) * sizeof(char);
	int fd = open(argv[2], O_WRONLY | O_CREAT, 0777);
	if (fd < 0) {
		perror("open error");
		exit(1);
	}
	
	//方便实验多个进程写同一个文件
	sleep(5);
	
	//加文件锁(阻塞式的独占写锁)
	if (!strcmp("lock", argv[3])) {
		//第二个进程想要对文件加锁必须要等到
		//第一个进程释放文件锁后才能加锁
		WRITE_LOCKW(fd, 0, SEEK_SET, 0);
		printf("clock pid : %d\n", getpid());
		printf("lock success\n");
	}

	char *p = argv[1];
	int i;
	for (i=0; i<size; i++) {
		if (write(fd, (p+i), 1) != 1) {
			perror("write error");
			exit(1);
		}
		printf("%d success write one character\n", getpid());
		sleep(1);
	}
	
	//加锁后 需要解锁
	if (!strcmp("lock", argv[3])) {
		UNLOCK(fd, 0, SEEK_SET, 0);
		printf("unlock pid : %d\n", getpid());
		printf("unlock success\n\n");
	}
	
	close(fd);
	
	return 0;
}




















