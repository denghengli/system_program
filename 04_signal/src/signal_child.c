#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sig_handler(int signo)
{
    printf("child process deafed, signo: %d\n", signo);
    //当父进程捕获到SIGCHLD信号后要调用wait函数去回收子进程
    wait(0);
}

void out(int n)
{
    int i = 0;
    for(i=0; i<n; i++){
        printf("%d out %d\n", getpid(), i);
        sleep(1);
    }
}

int main(void)
{
    if(signal(SIGCHLD, sig_handler) == SIG_ERR){
        perror("signal SIGCHLD error");
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    }
    //子进程
    else if(pid == 0){
        out(10);
    }
    //父进程
    else {
        out(100);
    }

    return 0;
}