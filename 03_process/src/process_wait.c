#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void out_status(int status)
{
    if(WIFEXITED(status)){
        printf("normal exit: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)){
        printf("abnormal term: %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)){
        printf("stopped sig: %d\n", WSTOPSIG(status));
    } else {
        printf("unknow sig\n");
    }
}

int main(int argc, char *argv[])
{
    int status;
    pid_t pid;

    if((pid=fork()) < 0){
        perror("fork error");
        exit(1);
    } else if (pid == 0){
        printf("pid: %d, ppid: %d\n", getpid(), getppid());
        pause();
        //exit(3);//子进程终止运行（异常终止）
    }

    //父进程阻塞，等待子进程结束并回收
    //wait(&status);

    //父进程非阻塞，等待子进程结束并回收
    do{
        //父进程中返回的pid是子进程的
        pid = waitpid(pid, &status, WNOHANG|WUNTRACED);
        if(pid == 0) 
            sleep(1);
    }while(pid == 0);

    out_status(status);
    printf("--------------------\n");

    return 0;
}
