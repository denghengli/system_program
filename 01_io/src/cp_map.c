#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("usage: %s srcfile destfile\n", argv[0]);
		exit(1);
	}
	
	//打开源文件并获取要拷贝的长度
	int sfd = open(argv[1], O_RDONLY);
	if (sfd < 0){
		perror("open error");
		exit(1);
	}
	off_t len = lseek(sfd, 0, SEEK_END);
	printf("len: %ld\n", len);
	
	//打开目标文件
	int dfd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (dfd < 0){
		perror("open error");
		exit(1);
	}
	
	//为映射的缓存预留一块文件空间
	//设置文件的偏移量为文件长度+len-1，也就是预留了文件开头的len字节
	lseek(dfd, len-1, SEEK_END); //这里与lseek(fd, len-1, SEEK_SET)等效,因为文件长度为0
	write(dfd, "0", 1);
	
	//进行存储映射
	char *saddr = mmap(0, len, PROT_READ, MAP_SHARED, sfd, 0);
	if(saddr < 0){
		perror("mmap error");
		exit(1);
	}
	
	char *daddr = mmap(0, len, PROT_WRITE, MAP_SHARED, dfd, 0);
	if(daddr < 0){
		perror("mmap error");
		exit(1);
	}
	
	//将源映射存储区 拷贝到 目标存储映射区
	memcpy(daddr, saddr, len);
	
	//解除映射
	munmap(saddr, 0);
	munmap(daddr, 0);
	
	close(sfd);
	close(dfd);
	
	return 0;
}








