#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__
#include <pthread.h>
#include <semaphore.h>

//银行账户
typedef struct
{
    int code;//账号
    double balance;//账户余额

    //银行账户进行加锁
    //pthread_mutex_t mutex;//方法1:互斥锁
    //pthread_rwlock_t rwlock;//方法2:读写锁
    sem_t sem;//方法3:信号量
    
}Account;

extern Account *create_account(int code, double balance);//创建
extern void destory_account(Account *a);//销毁
extern double withdraw(Account *a, double amt);//取款
extern double deposit(Account *a, double amt);//存款
extern double get_balance(Account *a);//查看余额

#endif