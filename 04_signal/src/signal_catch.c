#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//定义信号处理函数
void sig_handler(int signo)
{
    printf("%d, %d occured\n", getpid(), signo);
}

/*处理函数可有三种：
（1）自己编写的处理函数来捕获
（2）SIG_IGN 表示忽略信号
（3）SIG_DFL表示采用系统默认的方式处理信号，执行默认操作
*/
int main(void)
{
    //登记某个信号的处理函数

    //SIGTSTP: ctrl+z
    // if(signal(SIGTSTP, sig_handler) == SIG_ERR){
    //     perror("signal sigtstp error");
    // }
    if(signal(SIGTSTP, SIG_IGN) == SIG_ERR){
        perror("signal sigtstp error");
    }

    //SIGINT: ctrl+c
    if(signal(SIGINT, sig_handler) == SIG_ERR){
        perror("signal sigint error");
    }

    //SIGUSR1(10)、SIGUSR2(12)进程启动时两个信号被忽略
    //这里给打开，自己捕获
    if(signal(SIGUSR1, sig_handler) == SIG_ERR){
        perror("signal siguser1 error");
    }
    if(signal(SIGUSR2, sig_handler) == SIG_ERR){
        perror("signal siguser2 error");
    }

    //SIGKILL(9)、SIGSTOP(19)永远不能被忽略
    //这里给忽略，但是会失败，系统不让忽略！！！
    // if(signal(SIGKILL, SIG_IGN) == SIG_ERR){
    //     perror("signal sigkill error");
    // }
    // if(signal(SIGSTOP, SIG_IGN) == SIG_ERR){
    //     perror("signal sigstop error");
    // }

    int i = 0;
    while(i < 10){
        printf("%d out %d\n", getpid(), i++);
        sleep(1);

        //向进程自己发送SIGKILL（自己终止自己）
        // if(i == 5){
        //     raise(SIGKILL);
        // }
    }

    //向进程自己发送SIGUSR1和SIGUSR2信号
    raise(SIGUSR1);
    kill(getpid(), SIGUSR2);

    return 0;
}