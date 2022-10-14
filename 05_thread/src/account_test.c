#include "account.h"
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
    char name[20];
    Account *account;
    double amt;//金额
}OperArg;

//定义取款操作线程运行函数
void * withdraw_fn(void *arg)
{
    OperArg *oper = (OperArg*)arg;
    double amt = withdraw(oper->account, oper->amt);

    printf("%s(0x%lx) withdraw %f from account %d\n", oper->name, pthread_self(),
             amt, oper->account->code);

    return (void*)0;
}

//定义存款线程运行函数
void * deposit_fn(void *arg)
{
    OperArg *oper = (OperArg*)arg;
    double amt = deposit(oper->account, oper->amt);

    printf("%s(0x%lx) deposit %f from account %d\n", oper->name, pthread_self(),
             amt, oper->account->code);

    return (void*)0;
}

/*
 * 线程互斥案例
*/
int main(void)
{
    int err;
    pthread_t boy,girl;
    Account *a = create_account(10001, 10000);//同一个银行账户

    OperArg oper_boy, oper_girl;
    strcpy(oper_boy.name, "boy");
    oper_boy.account = a;
    oper_boy.amt = 1000;

    strcpy(oper_girl.name, "girl");
    oper_girl.account = a;
    oper_girl.amt = 2000;

    //启动两个子线程操作同一个银行账户
    if ((err=pthread_create(&boy, NULL, withdraw_fn, (void*)&oper_boy)) != 0){
        perror("pthread_create error");
    }
    if ((err=pthread_create(&girl, NULL, deposit_fn, (void*)&oper_girl)) != 0){
        perror("pthread_create error");
    }

    //等待线程结束回收资源
    pthread_join(boy, NULL);
    pthread_join(girl, NULL);

    printf("account balance: %f\n", get_balance(a));
    destory_account(a);

    return 0;
}


