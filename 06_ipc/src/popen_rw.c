#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/*标准库中的管道操作案例*/
int main(void)
{
    FILE *fp;

    //命令执行的结果放置在fp指向的结构体缓存中
    fp = popen("cat /etc/passwd", "r");
    char buf[512];
    memset(buf, 0, sizeof(buf));
    while(fgets(buf, sizeof(buf), fp) != NULL){
        printf("%s", buf);
    }
    pclose(fp);

    printf("--------------------------\n");

    //为wc命令提供统计的数据,wc命令用于统计行数
    fp = popen("wc -l", "w");
    //向fp指向的结构体缓存中写入数据
    fprintf(fp, "1\n2\n3\n");
    pclose(fp);

    exit(0);
}