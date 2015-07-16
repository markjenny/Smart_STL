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
	template<class RandomAccessIterator, class Distance, class T, class Compare>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, const T& val, Compare comp)
	{
		//这个也就是《算法4》中的swim函数的变形，不过这里的swim的效率还是要更高一点，它没有进行交换
		//仅仅是将父节点值赋给holeIndex，省去了一次的赋值（比较精彩！！！）
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




	//stl中的pop_heap操作并没有将堆顶的元素取出来，而是直接放在了底部数据(vector)的末尾，所以要得知元素还是
	//要使用back()，取出元素就使用pop_back()，但是不能乱用，乱用不能保证pop_heap的健壮
	
	//len是表示堆最后一个元素的位置（数组下标），注意len其实就是数组的最后一个元素的下标
	template<class RandomAccessIterator, class Distance, class T>
	void _pop_heap(RandomAccessIterator first, Distance len, Distance holeIndex, const T& value)
	{
		//这个函数的作用是调整堆，adjust
		*(first + len) = *(first);
		//因为后面要用到这个值，所以先保存，后面holeIndex会产生变化
		Distance topIndex = holeIndex;
		Distance childIndex = 2*holeIndex + 1;
		while(childIndex <= len - 1)
		{
			if(childIndex < len - 1 && *(first + childIndex) < *(first + len +1))
				childIndex++;
			*(first + holeIndex) = *(first + childIndex);
			holeIndex = childIndex;
			childIndex = 2 * holeIndex + 1;
		}

		//跳出循环表明该点hole现在已经没有子节点了，它已经是叶子节点了，现在开始swim
		_push_heap(first, holeIndex, , value);
	}

	template<class RandomAccessIterator, class Distance, class T>
	inline void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
	{
		_pop_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)));
	}

	//[first,last)这个区间是待排序的所有区间
	template<class RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		pop_heap_aux(first, last, distance_type(first), value_type(first));
	}


	//=======================默认的pop_heap是max-heap，现添加可以自定义排序规则的============================
	template<class RandomAccessIterator, class Distance, class T, class Compare>
	void _pop_heap(RandomAccessIterator first, Distance len, Distance holeIndex, const T& value, Compare comp)
	{
		//这个函数的作用是调整堆，adjust
		*(first + len) = *(first);
		Distance childIndex = 2*holeIndex + 1;
		//因为后面要用到这个值，所以先保存，后面holeIndex会产生变化
		Distance topIndex = holeIndex;
		while(childIndex <= len - 1)
		{
			if(childIndex < len - 1 && comp(*(first + childIndex), *(first + childIndex + 1)))
				childIndex++;
			*(first + holeIndex) = *(first + childIndex);
			holeIndex = childIndex;
			childIndex = 2 * holeIndex + 1;
		}

		//跳出循环表明该点hole现在已经没有子节点了，它已经是叶子节点了，现在开始swim
		_push_heap(first, holeIndex, topIndex, value, comp);
	}

	template<class RandomAccessIterator, class Distance, class T, class Compare>
	inline void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
	{
		_pop_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)), comp);
	}

	//[first,last)这个区间是待排序的所有区间
	template<class RandomAccessIterator, class Compare>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		pop_heap_aux(first, last, distance_type(first), value_type(first), comp);
	}


	//====================smart_heap的第三个功能：sort_heap：重复调用pop_heap即可===========================
	template<class RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		while((last - first) > 1)
			pop_heap(first, last);
	}

	//增加比较规则Compare
	template<class RandomAccessIterator, class Compare>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		while(last - first > 1)
			pop_heap(first, last, comp);
	}

	//smart_heap的最后一个功能：将一个底层数据结构(vector)的数据转换成一个堆，对于
	//smart_heap中的所有数据，都需要先萃取出他的value_type和distance_type
	template<class RandomAccessIterator, class Distance, class T>
	void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
	{
		if(last - first < 2)		//只有一个元素，不需要make heap
			return;
		//len表示vector中最后一个元素的下标
		Distance len = last - first - 1;
		//左后一个也节点对应的父节点即是这个树中的最后一个非叶子节点
		Distance parrent = (len - 1)/2;
		while (parrent >= 0)
		{
			//我对这个问题不是很清楚，准备看一下《C++ Template》看看是否有相关解释
			_pop_heap(first, len, parrent, T(*(first + parrent)));
			parrent--;
		}
	}

	template<class RandomAccessIterator>
	void make_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		make_heap_aux(first, last, distance_type(first), value_type(first));
	}

	//===============为make_heap增加可以自定义的比较类型版本===================
	template<class RandomAccessIterator, class Distance, class T, class Compare>
	void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
	{
		if(last - first < 2)		//只有一个元素，不需要make heap
			return;
		//len表示vector中最后一个元素的下标
		Distance len = last - first - 1;
		//左后一个也节点对应的父节点即是这个树中的最后一个非叶子节点
		Distance parrent = (len - 1)/2;
		while (parrent >= 0)
		{
			//我对这个问题不是很清楚，准备看一下《C++ Template》看看是否有相关解释
			_pop_heap(first, len, parrent, T(*(first + parrent)), comp);
			parrent--;
		}
	}

	template<class RandomAccessIterator, class Compare>
	void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		make_heap_aux(first, last, distance_type(first), value_type(first), comp);
	}

}


#endif	_SMART_HEAP_H_