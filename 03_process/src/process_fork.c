#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int g_v = 30;

int main(void)
{
    pid_t pid;
    static int s_v = 30;
    int a_v = 30;

    pid = fork(); //创建子进程
    /*在fork之后会运行两个进程(父进程和子进程)*/
    if(pid < 0){
        perror("fork error");
    } 
    //子进程,fork返回的是0
    else if(pid == 0){
        g_v = 40; s_v = 40; a_v = 40;
        printf("I am child process, pid is %d, ppid is %d, fork return is %d\n", getpid(), getppid(), pid);
        printf("g_v: %d, s_v: %d, a_v:%d\n", g_v, s_v, a_v);
        printf("g_v addr: %p, s_v addr: %p, a_v addr:%p\n", &g_v, &s_v, &a_v);
    } 
    //父进程，fork返回的是子进程的pid
    else if(pid > 0){
        g_v = 50; s_v = 50; a_v = 50;
        printf("I am parent process, pid is %d, ppid is %d, fork return is %d\n", getpid(), getppid(), pid);
        printf("g_v: %d, s_v: %d, a_v:%d\n", g_v, s_v, a_v);
        printf("g_v addr: %p, s_v addr: %p, a_v addr:%p\n", &g_v, &s_v, &a_v);
    }

    //父进程 和 子进程 都会执行
    printf("pid: %d\n", getpid());
    printf("g_v: %d, s_v: %d, a_v:%d\n", g_v, s_v, a_v);
    printf("g_v addr: %p, s_v addr: %p, a_v addr:%p\n\n", &g_v, &s_v, &a_v);

    sleep(1);

    return 0;
}

int main1(void)
{
    pid_t pid;
    char *s = "hello world, process fork";
    ssize_t size = strlen(s) * sizeof(char);

    /* 父进程执行的 */
    //标准IO方式(带缓存 ---> 全缓存),缓存建在父进程的堆中,fork子进程时会复制一份给子进程
    FILE *fp = fopen("s.txt", "w");
    fprintf(fp, "%s", s);

    //内核提供的系统调用方式(不带缓存,直接写入文件),fork子进程时只会把文件描述符复制给子进程
    int fd = open("s_fd.txt", O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG);//O_TRUNC
    write(fd, s, size);

    pid = fork(); //创建子进程
    /*在fork之后会运行两个进程(父进程和子进程)*/
    if(pid < 0){
        perror("fork error");
    } 
    //子进程,fork返回的是0
    else if(pid == 0){
        printf("I am child process, pid is %d, ppid is %d, fork return is %d\n", getpid(), getppid(), pid);
    } 
    //父进程，fork返回的是子进程的pid
    else if(pid > 0){
        printf("I am parent process, pid is %d, ppid is %d, fork return is %d\n", getpid(), getppid(), pid);
    }

    //父进程 和 子进程 都会执行
    printf("pid: %d\n", getpid());
    //父子进程操作时,是各自写入自己的缓存中。直到结束进程清缓存的时候,写入同一个文件中,所以s.txt中会有2组相同的数据
    //hello world, process fork  end!!!
    //hello world, process fork  end!!!
    fprintf(fp, "  end!!!\n");

    //父子进程操作时操作的是同一个文件。所以s_fd.txt中有这样的数据：
    //hello world, process fork  end!!!  end!!!
    write(fd, "  end!!!\n", strlen("  end!!!") * sizeof(char));

    sleep(1);

    //父进程 和 子进程 都会执行,各自释放
    fclose(fp);
    close(fd);

    return 0;
}
