#ifndef __IO_H__
#define __IO_H__
#include <sys/types.h>

extern void mycopy(int fdin, int fdout);

extern void set_fl(int fd, int flag);
extern void clr_fl(int fd, int flag);

extern int lock_reg(int fd, int cmd, short type, off_t offset, short whence, off_t length);
/*共享读锁,非阻塞式和阻塞式*/
#define READ_LOCK(fd, offset, whence, length) lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, length)
#define READ_LOCKW(fd, offset, whence, length) lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, length)
/*独占写锁,非阻塞式和阻塞式*/
#define WRITE_LOCK(fd, offset, whence, length) lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, length)
#define WRITE_LOCKW(fd, offset, whence, length) lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, length)
/*解锁*/
#define UNLOCK(fd, offset, whence, length) lock_reg(fd, F_SETLK, F_UNLCK,  offset, whence, length)

#endif