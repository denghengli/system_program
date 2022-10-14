#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int res;    //计算结果
    sem_t sem;

}Result;


//计算并将结果放置到Result中的线程运行函数
void *set_fn(void *arg)
{
    int i = 1, sum = 0;
    Result *r = (Result*)arg;

    for(; i<100; i++) 
        sum+=i;

    //将结果放置到Result中
    r->res = sum;
    
    //释放信号量
    sem_post(&r->sem);

    return (void*)0;
}

//获取结果的线程运行函数
void *get_fn(void *arg)
{
    Result *r = (Result*)arg;

    sleep(1);

    //获取信号量
    sem_wait(&r->sem);

    //获取结果
    int res = r->res;
    printf("0x%lx get sum is %d\n", pthread_self(), res);

    return (void*)0;
}

/*
 * 线程同步案例
 * 一个线程负责计算结果，一个线程负责获取结果
*/
int main(void)
{
    int err;
    pthread_t cal, get;

    Result r;
    sem_init(&r.sem, 0, 0);

    //获取结果线程
    if((err=pthread_create(&get, NULL, get_fn, (void*)&r)) != 0){
        perror("pthread creat error");
    }
    //计算结果线程
    if((err=pthread_create(&cal, NULL, set_fn, (void*)&r)) != 0){
        perror("pthread creat error");
    }

    pthread_join(cal, NULL);
    pthread_join(get, NULL);

    sem_destroy(&r.sem);

    return 0;
}


