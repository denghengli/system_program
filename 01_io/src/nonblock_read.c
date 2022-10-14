#include "io.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char buffer[4096] = {0};
	ssize_t size = 0;
	
	//设置为非阻塞I/O
	set_fl(STDIN_FILENO, O_NONBLOCK);
	
	sleep(5);
	
	size = read(STDIN_FILENO, buffer, sizeof(buffer));
	if (size < 0) {
		perror("read error");
		exit(1);
	} else if (size == 0) {
		printf("read finished!\n");
	} else {
		if (write(STDOUT_FILENO, buffer, size) != size) {
			perror("write error");
			exit(1);
		}
	}
	
	return 0;
}





