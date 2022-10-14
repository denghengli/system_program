#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    pid_t pid;

    pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    }
    //子进程结束成为僵尸进程
    else if(pid == 0){
        printf("pid: %d, ppid: %d\n", getpid(), getppid());
        exit(0);
    }

    //父进程继续做循环
    while (1)
    {
        sleep(1);
    }
    
    return 0;
}