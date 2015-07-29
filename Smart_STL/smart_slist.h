#ifndef _SMART_SLIST_H_
#define _SMART_SLIST_H_
#include <cstddef>
#include "smart_iterator_base.h"
namespace smart_stl
{
	/*前期需要说明的东西，我们知道list是一个双向的链表，而slist是一个单项的链表，我们没办法知道某一节点的前面是什么，这个需要我们从头结点
	开始一点点的去寻找，这就需要O(N)的时间，效率没有list高，所以只有在头结点附近进行insert和erase，其他地方进行这个操作都是不太明智的选择*/

	//首先构建slist节点结构，在这里我们采用了《effective C++》中的条款44：将与参数无关（non-typeparameters）的代码抽离templates

	//对于list_node_base它将与T无关的代码抽出来形成了list_node_base
	struct slist_node_base
	{
		slist_node_base* next;
	};

	template<class T>
	struct slist_node : public slist_node_base  //is-a的关系
	{
		T data;
	};

	//构建全局函数，已知某一点，在该点后加新点（这里就使用了OO的多态）
	inline slist_node_base* slist_make_link(slist_node_base* prev, slist_node_base* new_node)
	{
		new_node->next = prev->next;
		prev->next = new_node;
		return new_node;
	}

	inline size_t size(slist_node_base* pHead )
	{
		size_t result = 0;
		while(pHead != NULL)
		{
			result++;
			pHead = pHead->next;
		}
		return result;
	}

	//构建slist迭代器的基础结构
	struct slist_iterator_base
	{
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;
		typedef forward_iterator_tag iterator_category;

		slist_node_base* node;

		//构造函数
		explicit slist_iterator_base(slist_node_base* temp) : node(temp) {}

		void inc() {node = node->next;}

		bool operator == (const slist_iterator_base& rhs)	 { return node == rhs.node;}
		bool operator != (const slist_iterator_base& rhs) { return node != rhs.node; }
	};

	//构建slist迭代器的模板参数部分
	template<class T, class Ref, class Ptr>		//我还是有点没搞懂为什么要这么做，直接使用T不可以吗
	struct slist_iterator : public slist_iterator_base
	{
		//构建迭代器的步骤一：设定各种类型
		typedef slist_iterator<T, T&, T*> iterator;
		typedef slist_iterator<T, const T&, const T*> const_iterator;
		typedef slist_iterator<T, Ref, Ptr> self;

		//构建迭代器的步骤二：设置构造函数，复制构造函数；
	};

}

#endif