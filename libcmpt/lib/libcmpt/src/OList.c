/*
 * =====================================================================================
 *
 *       Filename:  OList.c
 *
 *    Description:  链表容器的实现
 *
 *        Version:  1.0
 *        Created:  20220507
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  denghengli
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>
	
#include "log_printf.h"
#include "OList.h"

struct double_list_node
{
	struct double_list_node *prev; /* prev node*/
    struct double_list_node *next; /* next node*/
    OListAnyMember *value; /* node value */
};

struct PRIVATE_DATA
{
	struct double_list_node *head;
	struct double_list_node *tail;
	int num;	//链表成员数量
	int size;	//链表成员大小
};

/**
* @brief 向OList链表中插入节点,内部会给节点数据分配memberSize大小的空间,然后将member拷贝过去
*
* @param pOList OList指针
* @param before 在此节点之前插入(如果为0，则默认为追加)
* @param member 插入的节点
*
* @return 新插入节点的迭代器
*/
static OListIterator insert(OList *pOList, OListIterator before, const OListAnyMember *member)
{
    struct double_list_node *node = NULL;
    struct double_list_node *temp = NULL;
    OListAnyMember *data = NULL;
    int i = 0;
    
    //如果为0，则默认为追加
    if(before == NULL)
    {
        return pOList->append(pOList, member);
    }
    else
    {
        for(i=0,temp=pOList->privateData->head; i<pOList->privateData->num; i++,temp=temp->next)
        {
            if(temp == before)
                break;
        }

        //参考节点不在链表中
        if(i == pOList->privateData->num)
            return NULL;

        //申请节点空间
    	node = OLIST_CALLOC(1, sizeof(struct double_list_node));
    	if(node == NULL)
    		return NULL;

    	//申请数据空间
    	data = OLIST_CALLOC(1, pOList->privateData->size);
    	if(data == NULL)
    	{
    		OLIST_FREE(node);
    		return NULL;
    	}

    	//赋值
    	node->next = NULL;
    	node->prev = NULL;
    	memcpy(data, member, pOList->privateData->size);    
        node->value = data;
        
    	//插入链表
    	if(temp == pOList->privateData->head)
    	{
    	    temp->prev = node;
    	    node->next = temp;
    	    pOList->privateData->head = node;
    	}
    	else
    	{
    	    temp->prev->next = node;
    	    node->prev = temp->prev;
    	    node->next = temp;
    	    temp->prev = node;
    	}
    	pOList->privateData->num += 1;
    }
    
	return node;
}

/**
* @brief 向OList链表尾部追加节点,内部会给节点数据分配memberSize大小的空间,然后将member拷贝过去
*
* @param pOList OList指针
* @param member 追加的节点
*
* @return 新追加节点的迭代器
*/
static OListIterator append(OList *pOList, const OListAnyMember *member)
{
	struct double_list_node *node = NULL;
	OListAnyMember *data = NULL;

	//申请节点空间
	node = OLIST_CALLOC(1, sizeof(struct double_list_node));
	if(node == NULL)
		return NULL;

	//申请数据空间
	data = OLIST_CALLOC(1, pOList->privateData->size);
	if(data == NULL)
	{
		OLIST_FREE(node);
		return NULL;
	}

	//赋值
	node->next = NULL;
	node->prev = NULL;
	memcpy(data, member, pOList->privateData->size);
    node->value = data;
    
	//尾插法插入链表
	if(pOList->privateData->head == NULL && pOList->privateData->tail == NULL && pOList->privateData->num == 0)
	{
	    pOList->privateData->head = node;
	    pOList->privateData->tail = node;
	}
	else
	{
	    node->prev = pOList->privateData->tail;
	    pOList->privateData->tail->next = node;
	    pOList->privateData->tail = node;
	}
	pOList->privateData->num += 1;

	return node;
}

/**
* @brief 更新OList链表中pos节点的内容
*
* @param pOList OList指针
* @param pos 节点的迭代器
* @param member 节点的新内容
*
* @return 成功返回pos，失败返回end(说明pos无效)
*/
static OListIterator update(OList *pOList, OListIterator pos, const OListAnyMember *member)
{
    struct double_list_node *temp = NULL;
    int i = 0;

    for(i=0,temp=pOList->privateData->head; i<pOList->privateData->num; i++,temp=temp->next)
    {
        if(temp == pos)
            break;
    }

    if(i == pOList->privateData->num)
        return NULL;

    memset(temp->value, 0, pOList->privateData->size);
    memcpy(temp->value, member, pOList->privateData->size);
        
	return pos;
}

/**
* @brief 获取当前OList链表中的节点个数
*
* @param pOList OList指针
*
* @return >=0:节点个数
*/
static int size(OList *pOList)
{
	return pOList->privateData->num;
}

/**
* @brief 获取第i个节点（i：[0,size).）
*
* @param pOList OList指针
*
* @return 节点指针(如果NULL，说明第i个节点不存在)   
*/
static OListAnyMember *at(OList *pOList, int i)
{
    struct double_list_node *temp = NULL;
    int cnt = 0;

    if((i+1) > pOList->privateData->num)
        return NULL;
        
    for(cnt=0,temp=pOList->privateData->head; cnt<i; cnt++,temp=temp->next);
    
	return temp->value;
}

/**
* @brief 获取指定节点
*
* @param pOList OList指针
* @param pos 节点的迭代器
*
* @return 节点指针(如果NULL，说明pos迭代器无效)   
*/
static OListAnyMember *value(OList *pOList, OListIterator pos)
{
    struct double_list_node *temp = NULL;
    int i = 0;
      
    for(i=0,temp=pOList->privateData->head; i<pOList->privateData->num; i++,temp=temp->next)
    {
        if(temp == pos)
            break;
    }

    //参考节点不在链表中
    if(i == pOList->privateData->num)
        return NULL;
 
	return temp->value;
}

/**
* @brief 查询OList链表中第i个节点的迭代器
*
* @param pOList OList指针
* @param i 节点的索引
*
* @return 节点的迭代器
*/
static OListIterator iterAt(OList *pOList, int i)
{
    struct double_list_node *temp = NULL;
    int cnt = 0;

    if((i+1) > pOList->privateData->num)
        return NULL;
        
    for(cnt=0,temp=pOList->privateData->head; cnt<i; cnt++,temp=temp->next);
    
	return temp;
}

/**
* @brief 获取第1个节点
*
* @param pOList OList指针
*
* @return 节点的迭代器
*/
static OListIterator begin(OList *pOList)
{  
	return pOList->privateData->head;
}
 
/**
* @brief 获取最后一个节点的下一个节点（为无效的节点，表示链表的结束位置）
*
* @param pOList OList指针
*
* @return 节点的迭代器
*/
static OListIterator end(OList *pOList)
{  
    if(pOList->privateData->tail) 
	    return pOList->privateData->tail->next;
	else return NULL;    
}

/**
* @brief 获取pos节点的下一个节点
*
* @param pOList OList指针
* @param pos 节点的迭代器
*
* @return 节点的迭代器(如果==end，说明已到末尾)
*/
static OListIterator next(OList *pOList, OListIterator pos)
{
    struct double_list_node *temp = NULL;
    int i = 0;

    if(pos == NULL)
        return NULL;
        
    for(i=0,temp=pOList->privateData->head; i<pOList->privateData->num; i++,temp=temp->next)
    {
        if(temp == pos)
            break;
    }

    //参考节点不在链表中
    if(i == pOList->privateData->num)
        return NULL;
    
	return temp->next;
}

/**
* @brief 获取pos节点的上一个节点
*
* @param pOList OList指针
* @param pos 节点的迭代器（如果pos为end，则上一个节点为链表的最后一个节点）
*
* @return 节点的迭代器(如果==end，说明已到头部)
*/
static OListIterator prev(OList *pOList, OListIterator pos)
{
    struct double_list_node *temp = NULL;
    int i = 0;

    if(pos == NULL)
        return pOList->privateData->tail;
        
    for(i=0,temp=pOList->privateData->head; i<pOList->privateData->num; i++,temp=temp->next)
    {
        if(temp == pos)
            break;
    }

    //参考节点不在链表中
    if(i == pOList->privateData->num)
        return NULL;
    
	return temp->prev;
}

/**
* @brief 删除指定节点
*
* @param pOList OList指针
* @param pos 节点的迭代器
*/
static void erase(OList *pOList, OListIterator pos)
{
    struct double_list_node *temp = NULL;
    int i = 0;

    if(pos == NULL)
        return;
        
    for(i=0,temp=pOList->privateData->head; i<pOList->privateData->num; i++,temp=temp->next)
    {
        if(temp == pos)
            break;
    }

    //参考节点不在链表中
    if(i == pOList->privateData->num)
        return;

    //删除节点为头节点
    if(temp == pOList->privateData->head)
    {
        if(temp->next) temp->next->prev = NULL;
        pOList->privateData->head = temp->next;
    }
    //删除节点为尾节点
    else if(temp == pOList->privateData->tail)
    {
        if(temp->prev) temp->prev->next = NULL;
        pOList->privateData->tail = temp->prev;
    }
    else
    {
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
    }

    //释放节点空间
    temp->next = NULL;
    temp->prev = NULL;
    OLIST_FREE(temp->value);
    temp->value = NULL;

    OLIST_FREE(temp);
    temp = NULL;

    pOList->privateData->num--;

    //这里很重要！在节点全部删除后，清零指针的值
    if(!pOList->privateData->num)
    {
        pOList->privateData->head = NULL;
        pOList->privateData->tail = NULL;
    }
}

/**
* @brief 删除第i个节点
*
* @param pOList OList指针
* @param i 位置索引 
*/
static void eraseAt(OList *pOList, int i)
{
    struct double_list_node *temp = NULL;
    int cnt = 0;

    if((i+1) > pOList->privateData->num)
        return;
        
    for(cnt=0,temp=pOList->privateData->head; cnt<i; cnt++,temp=temp->next);

    pOList->erase(pOList, (void*)temp);
}

/**
* @brief 删除链表所有节点
*
* @param pOList OList指针
*/
static void clear(OList *pOList)
{
    struct double_list_node *node = NULL;
    
	while(pOList->privateData->head != NULL && pOList->privateData->num)
	{
	    node = pOList->privateData->head;
	    pOList->privateData->head = pOList->privateData->head->next;
	    pOList->privateData->num--;
	    //释放内存
	    OLIST_FREE(node->value);
	    node->value = NULL;

	    node->next = NULL;
	    node->prev = NULL;
	    OLIST_FREE(node);
	    node = NULL;
	}

	pOList->privateData->tail = NULL;
}

/**
 * @brief 初始化一个链表对象（内部会动态分配内存空间，使用完后必须使用destroyOListObj接口进行释放）
 *
 * @param memberSize 链表成员大小
 *
 * @return 链表对象指针
 */
OList *initOListObj(int memberSize)
{
	struct _o_list *list = NULL;

	list = OLIST_CALLOC(1, sizeof(struct _o_list));
	if(!list)
	{
		logPrintf(LOG_APP_ERROR, "Init OList object fail!\n");
		return NULL;
	}

	list->privateData = OLIST_CALLOC(1, sizeof(struct PRIVATE_DATA));
	if(!list->privateData)
	{
		logPrintf(LOG_APP_ERROR, "Init OList object fail!\n");
		return NULL;
	}

	list->insert = insert;
	list->append = append;
	list->update = update;
	list->size = size;
	list->at = at;
	list->value = value;
	list->iterAt = iterAt;
	list->begin = begin;
	list->end = end;
	list->next = next;
	list->prev = prev;
	list->erase = erase;
	list->eraseAt = eraseAt;
	list->clear = clear;

	list->privateData->num = 0;
	list->privateData->size = memberSize;
	list->privateData->head = NULL;
	list->privateData->tail = NULL;

	return list;
}
 
/**
 * @brief 清除list对象,释放空间
 *
 * @param pOList
 */
void destroyOListObj(OList *pOList)
{
    //释放节点数据
	pOList->clear(pOList);

	//释放头结点
	OLIST_FREE(pOList->privateData);
	pOList->privateData = NULL;

	//释放对象
	OLIST_FREE(pOList);
	pOList = NULL;
}

