#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "ipc.h"
#include "log_printf.h"
#include "OMainLoop.h"
#include "OList.h"
#include "OBuffer.h"

extern OMainLoop *gMainLoop;

#endif