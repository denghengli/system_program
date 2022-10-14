#include "job.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *prompt = "mshell>";
#define MAX_COMMAND_LEN 256

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

//命令执行
void execute_cmd(Job *job)
{
    int i;

    for(i=0; i<job->progs_num; i++)
    {
        //cd
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
    }
}

//命令行解析
//例如：ls -l /etc/passwd
void parse_cmd(Job *job, char *line)
{
    char **args = (char**)malloc(100 * sizeof(char*));
    assert(args != NULL);

    //以空格分割命令
    char *cmd = strtok(line, " "); 
    args[0] = (char*)malloc(strlen(cmd) * sizeof(char));
    strcpy(args[0], cmd);//args[0] = "ls"

    int i = 1;
    char *s;
    while((s = strtok(NULL, " ")) != NULL)
    {
        args[i] = (char*)malloc(strlen(s) * sizeof(char));
        strcpy(args[i], s);//args[1] = "-l"  args[2] = "/etc/passwd" 
        i++;
    }

    Program *prog = creat_program(args);
    add_program(job, prog);

    int j;
    for(j=0; j<i; j++){
        //printf("%s\n", args[j]);//测试
        free(args[j]);
    }
    free(args);
}

int main(void)
{
    char buffer[MAX_COMMAND_LEN];
    ssize_t size, len;

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
            parse_cmd(job, buffer);
            /*执行命令*/
            execute_cmd(job);
            /*销毁*/
            destory_job(job);
        }

        write(STDOUT_FILENO, prompt, size);
        memset(buffer, 0, MAX_COMMAND_LEN);
    }

    return 0;
}