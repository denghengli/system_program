#include <unistd.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shm_account.h"
#include "sem_pv.h"

//取款 amt:取款的金额
double withdraw(Account *a, double amt)
{
    assert(a != NULL);

    sem_P(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做P(减1)操作
    if(amt < 0 || amt > a->balance){
        sem_V(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做V(加1)操作
        return 0.0;
    }

    double balance = a->balance;
    sleep(1);
    balance -= amt;
    a->balance = balance;
    sem_V(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做V(加1)操作

    return amt;
}

//存款
double deposit(Account *a, double amt)
{
    assert(a != NULL);

    sem_P(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做P(减1)操作
    if(amt < 0){
        sem_V(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做V(加1)操作
        return 0.0;
    }

    double balance = a->balance;
    sleep(1);
    balance += amt;
    a->balance = balance;
    sem_V(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做V(加1)操作

    return amt;
}

double get_balance(Account *a)
{
    assert(a != NULL);

    sem_P(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做P(减1)操作
    double balance = a->balance;
    sem_V(a->semid, 0, 1);//对信号量集semid中的0号信号量/信号灯做V(加1)操作

    return balance;
}
