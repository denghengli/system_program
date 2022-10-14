#ifndef __JOB_H__
#define __JOB_H__
#include <sys/types.h>

//重定向枚举
enum RedirectType{RedirectRead, RedirectWrite, RedirectAppend};

//重定向
typedef struct {
	enum RedirectType redirect;
	int fd;
}Redirection;

typedef struct{
	char **args;//对应于主函数中的char *argv[]参数

	Redirection *redireacts;
	int redirect_num;
	pid_t pid;
}Program;

typedef struct{
	char *cmd;
	int progs_num;
	Program *progs;
	pid_t pgid;//进程组ID
}Job;

extern Program* creat_program(char **arg);
extern void destory_program(Program *prog);
extern int add_program(Job *job, Program *prog);//向job中添加program

extern Redirection* creat_redirect(int fd, enum RedirectType type);
extern void destory_redirect(Redirection* r);
extern void add_redirection(Program *prog, Redirection *r);//向program中添加redirection

extern Job* creat_job(char *cmd);
extern void destory_job(Job *job);

#endif