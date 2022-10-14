#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "usage: %s dir\n", argv[0]);
		exit(1);
	}
	
	struct stat buff;
	if(lstat(argv[1], &buff) < 0){
		perror("lstat error");
		exit(1);
	}
	//判断是否是目录
	if(!S_ISDIR(buff.st_mode)){
		fprintf(stderr, "%s is not directory\n", argv[1]);
		exit(1);
	}
	
	//打开目录
	DIR *dir = opendir(argv[1]);
	
	//读取目录内的信息
	struct dirent *ent;
	while((ent = readdir(dir)) != NULL){
		printf("%-20s %10ld\n", ent->d_name, ent->d_ino);
	}
	closedir(dir);
	
	return 0;
}