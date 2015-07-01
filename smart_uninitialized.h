#ifndef _SMART_UNINITIALIZED_
#define _SMART_UNINITIALIZED_
#include "smart_iterator_base.h"
#include "smart_construct.h"
#include "smart_algorithm.h"
//该头文件函数是应用于container的copy、fill、fill_n的底层
//该函数需要使用迭代器iterator、萃取相关函数value_type、_type_traits、true_type、false_type、is_POD_type等函数
namespace smart_stl
{
	/************************************
	对于uninitialized_copy、uninitialized_fill、uninitialized_fill_n来说，我们主要是观察被fill或者是被copy的区域是否是POD，
	如果是POD，那么他们一定是拥有default ctor/dtor/copy/assignment函数，这样使用起来就是十分方便的，例如“*first = val;”
	这个语句就可以完成assignment，而如果不是POD，就需要调用它自己的构造函数来完成
	*************************************/

	//定义uninitialized_fill_n函数
	//为什么要把first定义成读写类型的迭代器，这是因为我们要对first进行读写，eg：&*first
	//需要注意的是，uninitialized_fill_n是以first为起点，连续填入n个元素，将迭代器指向最后一个被填入的元素的下一个位置
	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
		return _uninitialized_fill_n(first, n, x, value_type(first));
	}

	template<class ForwardIterator, class Size, class T, class T1>
	inline ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
	{
		typedef typename _type_traits<T1>::is_POD_type is_POD;
		return _uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type)
	{
		//调用的是smart_algorithm中的算法
		return fill_n(first, n, x);
	}
	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type)
	{
		ForwardIterator cur = first;
		//SIZE i = 0;这句话如果加上的话会导致多了4bytes的内存，而且没有必要，因为我们只用一次n，所以在循环中
		//可以直接使用n
		，直至0，循环结束
		for(; n > 0; n--, cur++)
		{
			//为什么不直接利用迭代器的&操作符？因为在迭代器定义的时候，我们将*和&这两个操作符重载了，
			//所以不能直接用&操作符，按照迭代器中对“*”重载思想：“*”是用于取出迭代器中的中心元素，那么在用&时就是
			//该重心元素的地址，这样就可以在该地址进行构造
			construct(&*cur, x);
		}
		return cur;
	}


	//定义uninitialized_fill函数
	template<class ForwardIterator, class T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
	{
		_uninitialized_fill(first, last, x, value_type(first));
	}

	template<class ForwardIterator, class T, class T1>
	inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
	{
		typedef typename _type_traits<T1>::is_POD_type is_POD;
		_uninitialized_fill_aux(first, last, x, is_POD());
	}

	template<class ForwardIterator, class T>
	inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _true_type)
	{
		fill(first, last, x);
	}

	template<class ForwardIterator, class T>
	inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _false_type)
	{
		ForwardIterator cur = first;
		for(; cur != last; cur++)
		{
			construct(&*cur, x);
		}
	}

	//定义uninitialized_copy函数,ForwardIterator,允许迭代器在区间上进行读写操作
	//因为要copy一段区间内的数据，这样这个数据就是只读的！“一定要保证所写的函数思路清晰”
	template<class InputIterator, class ForwardIterator>
	//该函数要返回result+last-first
	//只有同一level的函数才能可以重载，否则会导致逻辑不清晰
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
	{
		return _uninitialized_copy(first, last, result, value_type(result));
	}

	template<class InputIterator, class ForwardIterator, class T>
	inline ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
	{
		typedef typename _type_traits<T>::is_POD_type is_POD;
		return _uninitialized_copy_aux(first, last, result, is_POD());
	}

	template<class InputIterator, class ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _true_type)
	{
		return copy(first, last, result);
	}

	template<class InputIterator, class ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _false_type)
	{
		ForwardIterator cur_Forward = result;
		//InputIterator cur_Input = first;
		for (; first!= last; cur_Forward++, first++)
		{
			construct(&*cur_Forward, *first);
		}
		return cur_Forward;
	}

	//为char*,wchar_t*定义_uninitialized_copy偏特化
	inline char* _uninitialized_copy(char* first, char* last, char *result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* _uninitialized_copy(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	
}


#endif