#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("usage: %s file\n", argv[0]);
		exit(1);
	}
	
	int fd = open(argv[1], O_RDWR);
	if (fd < 0){
		perror("open error");
		exit(1);
	}
	
	//文件的偏移量为文件长度+19，也就是预留了文件开头的20字节
	lseek(fd, 19, SEEK_END);
	write(fd, "0", 1);
	
	//进行存储映射
	char *addr = NULL;
	addr = mmap(0, 20, PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr < 0){
		perror("mmap error");
		exit(1);
	}
	
	//修改存储区会同步到文件
	int i;
	for(i=0; i<20; i++){
		*(addr+i) = 'A' + i;
	}
	printf("write success\n");
	
	//解除映射
	munmap(addr, 0);
	close(fd);
	
	return 0;
}








