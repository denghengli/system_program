/*
 * =====================================================================================
 *
 *       Filename:  OList.h
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
#ifndef _O_LIST_
#define _O_LIST_
#include <stdlib.h>

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

//前向声明
typedef struct _o_list OList;//OList容器类型
typedef void * OListIterator;//OList容器迭代器
typedef void   OListAnyMember;//链表节点类型
struct PRIVATE_DATA;

#define OLIST_MALLOC(size)   malloc(size);
#define OLIST_CALLOC(n,size) calloc(n,size);
#define OLIST_FREE(p)        free(p);

/**
 * @brief 初始化一个链表对象（内部会动态分配内存空间，使用完后必须使用destroyOListObj接口进行释放）
 *
 * @param memberSize 链表成员大小
 *
 * @return 链表对象指针
 */
OList *initOListObj(int memberSize);

/**
 * @brief 清除list对象,释放空间
 *
 * @param pOList
 */
void destroyOListObj(OList *pOList);

//OList结构体定义
struct _o_list
{
	struct PRIVATE_DATA *privateData;//链表私有数据，外部无法操作
	
	/**
	 * @brief 向OList链表中插入节点,内部会给节点数据分配memberSize大小的空间,然后将member拷贝过去
	 *
	 * @param pOList OList指针
	 * @param before 在此节点之前插入(如果为0，则默认为追加)
	 * @param member 插入的节点
	 *
	 * @return 新插入节点的迭代器
	 */
	OListIterator (* insert)(OList *pOList, OListIterator before, const OListAnyMember *member);
	
	/**
	 * @brief 向OList链表尾部追加节点,内部会给节点数据分配memberSize大小的空间,然后将member拷贝过去
	 *
	 * @param pOList OList指针
	 * @param member 追加的节点
	 *
	 * @return 新追加节点的迭代器
	 */
	OListIterator (* append)(OList *pOList, const OListAnyMember *member);
	
	/**
	 * @brief 更新OList链表中pos节点的内容
	 *
	 * @param pOList OList指针
	 * @param pos 节点的迭代器
	 * @param member 节点的新内容
	 *
	 * @return 成功返回pos，失败返回end(说明pos无效)
	 */
	OListIterator (* update)(OList *pOList, OListIterator pos, const OListAnyMember *member);
	
	/**
	 * @brief 获取当前OList链表中的节点个数
	 *
	 * @param pOList OList指针
	 *
	 * @return >=0:节点个数
	 */
	int (* size)(OList *pOList);
	
	/**
	 * @brief 获取第i个节点（i：[0,size).）
	 *
	 * @param pOList OList指针
	 *
	 * @return 节点指针(如果NULL，说明第i个节点不存在)	 
	 */
	OListAnyMember *(* at)(OList *pOList, int i);
	
	/**
	 * @brief 获取指定节点
	 *
	 * @param pOList OList指针
	 * @param pos 节点的迭代器
	 *
	 * @return 节点指针(如果NULL，说明pos迭代器无效)	 
	 */
	OListAnyMember *(* value)(OList *pOList, OListIterator pos);
	
	/**
	 * @brief 查询OList链表中第i个节点的迭代器
	 *
	 * @param pOList OList指针
	 * @param i 节点的索引
	 *
	 * @return 节点的迭代器
	 */
	OListIterator (* iterAt)(OList *pOList, int i);
	
	/**
	 * @brief 获取第1个节点
	 *
	 * @param pOList OList指针
	 *
	 * @return 节点的迭代器
	 */
	OListIterator (* begin)(OList *pOList);
		
	/**
	 * @brief 获取最后一个节点的下一个节点（为无效的节点，表示链表的结束位置）
	 *
	 * @param pOList OList指针
	 *
	 * @return 节点的迭代器
	 */
	OListIterator (* end)(OList *pOList);
	
	/**
	 * @brief 获取pos节点的下一个节点
	 *
	 * @param pOList OList指针
	 * @param pos 节点的迭代器
	 *
	 * @return 节点的迭代器(如果==end，说明已到末尾)
	 */
	OListIterator (* next)(OList *pOList, OListIterator pos);
	
	/**
	 * @brief 获取pos节点的上一个节点
	 *
	 * @param pOList OList指针
	 * @param pos 节点的迭代器（如果pos为end，则上一个节点为链表的最后一个节点）
	 *
	 * @return 节点的迭代器(如果==end，说明已到头部)
	 */
	OListIterator (* prev)(OList *pOList, OListIterator pos);

	/**
	 * @brief 删除指定节点
	 *
	 * @param pOList OList指针
	 * @param pos 节点的迭代器
	 */
	void (* erase)(OList *pOList, OListIterator pos);
	
	/**
	 * @brief 删除第i个节点
	 *
	 * @param pOList OList指针
	 * @param i 位置索引 
	 */
	void (* eraseAt)(OList *pOList, int i);
	
	/**
	 * @brief 删除链表所有节点
	 *
	 * @param pOList OList指针
	 */
	void (* clear)(OList *pOList);
	
};

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif

