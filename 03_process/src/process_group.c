#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    //创建进程组，组长进程为父进程
    setpgid(getpid(), getpid());

    pid_t group1 = getpgid(getpid());
    pid_t group2;

    int i = 0;
    for(; i<3; i++){
        pid = fork();
        if(pid < 0){
            perror("fork error");
            exit(1);
        }
        //父进程
        else if (pid > 0){
            //将第一个子进程加入到group1
            if(i == 0) setpgid(pid, group1);
            //创建进程组，第二个子进程作为组长进程
            if(i == 1){
                setpgid(pid, pid);
                group2 = getpgid(pid);
            }
            //将第三个子进程加入到group2
            if(i == 2){
                setpgid(pid, group2);
            }
        }
        //子进程
        else{
            //将第一个子进程加入到group1
            if(i == 0) setpgid(getpid(), group1);
            //创建进程组，第二个子进程作为组长进程
            if(i == 1){
                setpgid(getpid(), getpid());
                group2 = getpgid(getpid());
            }
            //将第三个子进程加入到group2
            if(i == 2){
                setpgid(getpid(), group2);
            }
            //因为是进程扇，每个子进程要退出循环
            //父进程继续循环fork
            break;
        }
    }
}


