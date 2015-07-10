#ifndef _SMART_HEAP_H_
#define _SMART_HEAP_H_
#include "smart_iterator_base.h"

namespace smart_stl
{

	//samrt_heap的四个功能：push_heap、pop_heap、make_heap、sort_heap
	//heap的相关结构使用有所规定：1、first~last之间是heap的底层容器；2、last前的元素是待排序的元素

	//我还没有搞清楚为什么这个模板要获得元素的类型，直接用*（last-1），不可以吗？
	template<class RandomAccessIterator, class Distance, class T>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, const T& val)
	{
		//这个也就是《算法4》中的swim函数的变形，不过这里的swim的效率还是要更高一点，它没有进行交换
		//仅仅是将父节点值赋给holeIndex，省去了一次的赋值事件（比较精彩！！！）
		T val_copy = val;
		Distance parent = (holeIndex - 1)/2;
		while(holeIndex > topIndex && *(first + parent) < val_copy)
		{
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1)/2;
		}
		*(first + holeIndex) = val_copy;
	}

	template<class RandomAccessIterator, class Distance, class T>
	inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
	{
		//这里可以确定，我们在进行迭代器减法的时候产生的确实是distance_type的类型，但是后面又减去1，这样可能会产生类型提升，所以
		//继续用distance_type()进行强制转换
		_push_heap(first, Distance(first - last - 1), Distance(0), T(*(last - 1)));
	}


	template<class RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		//这里采用的技术是类型传递
		push_heap_aux(first, last, distance_type(first), value_type(first));
	}

	//=======================默认的push_heap是max-heap，现添加可以自定义排序规则的============================
	template<class RandomAccessIterator, class Distance, class T, class Compare comp>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, const T& val, Compare comp)
	{
		//这个也就是《算法4》中的swim函数的变形，不过这里的swim的效率还是要更高一点，它没有进行交换
		//仅仅是将父节点值赋给holeIndex，省去了一次的赋值事件（比较精彩！！！）
		T val_copy = val;
		Distance parent = (holeIndex - 1)/2;
		while(holeIndex > topIndex && comp(*(first + parent), val_copy))
		{
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1)/2;
		}
		*(first + holeIndex) = val_copy;
	}

	template<class RandomAccessIterator, class Distance, class T, class Compare>
	inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
	{
		//这里可以确定，我们在进行迭代器减法的时候产生的确实是distance_type的类型，但是后面又减去1，这样可能会产生类型提升，所以
		//继续用distance_type()进行强制转换
		_push_heap(first, Distance(first - last - 1), Distance(0), T(*(last - 1)), comp);
	}


	template<class RandomAccessIterator, class Compare>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		//这里采用的技术是类型传递
		push_heap_aux(first, last, distance_type(first), value_type(first), comp);
	}
}


#endif	_SMART_HEAP_H_