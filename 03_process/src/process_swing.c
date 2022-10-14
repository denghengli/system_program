#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int counter = 0;

    if(argc < 2){
        counter = 2;
    } else {
        counter = atoi(argv[1]);
    }

    /*进程扇*/
    int i = 1;
    pid_t pid;
    for(; i<counter; i++){
        pid = fork();
        if(pid < 0){
            perror("fork error");
            exit(1);
        } else if (pid > 0){
            
        } else if (pid == 0){
            break;//子进程退出循环，父进程继续循环
        }
    }

    printf("pid: %d, ppid: %d\n", getpid(), getppid());

    while (1)
    {
        sleep(1);
    }
    
    return 0;
}