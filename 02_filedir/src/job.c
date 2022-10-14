#include "job.h"
#include <malloc.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*统计命令行参数个数*/
static int arg_num(char **arg)
{
	int i = 0;
	char *start = arg[0];
	while(start != NULL){
		i++;
		start = arg[i];
	}

	return i;
}

Program* creat_program(char **arg)
{
	int i;

	Program *prog = (Program*)malloc(sizeof(Program));
	assert(prog != NULL);

	/*获取指针数组的数组大小*/
	int counter = arg_num(arg);
	/*为指针数组申请内存*/
	prog->args = (char **)calloc(counter+1, sizeof(char*));

	/*为传入的每个指针数组成员申请内存*/
	for(i=0; i<counter; i++)
	{
		int len = strlen(arg[i]);
		prog->args[i] = (char*)malloc(len);
		assert(prog->args[i] != NULL);
		strcpy(prog->args[i], arg[i]);
	}

	prog->args[i] = NULL; //将多申请的一个指针数组置为空
	return prog;
}

void destory_program(Program *prog)
{
	assert(prog != NULL);
	int i = 0;
	while (prog->args[i] != NULL)
	{
		free(prog->args[i]);//释放指针数组成员
		i++;
	}
	free(prog->args);//释放Program指针数组
	free(prog);//释放Program指针
}

Job* creat_job(char *cmd)
{
	Job *job = (Job*)malloc(sizeof(Job));
	assert(job != NULL);
	
	job->cmd = (char*)malloc(sizeof(char) * strlen(cmd));
	assert(job->cmd != NULL);
	strcpy(job->cmd, cmd);
	job->progs_num = 0;
	job->progs = NULL;
	
	return job;
}

void destory_job(Job *job)
{
	assert(job != NULL);
	free(job->cmd);
	for(int i=0; i<job->progs_num; i++){
		destory_program(&job->progs[i]);
	}
	free(job);
}

int add_program(Job *job, Program *prog)
{
	Program *ps = (Program*)malloc(sizeof(Program) * (job->progs_num+1));

	memcpy(ps, job->progs, job->progs_num*sizeof(Program));
	ps[job->progs_num++] = *prog;

	free(job->progs);
	job->progs = ps;

	return job->progs_num;
}
