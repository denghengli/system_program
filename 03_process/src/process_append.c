#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 父进程 和 子进程 操作同一个文件
 父进程调节偏移量，子进程追加内容
*/
int main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(stderr, "usage: %s file\n", argv[0]);
    }

    int fd = open(argv[1], O_WRONLY);
    if(fd < 0){
        perror("open error");
        exit(1);
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    }
    /*父进程，将文件偏移量调整到文件尾部*/
    else if(pid > 0){
        char *str = "I am parent\n";
        ssize_t size = strlen(str) * sizeof(char);
        if(write(fd, str, size) != size){
            perror("write error");
            exit(1);
        }
        lseek(fd, 0, SEEK_END);
    }
    /*子进程，从文件尾部追加内容*/
    else if(pid == 0){
        char *str = "I am child\n";
        ssize_t size = strlen(str) * sizeof(char);
        sleep(1);//确保父进程先执行
        //此处的fd是从父进程中复制过来的
        //但是和父进程中的fd都是指向同一个文件
        if(write(fd, str, size) != size){
            perror("write error");
            exit(1);
        }
    }

    printf("pid: %d finish\n", getpid());

    //父子进程都要去关闭文件描述符
    close(fd);

    return 0;
}

