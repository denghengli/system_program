#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int x,y;

    //STDIN_FILENO被重定向到了管道的读端
    if(read(STDIN_FILENO, &x, sizeof(int)) < 0){
        perror("read error");
    }
    if(read(STDIN_FILENO, &y, sizeof(int)) < 0){
        perror("read error");
    }

    int result = x + y;
    if (write(STDOUT_FILENO, &result, sizeof(int)) != sizeof(int)){
        perror("write error");
    }

    return 0;
}
