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


	// copy--无所不用其极！

	// All of these auxiliary functions serve two purposes.  (1) Replace
	// calls to copy with memmove whenever possible.  (Memmove, not memcpy,
	// because the input and output ranges are permitted to overlap.)
	// (2) If we're using random access iterators, then write the loop as
	// a for loop with an explicit count.
	/*******************************
	//sgi_stl的copy是允许copy的区间重叠，但是它并不组织重叠的区间进行copy产生错误的情况，【尽可能在时间和性能之间取一个折中，因为你不知道使用时出于时间还是性能上的考虑】
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

	/*****
	功能：初始copy泛化函数
	参数：所要copy的起始地址first，所要copy的终止地址last，copy的起始地址result
	返回值：无
	******/
	template<class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return _copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	/*****
	功能：初始copy的char*版本函数
	参数：所要copy的起始地址first，所要copy的终止地址last，copy的起始地址result
	返回值：无
	******/
	inline char* copy(char* first, char* last, char* result)
	{
		memmove(result, first, last - first);
		//注：指针的size和类型的size不是一边大的
		return result + (last - first);
	}

	/*****
	功能：初始copy的wchar_t*版本函数
	参数：所要copy的起始地址first，所要copy的终止地址last，copy的起始地址result
	返回值：无
	******/
	inline wchar_t* copy(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result, first,  sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	/*****
	功能：_copy_dispatch仿函数
	参数：所要copy的起始地址first，所要copy的终止地址last，copy的起始地址result
	返回值：无
	******/
	template<class InputIterator, class OutputIterator>
	struct _copy_dispatch
	{
		OutputIterator operator() (InputIterator first, InputIterator last, OutputIterator result)
		{
			return _copy(first, last, result, iterator_category(first));
		}
	};

	/*****
	功能：_copy_dispatch偏特化仿函数
	参数：所要copy的起始地址first，所要copy的终止地址last，copy的起始地址result
	返回值：无
	******/
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

	/*****
	功能：_copy_dispatch偏特化仿函数
	参数：所要copy的起始地址first，所要copy的终止地址last，copy的起始地址result
	返回值：无
	******/
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
	返回值：result
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

	/*****
	功能：适用于RandomAccessIterator的copy函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy到的起始位置result
	返回值：result
	******/
	template<class RandomAccessIterator, class OutputIterator, class Distance>
	inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; n--, result++, first++)
			*result = *first;
		return result;
	}

	/*****
	功能：原生指针没有trival assignment operator函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy到的起始位置result
	返回值：result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _true_type)
	{
		//注：sizeof是操作符，不是函数，故没有包含它的头文件
		memmove(result, first, (last - first)*sizeof(T));
		return result + (last - first); 
	}

	/*****
	功能：原生指针含有trival assignment operator函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy到的起始位置result
	返回值：result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _false_type)
	{
		return _copy_d(first, last, result, static_cast<ptrdiff_t*>0);
	}
	


	//我明白为什么在copy算法中没有考虑first <= result <= last中可能会产生覆盖的问题， 那是因为algorithm中还定义了向后
	//拷贝函数copy_backward，算法都是相同的，只是方向改变了
	/*****
	功能：初始copy_backward泛化函数
	参数：所要copy_backward的起始地址first，所要copy_backward的终止地址last，copy_backward的起始地址result
	返回值：无
	******/
	template<class BidirectionalInterator1, class BidirectionalInterator2>
	inline BidirectionalInterator2 copy_backward(BidirectionalInterator1 first, BidirectionalInterator1 last, BidirectionalInterator2 result)
	{
		return _copy_dispatch_backward<BidirectionalInterator1, BidirectionalInterator2>()(first, last, result);
	}

	/*****
	功能：初始copy_backward的char*版本函数
	参数：所要copy_backward的起始地址first，所要copy_backward的终止地址last，copy_backward的起始地址result
	返回值：无
	******/
	inline char* copy_backward(char* first, char* last, char* result)
	{
		memmove(result - (last - first), first, last - first);
		return result - (last - first);
	}

	/*****
	功能：初始copy_backward的wchar_t*版本函数
	参数：所要copy_backward的起始地址first，所要copy_backward的终止地址last，copy_backward的起始地址result
	返回值：无
	******/
	inline wchar_t* copy_backward(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result - (last - first), first, sizeof(wchar_t) * (last - first) );
		return result - (last - first);
	}

	/*****
	功能：_copy_dispatch_backward仿函数
	参数：所要copy_backward的起始地址first，所要copy_backward的终止地址last，copy_backward的起始地址result
	返回值：无
	******/
	template<class BidirectionalInterator1, class BidirectionalInterator2>
	struct _copy_dispatch_backward
	{
		BidirectionalInterator2 operator() (BidirectionalInterator1 first, BidirectionalInterator1 last, BidirectionalInterator2 result)
		{
			return _copy_backward(first, last, result, iterator_category(first));
		}
	};

	/*****
	功能：_copy_dispatch_backward偏特化仿函数
	参数：所要copy_backward的起始地址first，所要copy_backward的终止地址last，copy_backward的起始地址result
	返回值：无
	******/
	template<class T>
	struct _copy_dispatch_backward<T*, T*>
	{
		//此处有问题，所有的结构都要改，模板函数无法偏特化是要用仿函数
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			return _copy_htao_backward(first, last, result, htao());

		}
	};

	/*****
	功能：_copy_dispatch_backward偏特化仿函数
	参数：所要copy_backward的起始地址first，所要copy_backward的终止地址last，copy_backward的起始地址result
	返回值：无
	******/
	template<class T>
	struct _copy_dispatch_backward<const T*, T*>
	{
		T* operator()(const T* first, const T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			_copy_htao_backward(first, last, result, htao());
		}
	};

	/*****
	功能：针对不同迭代器类型(本版本为InputIterator)的copy_backward功能函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy_backward到的起始位置result
	返回值：result
	******/
	template<class BidirectionalInterator1, class BidirectionalInterator2>
	inline BidirectionalInterator2 _copy_backward(BidirectionalInterator1 first, BidirectionalInterator1 last, BidirectionalInterator2 result, bidirectional_iterator_tag)
	{
		//因为InputIterator只能执行向前++的操作，所以我们判断边界只能是通过first！=last来判断，这样造成的问题就是每次都是要使用比较操作，造成速率较慢
		for(; first != last; --last, --result)
			*result = *first;
		return result;
	}

	/*****
	功能：针对不同迭代器类型(本版本为RandomAccessIterator)的copy_backward功能函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy_backward到的起始位置result
	返回值：result+(last-first)
	******/
	template<class RandomAccessIterator, class BidirectionalInterator>
	inline BidirectionalInterator _copy_backward(RandomAccessIterator first, RandomAccessIterator last, BidirectionalInterator result, random_access_iterator_tag)
	{
		/*RandomAccessIterator具有+=n和-=n的功能，即可以向前走n步或者向后走n不，这样我们就可以不用去比较迭代器而直接利用last-first的值来作为
		循环次数因为_copy_dispatch中含有“原生指针”偏特化的版本，而原生指针也是RandomAccessIterator，所以同时可以调用这个函数，所以为了
		不增加代码的膨胀我们可以将这个功能划分成一个函数*/
		_copy_d_backward(first, last, result, distance_type(first));
	}

	/*****
	功能：适用于RandomAccessIterator的copy_backward函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy_backward到的起始位置result
	返回值：result
	******/
	template<class RandomAccessIterator, class BidirectionalInterator, class Distance>
	inline BidirectionalInterator _copy_d_backward(RandomAccessIterator first, RandomAccessIterator last, BidirectionalInterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; n--, --result, --first)
			*result = *first;
		return result;
	}

	/*****
	功能：原生指针有trival assignment operator函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy_backward到的起始位置result
	返回值：result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao_backward(const T* first, const T* last, T* result, _true_type)
	{
		//注：sizeof是操作符，不是函数，故没有包含它的头文件
		memmove(result - (last - first), first, (last - first)*sizeof(T));
		return result - (last - first); 
	}

	/*****
	功能：原生指针没有trival assignment operator函数
	参数：被复制的区间起始位置first，被复制区间的终止位置（不包括）last，所要copy_backward到的起始位置result
	返回值：result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao_backward(const T* first, const T* last, T* result, _false_type)
	{
		return _copy_d_backward(first, last, result, static_cast<ptrdiff_t*>0);
	}

}
#endif _SMART_ALGORUTHM_H_