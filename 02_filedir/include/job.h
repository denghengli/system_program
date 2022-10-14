#ifndef __JOB_H__
#define __JOB_H__

typedef struct
{
	char **args;//对应于主函数中的char *argv[]参数
}Program;

typedef struct
{
	char *cmd;
	int progs_num;
	Program *progs;
}Job;

extern Program* creat_program(char **arg);
extern void destory_program(Program *prog);

extern Job* creat_job(char *cmd);
extern void destory_job(Job *job);
extern int add_program(Job *job, Program *prog);

#endif