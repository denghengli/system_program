#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char *cmd1 = "cat"; //相对路径
char *cmd2 = "/bin/cat"; //绝对路径
char *argv1 = "/etc/passwd";
char *argv2 = "/etc/proup";

int main(void)
{
    pid_t pid;

    pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    } 
    /*子进程调用exec函数执行新的程序*/
    else if (pid == 0){
        //execl的pathname必须为绝对路径，execp的pathname可以为相对路径
        //第一个参数为：pathname，执行函数的路径
        //第二个以上参数：参数列表，第一个为要执行函数，最后一个必须为NULL
        if(execl(cmd2, cmd1, argv1, argv2, NULL) < 0){
            perror("execl error");
            exit(1);
        } else {
            printf("execl %s success\n", cmd1);
        }
    }
    /*父进程,等待子进程结束释放资源*/
    else{
        wait(NULL);
        printf("parent process \n");
    }

    return 0;
}