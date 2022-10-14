 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <unistd.h>
#include "shm_pipetell.h"

int fd[2];

//管道初始化
void pipe_init()
{
    if(pipe(fd) < 0){
        perror("pipe error");
    }
}

//利用管道进行等待
void wait_pipe()
{
    char c;
    //管道读写默认是阻塞性的
    if(read(fd[0], &c, 1) < 0){
        perror("wait pipe error");
    }
}

//利用管道进行通知
void notify_pipe()
{
    char c = 'a';
    if(write(fd[1], &c, 1) != 1){
        perror("notify pipe error");
    }
}

//销毁管道
void destroy_pipe()
{
    close(fd[0]);
    close(fd[1]);
}
