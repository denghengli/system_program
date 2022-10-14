#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 *父进程通过管道传输两个数据给子进程
 *由子进程负责从管道中读取并输出
*/
int main(void)
{
    int fd[2];

    //创建管道
    if(pipe(fd) < 0){
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    if((pid = fork()) < 0){
        perror("fork error");
        exit(1);
    }
    else if(pid > 0){ //父进程
        close(fd[0]);//写管道，所以要关掉读端fd[0]
        //往管道写入数据
        int start = 10, end = 100;
        write(fd[1], &start, sizeof(int));
        write(fd[1], &end, sizeof(int));
        close(fd[1]);

        wait(NULL);//回收子进程资源
    }
    else{ //子进程,子进程会复制一份父进程的管道fd
        close(fd[1]);//读管道，所以要关掉写端fd[1]
        //读取管道数据
        int start = 10, end = 100;
        read(fd[0], &start, sizeof(int));
        read(fd[0], &end, sizeof(int));
        close(fd[0]);

        printf("start=%d, end=%d\n", start, end);
    }

    return 0;
}