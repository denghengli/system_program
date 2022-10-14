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
	for(i=1; i<argc; i++)
	{
		/*判断文件是否存在*/
		if(access(argv[i], F_OK) < 0) {
			printf("%s is not exist\n", argv[i]);
			continue;
		}
		
		/*判断文件是否可读*/
		if(access(argv[i], R_OK) < 0) {
			printf("%d can not read %s\n", getpid(), argv[i]);
		} else {
			printf("%d can read %s\n", getpid(), argv[i]);
		}
		
		/*判断文件是否可写*/
		if(access(argv[i], W_OK) < 0) {
			printf("%d can not write %s\n", getpid(), argv[i]);
		} else {
			printf("%d can write %s\n", getpid(), argv[i]);
		}
		
		/*判断文件是否可执行*/
		if(access(argv[i], X_OK) < 0) {
			printf("%d can not excute %s\n", getpid(), argv[i]);
		} else {
			printf("%d can excute %s\n", getpid(), argv[i]);
		}		
	}
	
	return 0;
}


















