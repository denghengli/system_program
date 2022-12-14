#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

sem_t sem1;
sem_t sem2;

void* a_fn(void *arg)
{
    sem_wait(&sem1);
    printf("thread a running\n");
    return (void*)0;
}

void* b_fn(void *arg)
{
    sem_wait(&sem2);
    printf("thread b running\n");
    sem_post(&sem1);
    return (void*)0;
}

void* c_fn(void *arg)
{
    printf("thread c running\n");
    sem_post(&sem2);
    return (void*)0;
}

int main(void)
{
    int err;
    pthread_t a,b,c;

    //线程信号量初始化，初始值为0
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    pthread_create(&a, NULL, a_fn, (void*)0);
    pthread_create(&b, NULL, b_fn, (void*)0);
    pthread_create(&c, NULL, c_fn, (void*)0);

    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);

    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}