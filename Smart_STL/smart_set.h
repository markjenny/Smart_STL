#ifndef _SMART_SET_H_
#define _SMART_SET_H_

namespace smart_stl
{
	/*****************************************************
	set的value和key是合一的，并且没有重复的值，也正是这个原因；在set的迭代器中是不允许
	通过迭代器对元素进行写入；
	因为set在数据结构上也是拥有链表的，所以在删除、插入节点时，其他迭代器不会失效，当然被删除的迭代器是肯定失效了
	******************************************************/
	//因为set是键值合一的，所以没有Value，也就仅仅靠key进行排序
	template<class Key, class Compare = less<Key>, class Alloc = alloc>
}

#endif