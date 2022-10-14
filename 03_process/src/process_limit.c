#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RLIMIT(name) get_limit(#name, name)

void get_limit(char *name, int resource)
{
    struct rlimit limit;

    if(getrlimit(resource, &limit) < 0){
        perror("getrlimit error");
        return;
    }

    printf("%-15s", name);
    //rlim_cur: 软资源限制
    if(limit.rlim_cur == RLIM_INFINITY){ 
        printf("infinity     "); //无穷大
    }else{
        printf("%10ld", limit.rlim_cur);
    }

    //rlim_max:硬软资源限制
    if(limit.rlim_max == RLIM_INFINITY){ 
        printf("infinity"); //无穷大
    }else{
        printf("%10ld", limit.rlim_max);
    }

    printf("\n");
}


int main(void)
{
    /*获取各个资源的大小*/
    RLIMIT(RLIMIT_AS);  //进程可用存储大小
    RLIMIT(RLIMIT_CORE);//core文件最大字节数
    RLIMIT(RLIMIT_CPU); //CPU时间最大值
    RLIMIT(RLIMIT_DATA);//数据段最大长度
    RLIMIT(RLIMIT_FSIZE);//可创建文件的最大长度
    RLIMIT(RLIMIT_LOCKS);//文件锁的最大数
    RLIMIT(RLIMIT_MEMLOCK);//使用mlock能否在存储器中锁定的最大字节数
    RLIMIT(RLIMIT_NOFILE);//能打开的最大文件数
    RLIMIT(RLIMIT_NPROC);//每个用户ID可拥有的最大子进程数
    RLIMIT(RLIMIT_RSS);//最大驻内存集的字节长度
    RLIMIT(RLIMIT_STACK);//栈的最大长度

    return 0;
}