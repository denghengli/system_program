#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct 
{
    int d1;
    int d2;
}Arg;

void *th_fn(void *arg)
{
    Arg *r = (Arg *)arg;

    return (void*)(r->d1 + r->d2);
}

int main(void)
{
    int err;
    pthread_t th;
    Arg r = {20, 50};

    if((err=pthread_create(&th, NULL, th_fn, (void*)&r)) != 0){
        perror("pthread_create error");
    }

    int *result;
    pthread_join(th, (void**)&result);
    printf("result is %d\n", (int)result);

    return 0;
}