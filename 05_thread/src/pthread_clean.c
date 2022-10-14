#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void clean_fun(void *arg)
{
    char *s = (char*)arg;
    printf("clean_fun: %s\n", s);
}

void *th_fun(void *arg)
{
    int execute = (int)arg;

    /*
        触发线程调用清理函数的动作：
        1、线程调用pthread_exit()
        2、响应取消请求
        3、线程调用 pthread_cleanup_pop(1)
    */
    pthread_cleanup_push(clean_fun, "first clean fun");
    pthread_cleanup_push(clean_fun, "second clean fun");

    printf("thread running %lx\n", pthread_self());

    //调用pop时就会立刻调用push注册的回调函数clean_fun，而不是要到线程结束!!!
    // pthread_cleanup_pop(execute);
    // pthread_cleanup_pop(execute);

    sleep(2);

    pthread_cleanup_pop(execute);
    pthread_cleanup_pop(execute);

    return (void*)0;
}


int main(void)
{
    int err;
    pthread_t th1, th2;

    if((err=pthread_create(&th1, NULL, th_fun, (void*)1)) != 0){
        perror("pthread_creat err\n");
    }

    if((err=pthread_create(&th2, NULL, th_fun, (void*)0)) != 0){
        perror("pthread_creat err\n");
    }

    pthread_join(th1, NULL);
    printf("th1(%lx) finished\n", th1);

    pthread_join(th2, NULL);
    printf("th2(%lx) finished\n", th2);

    return 0;
}