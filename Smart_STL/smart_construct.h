/************************************************************************/
/* 构造函数和析构函数的基本工具                                                              */
/************************************************************************/
#ifndef _SMART_CONSTRUCT_H_
#define _SMART_CONSTRUCT_H_

#include <new> //利用其中的placement new
#include "../smart_iterator_base.h"
#include "../smart_type_traits.h"


namespace smart_stl
{
	//默认构造函数版本
	template <class T>
	inline void construct(T * p)
	{
		new (p) T();
	}

	//因为该函数比较短小，而且用到的时候非常多，所以采用inline的方式
	template <class T1, class T2>
	inline void construct(T1 * p, T2 value)
	{
		//这里是对应placement new的应用方法，是在p指针上构建T1类型的数据，value是T2类型的，需要之前的强制转换
		new (p) T1(value);
	}


	//迭代器的第一个版本
	template <class T>
	inline void destroy(T* pointer)
	{
		pointer->~T();
	}

	//迭代器的第二个版本，接受两个参数，该函数的作用是将迭代的value_type*萃取出来
	template <class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last)
	{
		//value_type是为了萃取出它的迭代器实际上是什么样的指针
		_destroy(first, last, value_type(first));
	}

	/*判断元素型别是否函授non-trivial构造函数，因为我们要判断first所指向的元素的类型，但是为了更
	方便的使用，我们应该有通过判断first来判断它所指向的元素的类型的函数*/
	//该模版运行了“参数传导”
	//该函数的作用是将value_type是否含有用户自带的non-trvial-ctor，如果有就调用该alue_type自己的析构函数
	//否则什么都不做，因为含有trivial ctor的数据类型都是原始数据类型
	template <class ForwardIterator, class T>
	inline void _destroy(ForwardIterator first, ForwardIterator last, T* value_type_of_iterator)
	{
		typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
		_aux_destroy(first, last, trivial_destructor());

	}

	//即该迭代器对应的对象是没有默认构造函数的，那么要显示调用destroy函数
	template<class ForwardIterator>
	inline void _aux_destroy(ForwardIterator first, ForwardIterator last, _false_type)
	{
		for (; first != last; first++)
		{
			//我们这样写的原因是因为我们在定义迭代的重载符号时，一般都是利用“*”展现出该迭代器所指的对象
			//*first也就是我们所有的T类型，那么&T对该类型的对象取地址便是destroy函数中的"T*"的参数
			destroy(&*first);
		}
	}

	//如果要destroy的对象是原始数据类型，即是含有trivial-ctor的对象，那么什么也不做
	template<class ForwardIterator>
	inline void _aux_destroy(ForwardIterator first, ForwardIterator last, _true_type)
	{
		//什么都不做，因都是系统自带的trivial-ctor，原始数据，是不用进行析构的
	}

	/*在stl现有的源码中，包含了这个特化函数，但是如果在type_traits中直接添加这两个类型的话，那么就
	不用直接在写这个函数，况且这个函数什么也没做，不明白为什么把他们单独提取出来，可能有历史原因
 	inline void destroy(char*, char*)
 	{
 		//什么也不做，因为在type_traits中我没有包含这种数据
 	}
	*/
	//为char型和wchar_t型设定函数，因为在实现中，调用char或wchar_t型的析构函数情况是非常多的，这样导致
	inline void destroy(char*, char*)
	{
		//什么也不做，专为提高效率
	}
	inline void destroy(wchar_t*, wchar_t*)
	{

	}

}
#endif