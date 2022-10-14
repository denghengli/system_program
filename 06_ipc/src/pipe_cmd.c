#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *cmd1[3] = {"/bin/cat", "/etc/passwd", NULL};
char *cmd2[3] = {"/bin/grep", "root", NULL};

/*
 *实现命令行管道命令功能
 * /bin/cat /etc/passwd | /bin/grep root
*/
int main(void)
{
    int fd[2];

    //创建管道
    if(pipe(fd) < 0){
        perror("pipe error");
        exit(1);
    }

    //创建进程扇
    int i = 0;
    pid_t pid;
    for(i=0; i<2; i++)
    {
        pid = fork();

        if(pid  < 0)
        {
            perror("fork error");
            exit(1);
        }
        else if(pid > 0)//父进程
        { 
            if(i == 1)//父进程要等到子进程全部创建完毕后才去回收
            {
                close(fd[0]);
                close(fd[1]);
                wait(0);//回收子进程1
                wait(0);//回收子进程2
            }
        }
        else if (pid == 0) //子进程,子进程会复制一份父进程的管道fd
        {
            if (i == 0) //第一个子进程,负责往管道写入数据
            {
                close(fd[0]);//关闭读端fd
                
                /*
                 *将标准输出重定向到管道的写端
                 *cat命令执行的结果会写入到管道的写端
                 */
                if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO){
                    perror("dup2 error");
                }

                //调用exec函数执行cat命令
                if(execvp(cmd1[0], cmd1) < 0){
                    perror("execvp error");
                    exit(1);
                }

                close(fd[1]);
                break;
            }

            if (i == 1) //第二个子进程,负责从管道读取数据
            {
                close(fd[1]);//关闭写端fd

                /*
                 *将标准输入重定向到管道的读端
                 *grep命令执行的输入会从管道的读端去读取，而不再是标准输入读取
                 */
                if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
                    perror("dup2 error");
                }

                //调用exec函数执行grep命令
                if(execvp(cmd2[0], cmd2) < 0){
                    perror("execvp error");
                    exit(1);
                }

                close(fd[0]);
                break;
            }
        }
    }

    return 0;
}