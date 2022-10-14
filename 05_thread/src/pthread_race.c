#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//定义线程运行函数
void* th_fn(void *arg)
{
    int distance = (int)arg;
    int i;

    for(i=1; i<=distance; i++){
        printf("%lx run %d\n", pthread_self(), i);
        //drand48 返回服从均匀分布的·[0.0, 1.0] 之间的 double 型随机数
        int time = (int)(drand48() * 100000);//最大100S
        usleep(time);//微妙
    }

    return (void*)0;
}

//龟兔赛跑
int main(void)
{
    int err;
    pthread_t rabbit,turtle;

    if((err = pthread_create(&rabbit, NULL, th_fn, (void*)50)) != 0){
        perror("pthread_creat error");
    }

    if((err = pthread_create(&turtle, NULL, th_fn, (void*)50)) != 0){
        perror("pthread_creat error");
    }

    //1、主控线程调用pthread_join，自己会阻塞，直到rabbit,turtle线程结束后方可运行
    pthread_join(rabbit, NULL);
    pthread_join(turtle, NULL);

    //sleep(10);//让主控线程睡眠，调度去执行rabbit,turtle两个子线程,否则会退出
    
    printf("control thread id: %lx\n", pthread_self());
    printf("finished!\n");

    return 0;
}