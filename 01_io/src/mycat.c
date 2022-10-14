#include "io.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

/*
 *不支持输入输出重定向
 *1、无参数,把标准输入（键盘）的内容拷贝到标准输出（屏幕）
 *2、有参数,把输入文件的内容拷贝到标准输出（屏幕）
*/
int main1(int argc, char *argv[])
{
	int fd_in = STDIN_FILENO;//0 标准输入
	int fd_out = STDOUT_FILENO;//1 标准输出
	
	//无参数,把标准输入的内容拷贝到标准输出
	if (argc == 1) {
		mycopy(fd_in, fd_out);
	} 
	else 
	{
		for (int i=1; i<argc; i++) 
		{
			fd_in = open(argv[1], O_RDONLY);
			if (fd_in < 0) {
				perror("open error");
				continue;
			}
			mycopy(fd_in, fd_out);
			close(fd_in);
		}
	}

	return 0;
}

/*
 *支持输入输出重定向: +表示输入重定向 -表示输出重定向
 *1、无参数,把标准输入（键盘）的内容拷贝到标准输出（屏幕）
 *2、有参数,把输入文件的内容拷贝到标准输出（屏幕）
*/
int main(int argc, char *argv[])
{
	int fd, i;
	int fd_in = STDIN_FILENO;//0 标准输入
	int fd_out = STDOUT_FILENO;//1 标准输出
	
	//无参数,把标准输入的内容拷贝到标准输出
	if (argc == 1) {
		mycopy(fd_in, fd_out);
	} 
	else 
	{
		for (i=1; i<argc; i++)
		{
			if (!strcmp(argv[i], "+"))//输入重定向
			{
				if ((fd = open(argv[++i], O_RDONLY)) < 0) {
					perror("open");
					exit(1);
				} else {
					if (dup2(fd, STDIN_FILENO) != STDIN_FILENO) {
						perror("dup2");
						exit(1);
					}
				}
				close(fd);
			}
			else if (!strcmp(argv[i], "-"))//输出重定向
			{
				if ((fd = open(argv[++i], O_WRONLY|O_CREAT|O_TRUNC, 0777)) < 0) {
					perror("open");
					exit(1);
				} else {
					if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO) {
						perror("dup2");
						exit(1);
					}
				}
				close(fd);
			}
			else
			{
				fprintf(stderr, "usage: %s [+] file1 [-] file2\n", argv[0]);
				exit(1);
			}
		}
		
		mycopy(STDIN_FILENO, STDOUT_FILENO);
	}
	
	return 0;
}




