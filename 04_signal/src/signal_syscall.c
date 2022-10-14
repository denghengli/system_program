#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


void sig_handler(int signo)
{
    if(signo == SIGTSTP){
        printf("SIGTSTP occured\n");
    }
}

int main(void)
{
    char buffer[512];
    ssize_t size;

    if(signal(SIGTSTP, sig_handler) == SIG_ERR){
        perror("signal sigtstp error");
    }

    //慢系统调用时，如果发生了信号，内核会重启系统调用
    //现象是：键盘输入后未按回车时，发生了信号，会重新输入
    printf("begin running and waiting for signal\n");
    size = read(STDIN_FILENO, buffer, 512);
    if(size < 0){
        perror("read error");
    }
    printf("read finished\n");

    if(write(STDOUT_FILENO, buffer, size) != size){
        perror("write error");
    }
    printf("end running\n");

    return 0;
}

