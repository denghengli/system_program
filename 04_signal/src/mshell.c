#include "job.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char** environ;//引入环境变量
char *prompt = "mshell>";
#define MAX_COMMAND_LEN 256
#define FOREGROUND 0 //后台进程
#define BACKGROUND 1 //前台进程

//例如： cd /bin
void cd_fun(Program *prog)
{
    if(chdir(prog->args[1]) < 0) {
        perror("cd error");
    }
}

//例如：pwd
void pwd_fun(Program *prog)
{
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    if(getcwd(buffer, sizeof(buffer)) == NULL) {
        perror("pwd error");
    }
    printf("%s\n", buffer);
}

//例如：exit
void exit_fun(Program *prog)
{
    exit(0);
}

//例如：env
void env_fun(void)
{
    int i = 0;
    char *env;

    while((env = environ[i]) != NULL) {
        printf("%s\n", env);
        i++;
    }
}

//例如：export CITY=beijing 导出环境变量
void export_fun(Program *prog)
{
    if(prog->args[1] == NULL){
        fprintf(stderr, "export: invalid argument\n");
        return;
    }
    putenv(prog->args[1]);
}

//例如：echo $PATH 查看环境变量
void echo_fun(Program *prog)
{
    char *s = prog->args[1];
    if(s == NULL){
        fprintf(stderr, "echo: invalid argument\n");
        return;
    }

    if(s[0] == '$'){
        char *v = getenv(s+1);
        printf("%s\n", v);
    }else{
        printf("%s\n", s);
    }
}

//命令执行
void execute_cmd(Job *job, int bg)
{
    int i;

    for(i=0; i<job->progs_num; i++)
    {
        //cd /bin
        if(!strcmp(job->progs[i].args[0], "cd")) {
            cd_fun(&job->progs[i]);
            return;
        }

        //pwd
        if(!strcmp(job->progs[i].args[0], "pwd")) {
            pwd_fun(&job->progs[i]);
            return;
        }

        //exit
        if(!strcmp(job->progs[i].args[0], "exit")) {
            exit_fun(&job->progs[i]);
            return;
        }

        //env
        if(!strcmp(job->progs[i].args[0], "env")) {
            env_fun();
            return;
        }

        //export CITY=beijing
        if(!strcmp(job->progs[i].args[0], "export")) {
            export_fun(&job->progs[i]);
            return;
        }

        //echo $PATH
        if(!strcmp(job->progs[i].args[0], "echo")) {
            echo_fun(&job->progs[i]);
            return;
        }

        //执行其他命令
        pid_t pid;
        if((pid = fork()) < 0)
        {
            perror("fork error");
        }
        //子进程，调用exec函数执行系统中的其他命令
        else if(pid == 0)
        {
            //第一个子进程作为组长进程
            if(i == 0){
                setpgid(getpid(), getpid());
                job->pgid = getpgid(getpid());
            }else{
                setpgid(getpid(), job->pgid);
            }
            if(bg == FOREGROUND){
                tcsetpgrp(0, getpgid(getpid()));//设置为前台进程组
            }

            //对标准输入、标准输出、追加输出进行重定向
            job->progs[i].pid = getpid();
            for(int k=0; k<job->progs[i].redirect_num; k++){
                //标准输入重定向
                if(job->progs[i].redireacts[k].redirect == RedirectRead){
                    if(dup2(job->progs[i].redireacts[k].fd, STDIN_FILENO) != STDIN_FILENO){
                        perror("dup2 error");
                    }
                }

                //标准输出重定向
                if(job->progs[i].redireacts[k].redirect == RedirectWrite){
                    if(dup2(job->progs[i].redireacts[k].fd, STDOUT_FILENO) != STDOUT_FILENO){
                        perror("dup2 error");
                    }
                }

                //追加重定向
                if(job->progs[i].redireacts[k].redirect == RedirectAppend){
                    if(dup2(job->progs[i].redireacts[k].fd, STDOUT_FILENO) != STDOUT_FILENO){
                        perror("dup2 error");
                    }
                }
            }

            //execvp(命令的相对或绝对路径, 参数(args))
            if(execvp(job->progs[i].args[0], job->progs[i].args) < 0){
                perror("execvp error");
                exit(1);
            }
        }
        //父进程，等待子进程结束，回收资源
        else
        {
            //第一个子进程作为组长进程
            if(i == 0){
                setpgid(pid, pid);
                job->pgid = getpgid(pid);
            }else{
                setpgid(pid, job->pgid);
            }
            if(bg == FOREGROUND){
                tcsetpgrp(0, job->pgid);
                waitpid(-job->pgid, NULL, WUNTRACED);
            }
            if(bg == BACKGROUND){
                waitpid(-job->pgid, NULL, WNOHANG);
            }
            //wait(NULL);
        }
    }
}

//命令行解析
//例如：ls -l /etc/passwd
//例如：cat > /etc/passwd
void parse_cmd(Job *job, char *line, int *bg)
{
    char **args = (char**)calloc(100, sizeof(char*));
    assert(args != NULL);

    //以空格分割命令
    char *cmd = strtok(line, " "); 
    args[0] = (char*)calloc(strlen(cmd)+1, sizeof(char));
    strcpy(args[0], cmd);//args[0] = "ls"

    Redirection *rs[5];//重定向
    int redirect_num = 0;
    char *s;
    int i = 1;
    *bg = FOREGROUND;//默认为前台进程
    while((s = strtok(NULL, " ")) != NULL)
    {
        if(!strcmp(s, "&")){//后台进程
            *bg = BACKGROUND;
            continue;
        }

        if(!strcmp(s, "<")){ //输入重定向
            char *file = strtok(NULL, " ");
            if(file == NULL) continue;
            else{
                int fd = open(file, O_RDONLY);
                rs[redirect_num++] = creat_redirect(fd, RedirectRead);
            }
            continue;
        }
        if(!strcmp(s, ">")){//输出重定向
            char *file = strtok(NULL, " ");
            if(file == NULL) continue;
            else{
                int fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
                rs[redirect_num++] = creat_redirect(fd, RedirectWrite);
            }
            continue;
        }
        if(!strcmp(s, ">>")){//追加重定向
            char *file = strtok(NULL, " ");
            if(file == NULL) continue;
            else{
                int fd = open(file, O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
                rs[redirect_num++] = creat_redirect(fd, RedirectAppend);
            }
            continue;
        }

        args[i] = (char*)calloc(strlen(s)+1, sizeof(char));
        strcpy(args[i], s);//args[1] = "-l"  args[2] = "/etc/passwd" 
        i++;
    }

    Program *prog = creat_program(args);
    for(int k=0; k<redirect_num; k++){
        add_redirection(prog, rs[k]);
        destory_redirect(rs[k]);//向program中添加redirection
    }
    add_program(job, prog);//向job中添加program

    int j;
    for(j=0; j<i; j++){
        //printf("%s\n", args[j]);//测试
        free(args[j]);
    }
    free(args);
}

void sig_handler(int signo)
{
    if(signo == SIGCHLD){
        //非阻塞模式回收进程组中所有的子进程
        waitpid(-1, NULL, WNOHANG);
        //重新调度为前台进程组
        tcsetpgrp(0, getpgid(getpid()));
    }
}

/*
 *1、支持命令：pwd、cd、exit、env、export、echo、其他命令、
 *2、重定向命令：> < >>
 *3、解析命令中后台进程标志 &
 *4、进程组的创建
 *5、前台进程组的设置
 *6、回收进程组中的所以子进程
 *7、作业控制信号处理：通过fork出的子进程跟终端交互时会产生以下两个信号，
     这两个信号产生时，默认的处理是停止进程。所以要对这两信号加以处理，
     可忽略这两信号
        SIGTTIN:后台进程组的成员读控制终端
        SIGTTOU:后台进程组的成员写控制终端
*/
int main(void)
{
    char buffer[MAX_COMMAND_LEN];
    ssize_t size, len;
    int bg;//设置前台和后台进程的标准

    /*7、作业控制信号的处理*/
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGINT, SIG_IGN);//ctrl+c
    signal(SIGTSTP, SIG_IGN);//ctrl+z
    signal(SIGCHLD, sig_handler);//子进程结束信号

    setpgid(getpid(), getpid());//创建进程组

    memset(buffer, 0, MAX_COMMAND_LEN);
    size = strlen(prompt) * sizeof(char);
    write(STDOUT_FILENO, prompt, size);//打印提示信息

    while(1)
    {
        /*读取命令行的内容*/
        len = read(STDIN_FILENO, buffer, MAX_COMMAND_LEN);

        if(strlen(buffer) > 0)
        {
            buffer[len - 1] = 0;
            Job *job = creat_job(buffer);
            /*解析命令行*/
            parse_cmd(job, buffer, &bg);
            /*执行命令*/
            execute_cmd(job, bg);
            /*销毁*/
            destory_job(job);
        }

        write(STDOUT_FILENO, prompt, size);
        memset(buffer, 0, MAX_COMMAND_LEN);
    }

    return 0;
}