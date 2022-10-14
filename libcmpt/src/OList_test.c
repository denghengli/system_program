#include "includes.h"

//打印 OList 内的所有数据
void printfOList(OList *list)
{
    int i = 0;
    OListIterator iter;
    
    logPrintf(LOG_APP_ERROR, "OList list:\n");
    for(iter = list->begin(list); iter != list->end(list); iter = list->next(list, iter))
    {
        //int * data = list->value(list, iter);
        int * data = list->at(list, i);
        logPrintf(LOG_APP_ERROR, "[%d]:%d\n", i, *data);
        i++;
    }
}

int OList_test(void)
{
    int data0 = 10, data1 = 20, data2 = 30;
    
    //初始化 OVector 对象
    OList * list = initOListObj(sizeof(int));
    
    //1、追加一条数据
    logPrintf(LOG_APP_ERROR, "append one data\n");
    list->append(list, &data0);
    printfOList(list);
    
    //2、在第一条数据前插入一条数据
    logPrintf(LOG_APP_ERROR, "insert one data before the first data\n");
    OListIterator iter = list->begin(list); //获取第一条内容的迭代器
    list->insert(list, iter, &data1);
    printfOList(list);
    
    //3、更新第一条数据
    logPrintf(LOG_APP_ERROR, "update the first data\n");
    iter = list->iterAt(list, 0); //获取第一条内容的迭代器
    list->update(list, iter, &data2);
    printfOList(list);
    
    //4、删除第一条数据
    logPrintf(LOG_APP_ERROR, "erase the first data\n");
    //list->eraseAt(list, 0);
    list->erase(list, list->begin(list));
    printfOList(list);
    
    //5、删除所有数据
    logPrintf(LOG_APP_ERROR, "delete all data\n");
    list->clear(list);
    printfOList(list);
    
    //销毁 OList 对象
    destroyOListObj(list);
    
    return 0;
}


