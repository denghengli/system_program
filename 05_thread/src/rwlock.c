#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

pthread_rwlock_t rwlock;

//2次加锁
int main(int argc, char *argv[])
{
    if (argc < 3){
        printf("-usage:%s [r|w] [r|w]\n", argv[0]);
        exit(1);
    }

    //读写锁初始化
    pthread_rwlock_init(&rwlock, NULL);

    //第一个读写锁
    if(!strcmp("r", argv[1]))
    {
        if(pthread_rwlock_rdlock(&rwlock) != 0){ //成功返回0
            printf("first read lock failure\n");
        } else {
            printf("first read lock success\n");
        }
    }
    else if(!strcmp("w", argv[1]))
    {
        if(pthread_rwlock_wrlock(&rwlock) != 0){ //成功返回0
            printf("first write lock failure\n");
        } else {
            printf("first write lock success\n");
        }
    }

    //第二个读写锁
    if(!strcmp("r", argv[2]))
    {
        if(pthread_rwlock_rdlock(&rwlock) != 0){ //成功返回0
            printf("second read lock failure\n");
        } else {
            printf("second read lock success\n");
        }
    }
    else if(!strcmp("w", argv[2]))
    {
        if(pthread_rwlock_wrlock(&rwlock) != 0){ //成功返回0
            printf("second write lock failure\n");
        } else {
            printf("second write lock success\n");
        }
    }

    //释放读写锁
    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);

    return 0;
}
