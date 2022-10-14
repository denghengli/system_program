#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 协同进程案例 */
int main(void)
{
    int fda[2], fdb[2];

    //创建两个管道
    if((pipe(fda) < 0) || (pipe(fdb) < 0)){
        perror("pipe error");
        exit(1);
    }

    //创建子进程
    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    }
    else if (pid > 0)
    {
        /*
            1)父进程负责从标准输入上读取x和y
            2)将x和y写入管道a
            3)从管道b中读取累加的结果并输出
        */
        close(fda[0]);
        close(fdb[1]);
        int x,y;
        //1)
        printf("please input x and y:");
        scanf("%d %d", &x, &y);
        //2)
        write(fda[1], &x, sizeof(int));
        write(fda[1], &y, sizeof(int));
        //3)
        int result;
        read(fdb[0], &result, sizeof(int));//如果管道没有数据会阻塞
        printf("add result is %d\n", result);

        close(fda[1]);
        close(fdb[0]);
        wait(0);
    }
    else if (pid == 0)
    { 
        /*
            1)子进程负责从管道a中读取父进程写入的累加参数x和y
            2)通过exec函数去调用bin/add程序进行累加
            3)将累加的结果写入到管道b
        */
       close(fda[1]);
       close(fdb[0]);

       //将标准输入重定向到管道a的读端(add程序中将从a中读取累加参数x和y)
       if (dup2(fda[0], STDIN_FILENO) != STDIN_FILENO){
           perror("dup2 error");
       }
       //将标准输出重定向到管道b的写端(add程序将计算结果写入到管道b中)
       if (dup2(fdb[1], STDOUT_FILENO) != STDOUT_FILENO){
           perror("dup2 error");
       }

       if (execlp("bin/add", "bin/add", NULL) < 0){
           perror("execlp error");
       }

       close(fda[0]);
       close(fdb[1]);
    } 

    return 0;
}