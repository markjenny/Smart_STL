#ifndef _SMART_ALGORUTHM_H_
#define _SMART_ALGORUTHM_H_
#include <string.h>
#include <cstddef>			//for ptrdiff_t
#include "smart_iterator_base.h"


namespace smart_stl
{
	/*****
	功能：fill_n算法
	参数：所要fill的起始地址first，所要fill的个数n，fill的元素值val
	返回值：无
	******/
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& val)
	{
		for (; n > 0; first++, n-- )
			*first = val;
		return first;
	}

	/*****
	功能：copy算法
	参数：所要fill的起始地址first，所要fill的个数n，fill的元素值val
	返回值：无
	******/
	/*******************************
	这里需要说一下，原生指针并不是非要指向内置类型的指针，可以是int、float、double等，同时也可以是自己定义的一个类class，所谓原生指针
	就是真的是指向那种类型的指针，而不是行为上像指针但是却不是指针的
                                |-------_copy_dispatch-----|--------_copy_dispatch泛型：此时已经说明first和last不是原生指针了，那么就是迭代器类型的了，那么我们就可以
																						考虑它是否是InputIterator还是RandomIterator了，这里注意如果不是RandomIterator，那么剩下的就都是InputIterator，因为
																						其他迭代器的类型都是继承InputIterator，省去调转函数，思路也更加清晰
								|				                           |--------偏特化 原生指针_copy_dispatch(T*,T*) : 这个时候就可以考虑has_trivial_assignment_operator的真假
								|                                          |--------偏特化 原生指针_copy_dispatch(const T* ,const T*) ： 这个时候可以考虑has_trivial_assignment_operator的真假
	copy功能分布---- |-------copy(char*,char*)
                                |
                                |
                                |-------copy(wchar_t*, wchar_t*)
	********************************/
	//另外需要说一点的是，在很短小的函数可以采用inline函数，包括仅含调转功能的函数，因为调转函数最大的问题就是增加栈的深度并且不断的去更改寄存器的CS：IP指针
	//造成指针一直还变换
	template<class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return _copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	inline char* copy(char* first, char* last, char* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* copy(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	template<class InputIterator, class OutputIterator>
	struct _copy_dispatch
	{
		OutputIterator operator() (InputIterator first, InputIterator last, OutputIterator result)
		{
			return _copy(first, last, result, iterator_category(first));
		}
	};

	template<class T>
	struct _copy_dispatch<T*, T*>
	{
		//此处有问题，所有的结构都要改，模板函数无法偏特化是要用仿函数
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			return _copy_htao(first, last, result, htao());

		}
	};

	template<class T>
	struct _copy_dispatch<const T*, T*>
	{
		T* operator()(const T* first, const T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			_copy_htao(first, last, result, htao());
		}
	};

	/*****
	功能：针对不同迭代器类型(本版本为InputIterator)的copy功能函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy到的起始位置result
	返回值：result+(last-first)
	******/
	template<class InputIterator, class OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//因为InputIterator只能执行向前++的操作，所以我们判断边界只能是通过first！=last来判断，这样造成的问题就是每次都是要使用比较操作，造成速率较慢
		for(; first != last; first++, result++)
			*result = *first;
		return result;
	}

	/*****
	功能：针对不同迭代器类型(本版本为RandomAccessIterator)的copy功能函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy到的起始位置result
	返回值：result+(last-first)
	******/
	template<class RandomAccessIterator, class OutputIterator>
	inline OutputIterator _copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
	{
		/*RandomAccessIterator具有+=n和-=n的功能，即可以向前走n步或者向后走n不，这样我们就可以不用去比较迭代器而直接利用last-first的值来作为
		循环次数因为_copy_dispatch中含有“原生指针”偏特化的版本，而原生指针也是RandomAccessIterator，所以同时可以调用这个函数，所以为了
		不增加代码的膨胀我们可以将这个功能划分成一个函数*/
		_copy_d(first, last, result, distance_type(first));
	}

	template<class RandomAccessIterator, class OutputIterator, class Distance>
	inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; n--, result++, first++)
			*result = *first;
		return result;
	}

	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _true_type)
	{
		//注：sizeof是操作符，不是函数，故没有包含它的头文件
		memmove(result, first, (last - first)*sizeof(T));
		return result + (last - first); 
	}

	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _false_type)
	{
		return _copy_d(first, last, result, static_cast<ptrdiff_t*>0);
	}
	

}
#endif _SMART_ALGORUTHM_H_