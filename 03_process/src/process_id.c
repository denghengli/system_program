#include <unistd.h>
#include <stdio.h>

int main(void)
{
    //获取当前进程的进程号
    printf("getpid: %d\n", getpid());   
    //获取当前进程的父进程号
    printf("getppid: %d\n", getppid());

    //实际用户ID
    printf("getuid: %d\n", getuid());
    //有效的用户ID
    printf("geteuid: %d\n", geteuid());

    //实际组ID
    printf("getgid: %d\n", getgid());
    //有效组ID
    printf("getegid: %d\n", getegid());

    return 0;
}