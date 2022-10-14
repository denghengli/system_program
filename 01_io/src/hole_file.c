#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


/*利用lseek生成空洞文件(从文件尾部跳过若干字节再写入)*/
char *buffer = "0123456789";

int main(int argc, char *argv[])
{
	int fd;
	size_t size = strlen(buffer) * sizeof(char);
	
	if (argc != 2) {
		fprintf(stderr, "usage: %s [file]\n", argv[0]);
		exit(1);
	}
	
	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd < 0) {
		fprintf(stderr, "open error : %s\n", strerror(errno));//等同于 perror("open error");
		perror("open error");
		exit(1);
	}
	
	//将字符串写入到空洞文件中
	if (write(fd, buffer, size) != size) {
		perror("write error");
		exit(1);
	}
	//定位到文件尾部 10个字节处
	if (lseek(fd, 10, SEEK_END) < 0) {
		perror("lseek error");
		exit(1);
	}
	//继续写入
	if (write(fd, buffer, size) != size) {
		perror("write error");
		exit(1);
	}
	
	close(fd);
	
	return 0;
}











