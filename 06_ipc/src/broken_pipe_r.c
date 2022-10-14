#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    不完整管道案例：读取一个写端已经关闭的管道
*/
int main(void)
{
    int fd[2];

    //创建管道
    if (pipe(fd) < 0){
        perror("pipe error");
        exit(1);
    }

    //创建进程
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0)
    {
        sleep(1);//确保子进程先运行,写入数据后关闭管道的写端
        close(fd[1]);
        while(1)
        {
            char c;
            if(read(fd[0], &c, 1) == 0){
                printf("\nwrite-end of pipe closed\n");
                break;
            }else{
                printf("%c", c);
            }
        }
        close(fd[0]);
        wait(0);
    }
    else if (pid == 0)
    {
        close(fd[0]);
        char *s = "123";
        write(fd[1], s, strlen(s));
        //写入数据后关闭管道的写端
        close(fd[1]);
    }

    return 0;
}