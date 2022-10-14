#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

//银行账户
typedef struct
{
    int code;//账号
    double balance;//账户余额

    int semid;//在共享资源上绑定一个信号量集

}Account;

extern double withdraw(Account *a, double amt);//取款
extern double deposit(Account *a, double amt);//存款
extern double get_balance(Account *a);//查看余额

#endif