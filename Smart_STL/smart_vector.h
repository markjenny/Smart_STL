#ifndef _SMART_VECTOR_H_
#define _SMART_VECTOR_H_
#include <iostream>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_construct.h"
#include "smart_uninitialized.h"
#include <cstddef> //for ptrdiff_t  and size_t,because you need to "define" the ptrdiff_t
/*****暂时没有包含reverse_iterator相关功能******
author:Mark
date:4/15/2015
************************************************/

namespace smart_stl
{
	//for vector
	/*因为在定义模版的时候，vector和simple_alloc都是在命名空间
	smart_stl中，所以并不用加上smart_stl::命名空间标识符*/
	/*************vector所需要的函数整理****************
	首先，需要记录空间配置的基本量start、finish、end_of_storage，这些我们定义成迭代器类型
	基本函数：
	begin()、end()、size()、capacity()、empty()、oprator[]()、front()、back()、push_back()、
	pop_back()、erase()包括单参数和两个区间参数、resize()、clear()
	*****************************************************/
	template<class T, class Alloc = simple_alloc<T>>
	class vector
	{
	public:
		//为vector定义内嵌类型，五个基本类型
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		//the iterator is only for vector
		typedef T* iterator;
		//const类内嵌类型
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef const T* const_iterator;

		//the vector allocator,why is the allocator defined protected,perhaps other container take the vector as the sequence
	protected:
		//此时在类模板参数中，已经确定了T,所以在后面的data_allocator中可以直接使用
		typedef Alloc data_allocator;

	private:
		iterator start_;
		iterator finish_;
		iterator end_of_storage_;

	public:
		/*****涉及到构造函数、复制构造函数、析构函数*/
		
		/*****
		功能：vector构造函数
		参数：无
		返回值：无
		******/
		vector() : start_(0), finish_(0), end_of_storage_(0){}

		/*****
		功能：vector构造函数
		参数：需要构造的元素个数n
		返回值：无
		******/
		//防止隐式调用构造函数，让接口具有统一性，“使得接口更加容易正确使用”，注：explicit是声明修饰符
		explicit vector(const size_type n);

		/*****
		功能：vector构造函数
		参数：需要构造的元素个数n，需要构造的元素值val
		返回值：无
		******/
		vector(size_type n, const value_type& val);

		/*****
		功能：vector构造函数
		参数：被复制的vector的迭代器first和last
		返回值：无
		******/
		//用于通过现有的数组来构造vector对象
		template<class InputIterator>
		vector(InputIterator first, InputIterator last);

		/*****
		功能：vector复制构造函数
		参数：被复制的vector对象v
		返回值：无
		******/
		vector(const vector& v);

		/*****
		功能：vector的assignment operator
		参数：被进行赋值操作的vector对象
		返回值：无
		******/
		vector& operator=(const vector& v);

		/*****
		功能：vector析构函数
		参数：无
		返回值：无
		******/
		//“如果某函数要调用多个其他的函数，而其他的函数时普通函数，那么该函数没有必要设置成inline类型的”
		//inline函数适合那些简短的，重复性高的函数，所以~vector()个人认为不适合做inline函数
		~vector();

		/*************************************************/


		/****************************************************与比较相关*************************************************************/
		/*****
		功能：判断两个vector对象是否相同
		参数：被用来比较的vector对象v
		返回值：是否相同的bool型结果
		******/
		bool operator==(const vector& v) const;

		/*****
		功能：判断两个vector对象是否不同
		参数：被用来比较的vector对象v
		返回值：是否不同的bool型结果
		******/
		bool operator!=(const vector& v) const;

		/**************************************************************************************************************************/

		/*****与迭代器相关*********
		*****************************/
		iterator begin(){return start_;}
		const_iterator begin()const {return start_;}
		const_iterator cbegin() const {return start_;}
		iterator end() {return finish_;}
		const_iterator end() const {return finish_;}
		//两个不同函数名，其他内容都是一样的，这样的原因？
		const_iterator cend() const{return finish_;}

		/******与容量相关**************
		**************************/
		difference_type size() const {return finish_ - start_;}
		difference_type capacity() const {return end_of_storage_ - start_;}
		bool empty() const {return start_ == finish_;}
		void resize(size_type new_size);
		void resize(size_type new_size, const T& val);

		/*******与访问元素有关***********
		******************************/
		//个人认为reference oprator[]调用begin()，而const_reference operator[]调用cbegin()是为了更好滴理解函数的逻辑
		reference operator[](difference_type i)  {return *(begin() + i);}
		//在访问元素付给某个变量的
		const_reference operator[](difference_type i) const {return *(cbegin() + i);}
		reference front() {return (*begin());}
		reference back() {return (*end());}

		/*******与修改容器相关***********
		******************************/
		//销毁容器中所有的对象，并使得容器的size为0，但是并不回收已有的空间
		void clear();
		//交换两个容器，所以要将其中一个容器变量作为引用传入
		void swap(vector& v);
		void push_back(const value_type& val);
		void pop_back();
		//插入迭代器，在position位置前进行插入，并返回该位置前迭代器
		iterator insert(iterator positon, const value_type& val);
		//第二种迭代器是将position位置前加入n个值为val的迭代器，加入n个迭代器，这个n更方便让我们理解这个n的物理含义
		void insert(iterator positon, const size_type& n, const value_type& val);
		//erase也是有两种函数，一种是只删除特定位置position上的迭代器，第二种是删除
		//erase的时候返回的是position前一个位置的迭代器
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

	private:
		//fill_initialize用于vector的构造函数
		void fill_initialize(size_type n, const value_type& val);
		//用于配置空间并将数据进行填充
		iterator allocate_and_fill(size_type n, const value_type& val);
		//后续要使用到构造和插入的辅助函数insert_aux,vector_aux
		/*****************
		此处待填写
		******************/
		//private类型的相关函数
		void deallocate();

		//区分调用vector(InputIterator first, InputIterator last)的是指针还是整数，然后进行不同的处理
		template<class InputIterator>
		void vector_aux(InputIterator first, InputIterator last, _false_type);

		template<class Integer>
		void vector_aux(Integer n, const value_type& val, _true_type);

		template<class InputIterator>
		void allocate_and_copy(InputIterator first, InputIterator last);



	};
	/**********************************vector相关实现***************************************************/
	//由于模板类的函数声明和实现的其中一种方式是将声明与实现都放在.h文件中，一下便是vector的具体实现

	
	
	/**************************涉及到构造函数、复制构造函数、析构函数**********************************/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(const size_type n)
	{
		fill_initialize(n, T());
	}

	template<class T, class Alloc>
	vector<T, Alloc>::vector(size_type n, const value_type& val)
	{
		fill_initialize(n, val);
	}

	template<class T, class Alloc>
	template<class InputIterator>
	vector<T, Alloc>::vector(InputIterator first, InputIterator last)
	{
		typedef typename is_Integer<InputIterator>:: class_type is_integer;
		vector_aux(first, last, is_integer());
	}

	/*****
	功能：vector复制构造函数
	参数：被复制的vector对象v
	返回值：无
	******/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(const vector& v)
	{
		allocate_and_copy(v.start_, v.finish_);
	}


	template<class T, class Alloc>
	vector<T,Alloc>::~vector()
	{
		//我们在析构一个对象的时候，大致的思路都是这样的，先进行销毁、然后将该快内存块返还给内存池
		smart_stl::destroy(start_, finish_);
		deallocate();
	}


	/****************************************************************************************************/



	template<class T, class Alloc>
	void vector<T, Alloc>::fill_initialize(size_type n, const value_type& val)
	{
		start_ = allocate_and_fill(n, val);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& val)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, val);
		//result = result - n;
		return result;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::deallocate()
	{
		//判断start_是否为null
		if (start_)
		{
			data_allocator::deallocate(start_, end_of_storage_ - start_);//end_of_storage_和end_符合前开后闭的特性
		}
	}


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::vector_aux(InputIterator first, InputIterator last, _false_type)
	{
		allocate_and_copy(first, last);
	}


	template<class T, class Alloc>
	template<class Integer>
	void vector<T, Alloc>::vector_aux(Integer n, const value_type& val, _true_type)
	{
		fill_initialize(n, val);
	}


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::allocate_and_copy(InputIterator first, InputIterator last)
	{
		start_ = data_allocator::allocate(last - first);
		finish_ = uninitialized_copy(first, last, start_);
		end_of_storage_ = finish_;
	}


}

#endif	_SMART_VECTOR_H_
