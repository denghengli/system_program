#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct 
{
    char name[20];//线程名称
    int time;//睡眠时间
    int start;//起点
    int end;//终点
}raceArg;


//定义线程运行函数
void* th_fn(void *arg)
{
    raceArg *r = (raceArg*)arg;
    int i = r->start;

    for(i=1; i<=r->end; i++){
        printf("%s(%lx) run %d\n", r->name, pthread_self(), i);
        usleep(r->time);//微妙
    }

    return (void*)r->end;
}

//龟兔赛跑
int main(void)
{
    int err;
    pthread_t rabbit,turtle;
    // raceArg rabbit_arg = {"rabbit", (int)(drand48()*1000000), 10, 100};//drand48 返回服从均匀分布的·[0.0, 1.0] 之间的 double 型随机数
    // raceArg turtle_arg = {"turtle", (int)(drand48()*1000000), 10, 60};
    raceArg rabbit_arg = {"rabbit", 100, 10, 100};//drand48 返回服从均匀分布的·[0.0, 1.0] 之间的 double 型随机数
    raceArg turtle_arg = {"turtle", 100, 10, 60};

    if((err = pthread_create(&rabbit, NULL, th_fn, (void*)&rabbit_arg)) != 0){
        perror("pthread_creat error");
    }

    if((err = pthread_create(&turtle, NULL, th_fn, (void*)&turtle_arg)) != 0){
        perror("pthread_creat error");
    }

    //1、主控线程调用pthread_join，自己会阻塞，直到rabbit,turtle线程结束后方可运行
    int result;

    pthread_join(turtle, (void*)&result);
    printf("turtle return is: %d\n", result);

    pthread_join(rabbit, (void*)&result);
    printf("rabbit return is: %d\n", result);
    
    //sleep(10);//让主控线程睡眠，调度去执行rabbit,turtle两个子线程,否则会退出
    
    printf("control thread id: %lx\n", pthread_self());
    printf("finished!\n");

    return 0;
}