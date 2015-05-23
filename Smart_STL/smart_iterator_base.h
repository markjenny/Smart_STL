/********************
该头文件的作用是迭代器的基本功能，不包括insert_back_iterator/insert_front_iterator等功能以及逆向迭代器
*********************/
#ifndef _SMART_ITERATOR_BASE_H_
#define _SMART_ITERATOR_BASE_H_
#include <cstddef>	//for ptrdiff_t

namespace smart_stl
{
	struct input_iterator_tag 
	{

	};

	struct output_iterator_tag 
	{

	};

	/*这种继承的作用即是为了在编译的时候就可以选择重载函数的版本，节省效率；还有一点就是当使用了
	继承时，对于并没有这种类型的函数时，但是拥有继承关系后可以调用父亲类型的重载函数，自然而然地完成
	“传递调用”*/
	struct forward_iterator_tag : public input_iterator_tag 
	{

	};

	struct bidirectional_iterator_tag : public forward_iterator_tag
	{

	};

	struct random_access_iterator_tag : public bidirectional_iterator_tag
	{

	};

	template<class _Tp, class _Distance>
	struct input_iterator
	{
		typedef input_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	//sgi_stl中对于output_iterator是将所有类型类型设置成了void，是为了避免通过output_iterator来读取元素的值？
	//write only是只能改变数据，那么我们就不需要他的其他情况，只是改变就好了，但是如果不知道类型的强行转换
	//不是会发生数据截断吗？
	template<class _Tp, class _Distance>
	struct output_iterator
	{
		typedef output_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<class _Tp, class _Distance>
	struct forward_iterator
	{
		typedef forward_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<class _Tp, class _Distance>
	struct bidirectional_iterator
	{
		typedef bidirectional_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<class _Tp, class _Distance>
	struct random_access_iterator
	{
		typedef random_access_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};


	//之所以构建这个迭代器是因为在自己写迭代器的时候很有可能没有写全这五个内嵌相应型别，可能无法与Smart_stl组件
	//顺利搭配，所以当新设计的迭代器继承这个基础迭代器就可以保证符合Smart_STL使用规范
	template<class _Category,
					 class _Tp,
					 class _Distance = ptrdiff_t,
					 class _Pointer = _Tp*,
					 class _Reference = _Tp&>
	 struct iterator
	 {
		 typedef _Category iterator_category;
		 typedef _Tp value_type;
		 typedef ptrdiff_t difference_type;
		 typedef _Pointer pointer;
		 typedef _Reference reference;
	 };

	 //构建迭代器萃取器
	 template<class _Iterator>
	 struct iterator_traits
	 {
		 //因为编译器不知道_Iterator中的iterator_category是类型还是成员，所以通过typename来告知编译器
		 typedef typename _Iterator::iterator_category iterator_category;
		 typedef typename _Iterator::value_type value_type;
		 typedef typename _Iterator::difference_type difference_type;
		 typedef typename _Iterator::pointer pointer;
		 typedef typename _Iterator::reference reference;
	 };
	 //偏特化，针对“原生指针”
	 template<class _Tp>
	 struct iterator_traits<_Tp*>
	 {
		 typedef random_access_iterator_tag iterator_category;
		 typedef _Tp value_type;
		 typedef ptrdiff_t difference_type;
		 typedef _Tp* pointer;
		 typedef _Tp& reference;
	 };

	 //偏特化的”原生指针“一般都分为T*和const T*两种
	 template<class _Tp>
	 struct iterator_traits<const _Tp*>
	 {
		 typedef random_access_iterator_tag iterator_category;
		 typedef _Tp value_type;
		 typedef ptrdiff_t difference_type;
		 typedef _Tp* pointer;
		 typedef _Tp& reference;
	 };

	 /*接下来应该是指定函数，可以萃取出迭代器的各项成员类型，主要
	 是用在其他重载函数的类型上，进而完成重载函数的正确选择
	 因为对于迭代器来说有五中内容，但是由于其中的pointer和reference是与
	 value_type相关的所以只需求出三种即可*/

	 //由于该函数较小，所以直接利用inline获取更高的效率
	 template<class Iterator>
	 inline typename iterator_traits<Iterator>::value_type*
		 value_type(const Iterator& )
	 {
		 //在C++的标准中显示强制转换就是利用static_cast
		 return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	 }
	 
	 template<class Iterator>
	 inline typename iterator_traits<Iterator>::iterator_category
		 iterator_category(const Iterator&)
	 {
		 typedef typename iterator_traits<Iterator>::iterator_category catogory;
		 return catogory();
	 }

	 template<class Iterator>
	 inline typename iterator_traits<Iterator>::difference_type*
		 difference_type(const Iterator&)
	 {
		 return static_cast<typename iterator_traits<Iterator>::difference_type*>(0)
	 }

}
#endif
