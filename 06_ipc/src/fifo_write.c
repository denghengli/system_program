#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>

/*命令管道案例*/
int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("usage:%s fifofile(such as: test.pipe) \n", argv[0]);
        exit(1);
    }

    printf("open fifo write...\n");
    //打开命名管道
    int fd = open(argv[1], O_WRONLY);
    if(fd < 0){
        perror("open error");
        exit(1);
    } else {
        printf("open file success: %d\n", fd);
    }

    //从命名管道写入数据
    char *s = "123456789";
    size_t size = strlen(s);
    if(write(fd, s, size) != size){
        perror("write error");
    }
    close(fd);

    exit(0);
}