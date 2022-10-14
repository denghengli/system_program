#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern char **environ; //系统定义好的环境变量

void show_env()
{
    int i = 0;
    char *env;

    while((env = environ[i]) != NULL){
        printf("%s\n", env);
        i++;
    }
}

/*通过系统变量方式获取环境变量*/
void main(void)
{
    show_env();
    //环境变量操作
    printf("--------------------------------\n");
    putenv("CITY=beijing");
    setenv("NAME", "denghengli", 1);
    show_env();
    
    return;
}

/*
//通过参数方式获取环境变量
int main(int argc, char *argv[], char *envp[])
{
    int i = 0;
    char *env;

    while((env = envp[i]) != NULL){
        printf("%s\n", env);
        i++;
    }

    return 0;
}*/
