#include <unistd.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ADD 5
#define SUB 6

void do_line(char *line);
int get_item(char *line);
void cmd_add(void);
void cmd_sub(void);

char *prompt = "cal: ";
jmp_buf env;

int main(void)
{
    ssize_t size = strlen(prompt) * sizeof(char);
    char buff[256];
    ssize_t len;

    //设置跳转点
    if(setjmp(env) < 0){
        perror("setjmp error");
    }

    write(STDOUT_FILENO, prompt, size);
    while(1)
    {
        len = read(STDIN_FILENO, buff, 256);
        if(len < 0){
            break;
        }

        buff[len-1] = 0;
        do_line(buff);//解析命令行

        write(STDOUT_FILENO, prompt, size);
    }
}

/*
    add 3 4
    sub 5 7
*/
void do_line(char *line)
{
    int cmd = get_item(line);

    switch(cmd)
    {
        case ADD:
            cmd_add();
            break;

        case SUB:
            cmd_sub();
            break;

        default:
            fprintf(stderr, "error commmand\n");
            break;
    }
}

static int is_number(char *item)
{
    int len = strlen(item);
    int i;

    for(i=0; i<len; i++)
    {
        if(item[i] > '9' || item[i] <'0')
            return 0;
    }

    return 1;
}

int get_item(char *line)
{
/*
    add 3 4
    sub 5 7
*/
    char *item = strtok(line, " ");
    if(line != NULL) //这里 item = “add”
    {
        if(!strcmp("add", item)) return ADD;
        if(!strcmp("sub", item)) return SUB;
    }
    else //这里 item = “3” 或 item = “4”
    {
        if(is_number(item)){
            int i = atoi(item);
            return i;
        }else{
            fprintf(stderr, "arg not number\n");
            //进行非局部跳转,不再继续往后执行，重新提示输入
            longjmp(env, 1);
        }
    }
}

void cmd_add(void)
{
    int i = get_item(NULL);
    int j = get_item(NULL);
    printf("result is %d\n", (i+j));
}

void cmd_sub(void)
{
    int i = get_item(NULL);
    int j = get_item(NULL);
    printf("result is %d\n", (i-j));
}

