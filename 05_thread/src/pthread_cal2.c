#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int res;     //计算结果
    int counter; //等待线程数量计数
    int wait_num;//等待线程的数量
    pthread_cond_t  cond;//条件变量
    pthread_mutex_t mutex;//互斥锁

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
    
    //判断获得结果的线程是否准备好
    while(r->counter < r->wait_num){
        usleep(100);
    }

    //唤醒等待的那个获取结果的线程
    pthread_cond_broadcast(&r->cond);

    return (void*)0;
}

//获取结果的线程运行函数
void *get_fn(void *arg)
{
    Result *r = (Result*)arg;

    sleep(1);

    pthread_mutex_lock(&r->mutex);
    r->counter += 1;//代表获取结果的线程已经准备好
    //获取结果的线程进行等待
    pthread_cond_wait(&r->cond, &r->mutex);
    pthread_mutex_unlock(&r->mutex);//这里的unlock与前面的lock不是一对，b必须释放!!!!! 具体原因如下：
    //pthread_cond_wait函数内部流程:
    // 1)unlock(&mutex);释放锁
    // 2)lock(&mutex);
    // 3)将线程自己插入到条件变量的等待队列中
    // 4)unlock(&mutex)
    // 5)当前等待的线程阻塞 <==等其他线程通知唤醒
    // 6)在唤醒后，lock(&mutex)
    // 7)从等待队列中删除线程

    //获取结果
    int res = r->res;
    printf("0x%lx get sum is %d\n", pthread_self(), res);

    return (void*)0;
}

/*
 * 线程同步案例
 * 一个线程负责计算结果，多个线程负责获取结果
*/
int main(void)
{
    int err;
    pthread_t cal, get1, get2;

    Result r;
    r.counter = 0;
    r.wait_num = 2;
    pthread_cond_init(&r.cond, NULL);
    pthread_mutex_init(&r.mutex, NULL);

    //获取结果线程1
    if((err=pthread_create(&get1, NULL, get_fn, (void*)&r)) != 0){
        perror("pthread creat error");
    }
    //获取结果线程2
    if((err=pthread_create(&get2, NULL, get_fn, (void*)&r)) != 0){
        perror("pthread creat error");
    }

    //计算结果线程
    if((err=pthread_create(&cal, NULL, set_fn, (void*)&r)) != 0){
        perror("pthread creat error");
    }

    pthread_join(cal, NULL);
    pthread_join(get1, NULL);
    pthread_join(get2, NULL);

    pthread_cond_destroy(&r.cond);
    pthread_mutex_destroy(&r.mutex);

    return 0;
}


