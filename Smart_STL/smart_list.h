#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"

namespace smart_stl
{
	//定义list中的节点结构
	template<class T>
	struct list_node
	{
		typedef T data;
		list_node<T>* prev;
		list_node<T>* next;
	};

	//定义list中的迭代器
	template<class T>
	struct list_iterator:public bidirectional_iterator<T, size_t>
	{
		//这个T是与list_iterator_模板类中的T是相同的
		typedef list_iterator<T> iterator;

		typedef list_node<T>* list_nodePtr;

	private:
		list_nodePtr nodeP;

		//关于迭代器所要满足的所有操作符
	public:
		bool operator != (const iterator& x) const {return nodeP != x.nodeP;}
		bool operator == (const iterator& x) const {return nodeP == x.nodeP;}

		iterator& operator ++ () {nodeP = (list_nodePtr)nodeP.next; return *this;}
		iterator& operator ++ (int) 
		{
			//初始时候我写的代码是这样的，这是因为我没有搞懂迭代器和指针的区别----“迭代器不仅仅是指针！”
			//list_nodePtr temp = nodeP;
			//nodeP = (list_nodePtr)nodeP.next;
			//return temp;
			iterator temp = *this;
			//直接利用上一个函数中已经实现的++操作符
			++*this;
			return temp;
		}

		iterator& operator -- () {nodeP = (list_nodePtr)nodeP.prev; return *this;}
		iterator& operator -- (int) {iterator temp = *this; --*this; return temp;}

		reference operator * () {return (*nodeP).data;}
		//以下是【成员存取运算子】的标准写法！！！
		pointer operator -> () {return &(operator*());}
	};
}
#endif