#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include <cstddef>

namespace smart_stl
{
	//我们想要理解deque出现的意义，deque的表面的使用方法就像“vector”一样，但是它是由一段段连续的空间组成的，表面上与
	//vector相同，但是它在“头尾”的存取效率更高，如果关注头尾部分的话，可以使用deque
	//deque中的迭代器是比较特殊的迭代器，因为要考虑到从一个段跳到上一个段或者是下一个段
	inline size_t deque_buf_size(size_t sz)
	{
		return n != 0 ? n : ( sz > 512 ? size_t(1) : size_t(512 / sz) );
	}

	//用户应该不用去管每个bucket中有多少个元素，这样是没有意义的，这是相当于内部结构，用户在使用的时候也没有直接去
	//设定每个bucket中有多少个元素
	//时刻记住deque拥有一个中控区，中控区上的每个点指向缓冲区，缓冲区中默认是有512bytes的空间，如果相关的类型是比512bytes大
	//的话，缓冲区中就只存储这种类型的一个数据
	template<class T, class Ref, class Ptr>
	struct deque_iterator
	{
		//偏特化
		//下面这个iterator应该是留给deque的数据结构用的
		typedef deque_iterator<T, T&, T*> iterator;
		typedef deque_iterator<T, const T&, const T*> const_iterator;

		static size_t buffer_size() {return deque_buf_size();}

		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef ptrdiff_t distance_type;

		//所需要的其他定义
		typedef size_t size_type;
		typedef T** map_pointer;		//deque的主控被称为map
		//尽量来维持deque_iterator的“原始感”？
		typedef deque_iterator self;

		//deque_iterator需要的相关成员
		T* cur;		//当前迭代器所指的缓冲区的当前点
		T* first;		//当前迭代器所指的缓冲区的起始点
		T* last;		//当前迭代器所指的缓冲区的终止点（不包括）
		map_pointer node;		//中控区map中的node点


		//迭代器中所需要的各种操作符
		//迭代器中所需要的各种逻辑比较符
		//回顾一下list中的迭代器，它是只有operator==和operator！=，这是和它的具体结构有关，而deque的表面现象是与“vector”类似，
		//所以有一些新的>，<，<+等操作
		bool operator == (const self& deq_iter) {return cur == deq_iter.cur;}
		bool operator != (const self& deq_iter);
		bool operator > (const self& deq_iter);
		bool operator >= (const self& deq_iter);
		bool operator < (const self& deq_iter);
		bool operator <= (const self& deq_iter);

		//deque_iterator所要的特定迭代器，“跳转缓冲区”set_node
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + distance_type(buffer_size());
		}
	};
}


#endif