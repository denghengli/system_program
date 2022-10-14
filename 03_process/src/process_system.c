#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char *cmd1 = "date"; //相对路径

//自己模拟实现system函数
void mysystem(char *cmd)
{
    pid_t pid;
    if((pid=fork()) < 0){
        perror("fork error");
        exit(1);
    }else if(pid == 0){
        if(execlp("/bin/bash", "/bin/bash", "-c", cmd, NULL) < 0){
            perror("execlp error");
            exit(1);
        }
    }else{
        wait(NULL);
    }
}

int main(void)
{
    system("clear");
    system(cmd1);

    mysystem(cmd1);

    return 0;
}