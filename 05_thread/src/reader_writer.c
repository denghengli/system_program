#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int value;

    int r_wait;
    pthread_cond_t r_cond;
    pthread_mutex_t r_mutex;

    int w_wait;
    pthread_cond_t w_cond;
    pthread_mutex_t w_mutex;

}Storage;

void set_data(Storage *s, int value)
{
    s->value = value;
}

int get_value(Storage *s)
{
    return s->value;
}

//写者线程执行的线程运行函数
void *write_fn(void *arg)
{
    Storage *s = (Storage*)arg;
    int i = 1;
    for(; i<=100; i++){
        set_data(s, i);
        printf("0x%lx write data: %d\n", pthread_self(), i);

        //等待读者线程准备就绪
        while(!s->r_wait){
            usleep(1000);
        }
        //唤醒读者线程
        s->r_wait = 0;
        pthread_cond_broadcast(&s->r_cond);

        /*
         * 写者线程等待阻塞
         * 等待读者线程读取完数据后通知唤醒它，然后继续写入数据
        */
        pthread_mutex_lock(&s->w_mutex);
        s->w_wait = 1;
        pthread_cond_wait(&s->w_cond, &s->w_mutex);
        pthread_mutex_unlock(&s->w_mutex);
    }
}

//读者线程执行的线程运行函数
void *read_fn(void *arg)
{
    Storage *s = (Storage*)arg;
    int i = 1;

    for(; i<100; i++){
        /*
         * 读者线程等待阻塞
         * 等待写者线程写完数据后通知唤醒它，然后读取数据
        */
        pthread_mutex_lock(&s->r_mutex);
        s->r_wait = 1;
        pthread_cond_wait(&s->r_cond, &s->r_mutex);
        printf("0x%lx read data: %d\n", pthread_self(), get_value(s));
        pthread_mutex_unlock(&s->r_mutex);

        //等待写者线程准备就绪
        while(!s->w_wait){
            usleep(1000);
        }
        //唤醒写者线程
        s->w_wait = 0;
        pthread_cond_broadcast(&s->w_cond);
    }
}

int main(void)
{
    int err;
    pthread_t rth, wth;

    Storage s;
    s.r_wait = 0;
    s.w_wait = 0;
    pthread_cond_init(&s.r_cond, NULL);
    pthread_cond_init(&s.w_cond, NULL);
    pthread_mutex_init(&s.r_mutex, NULL);
    pthread_mutex_init(&s.w_mutex, NULL);

    err = pthread_create(&rth, NULL, read_fn, (void*)&s);
    if (err != 0){
        perror("pthread creat error");
    }
    err = pthread_create(&wth, NULL, write_fn, (void*)&s);
    if (err != 0){
        perror("pthread creat error");
    }

    pthread_join(rth, NULL);
    pthread_join(wth, NULL);

    pthread_cond_destroy(&s.r_cond);
    pthread_cond_destroy(&s.w_cond);
    pthread_mutex_destroy(&s.r_mutex);
    pthread_mutex_destroy(&s.w_mutex);

    return 0;
}













