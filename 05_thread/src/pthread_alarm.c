#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int signo)
{
    printf("pthread id in the sig_hanlder:%lx\n", pthread_self());
    if(signo == SIGALRM){
        printf("timeout...\n");
    }
    alarm(2);//重新启动
}

void *th_fn(void *arg)
{
    if(signal(SIGALRM, sig_handler) == SIG_ERR){
        perror("signal sigalrm error");
    }

    //在子线程中设置定时器
    //这里其实是发给了主线程（控制线程）
    alarm(2);

    int i;
    for(i=1; i<=100; i++){
        printf("child thread(%lx) i:%d\n", pthread_self(), i);
        sleep(1);
    }
}

/*
 * 线程与信号案例
 * 
*/
int main(void)
{
    int err;
    pthread_t th;
    pthread_attr_t attr;//定义线程属性

    pthread_attr_init(&attr);//初始化线程属性
    //设置分离属性为分离状态启动,不用再调用pthread_join()
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if((err = pthread_create(&th, &attr, th_fn, (void*)0)) != 0){
        perror("pthread creat error");
    }

    //主线程（控制线程继续执行）
    while(1) {
        printf("control thread(%lx) is running...\n\n", pthread_self());
        sleep(10);//睡眠不到10S，就会被定时器唤起
    }

    return 0;
}