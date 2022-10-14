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
		
		//获得文件的权限信息
		mode_t mode = buff.st_mode;
		
		//uesr permission
		if(S_IRUSR & mode) printf("r");
		else printf("-");
		if(S_IWUSR & mode) printf("w");
		else printf("-");
		if(S_IXUSR & mode) printf("x");
		else printf("-");
		
		//group permission
		if(S_IRGRP & mode) printf("r");
		else printf("-");
		if(S_IWGRP & mode) printf("w");
		else printf("-");
		if(S_IXGRP & mode) printf("x");
		else printf("-");
		
		//other permission
		if(S_IROTH & mode) printf("r");
		else printf("-");
		if(S_IWOTH & mode) printf("w");
		else printf("-");
		if(S_IXOTH & mode) printf("x");
		else printf("-");
		
		printf("\n");
	}
	
	return 0;
}


















