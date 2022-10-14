#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void sig_handler(int signo)
{
    if(signo == SIGALRM){
        printf("clock timrout\n");
        alarm(5);//重新设置定时器
    }
}

void out_data(void)
{
    int i = 1;
    while(i <= 20){
        double d = drand48();
        printf("%-10d:%lf\n", i++, d);

        if(i == 16) alarm(0); //取消定时器

        sleep(1);
    }
}

int main(void)
{
    if(signal(SIGALRM, sig_handler) == SIG_ERR){
        perror("signal sigalrm error");
    }

    //设置定时器
    alarm(5);
    printf("begin running main\n");
    out_data();
    printf("end running main\n");

    return 0;
}