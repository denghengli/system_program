#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void* th_fn(void *arg)
{
    int temp = (int)arg;

    return (void*)temp;
}

void out_state(pthread_attr_t *attr)
{
    int state;

    pthread_attr_getdetachstate(attr, &state);
    if(state == PTHREAD_CREATE_DETACHED){
        printf("detached state\n");
    }else if(state == PTHREAD_CREATE_JOINABLE){
        printf("joinable state\n");
    }else{
        printf("error state\n");
    }
}

int main(void)
{
    int err, result;
    pthread_t default_th, detach_th;
    pthread_attr_t attr;//定义线程属性

    pthread_attr_init(&attr);//初始化线程属性

    //以分离属性的默认值正常启动
    out_state(&attr);
    if((err = pthread_create(&default_th, &attr, th_fn, (void*)10)) != 0){
        perror("pthread creat error");
    }
    pthread_join(default_th, (void*)&result);
    printf("default_th result is:%d\n", result);

    printf("--------------------------------------------------\n");

    //设置分离属性为分离状态启动
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    out_state(&attr);
    if((err = pthread_create(&detach_th, &attr, th_fn, (void*)20)) != 0){
        perror("pthread creat error");
    }
    //pthread_join(detach_th, (void*)&result);//这里调用会失败
    printf("detach_th result is:%d\n", result);

    //销毁线程属性
    pthread_attr_destroy(&attr);

    return 0;
}
