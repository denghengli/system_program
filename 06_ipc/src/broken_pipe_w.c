#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void sig_handler(int signo)
{
    if(signo == SIGPIPE){
        printf("SIGPIPE occured\n");
    }
}

/*
    不完整管道案例：写一个读端已经关闭的管道
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
        sleep(1);//确保子进程先运行,关闭管道的读端
        close(fd[0]);
        
        //捕获SIGPIPE信号
        signal(SIGPIPE, sig_handler);

        char *s = "123";
        if(write(fd[1], s, sizeof(s)) != sizeof(s)){
            fprintf(stderr, "%s, %s\n", strerror(errno),
                    (errno == EPIPE) ? "EPIPE" : "unknow");
        }

        close(fd[1]);
        wait(0);
    }
    else if (pid == 0)
    {
        close(fd[0]);
        close(fd[1]);
    }

    return 0;
}