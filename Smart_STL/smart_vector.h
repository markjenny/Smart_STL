#ifndef _SMART_VECTOR_H_
#define _SMART_VECTOR_H_
#include <iostream>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_construct.h"
#include "smart_uninitialized.h"
#include "smart_algorithm.h"
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
		typedef ptrdiff_t distance_type;
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
		/************************************涉及到构造函数、复制构造函数、析构函数*************************************/
		
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
		~vector();

		/**************************************************************************************************************************/


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

		/*****
		功能：判断两个vector对象是否相同的【友元函数】
		参数：被用来比较的两个vector对象v1和v2
		返回值：是否相同的bool型结果
		******/
		friend bool operator==(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2);


		/*****
		功能：判断两个vector对象是否不同的【友元函数】
		参数：被用来比较的两个vector对象v1和v2
		返回值：是否相同的bool型结果
		******/
		friend bool operator!=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2);
		/**************************************************************************************************************************/

		/*****************************************************与迭代器相关********************************************************/
		iterator begin(){return start_;}
		//const_iterator begin()const {return start_;}
		const_iterator cbegin() const {return start_;}
		iterator end() {return finish_;}
		const_iterator end() const {return finish_;}
		//两个不同函数名，其他内容都是一样的，这样的原因？
		const_iterator cend() const{return finish_;}
		/***************************************************************************************************************************/

		/***********************************************************与容量相关*****************************************************/
		distance_type size() const {return finish_ - start_;}
		distance_type capacity() const {return end_of_storage_ - start_;}
		bool empty() const {return start_ == finish_;}
		void resize(size_type new_size);
		void resize(size_type new_size, const T& val);
		/***************************************************************************************************************************/

		/************************************************************与访问元素有关************************************************/
		//个人认为reference oprator[]调用begin()，而const_reference operator[]调用cbegin()是为了更好滴理解函数的逻辑
		reference operator[](distance_type i)  {return *(begin() + i);}
		//在访问元素付给某个变量的
		const_reference operator[](distance_type i) const {return *(cbegin() + i);}
		reference front() {return (*begin());}
		reference back() {return *(end() - 1);}
		/***************************************************************************************************************************/

		/**********************************************************与修改容器相关**************************************************/
		//销毁容器中所有的对象，并使得容器的size为0，但是并不回收已有的空间
		void clear();
		//交换两个容器，所以要将其中一个容器变量作为引用传入
		void swap(vector& v);
		/*****
		功能：push_back函数
		参数：插入的元素val
		返回值：无
		******/
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
		/***************************************************************************************************************************/

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

		//push_back调用insert_aux，sgi_stl中先判断是否有备用空间，然后insert_aux中又去判断是否有备用空间，觉得没有必要，直接让push_back
		//去调用insert_aux
		void insert_aux(iterator position, const T& val);



	};
	/**********************************【vector相关实现】***************************************************/
	//由于模板类的函数声明和实现的其中一种方式是将声明与实现都放在.h文件中，一下便是vector的具体实现

	
	
	/**************************涉及到构造函数、复制构造函数、析构函数**********************************/
	/*****
	功能：vector构造函数
	参数：无
	返回值：无
	******/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(const size_type n)
	{
		fill_initialize(n, T());
	}

	/*****
	功能：vector构造函数
	参数：需要构造的元素个数n，构造的元素值为val
	返回值：无
	******/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(size_type n, const value_type& val)
	{
		fill_initialize(n, val);
	}

	/*****
	功能：vector构造函数
	参数：被复制的vector的迭代器first和last
	返回值：无
	******/
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

	/*****
	功能：vector的assignment operator
	参数：被进行赋值操作的vector对象
	返回值：无
	******/
	template<class T, class Alloc>
	vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& v)
	{
		//安全检查
		if (*this != v)
			allocate_and_copy(v.start_, v.finish_);
		return *this;
	}


	/*****
	功能：vector析构函数
	参数：无
	返回值：无
	******/
	template<class T, class Alloc>
	vector<T,Alloc>::~vector()
	{
		//我们在析构一个对象的时候，大致的思路都是这样的，先进行销毁、然后将该快内存块返还给内存池
		smart_stl::destroy(start_, finish_);
		deallocate();
	}


	/***************************************************************************************************************************/




	/****************************************************与比较相关*************************************************************/
	/*****
	功能：判断两个vector对象是否相同
	参数：被用来比较的vector对象v
	返回值：是否相同的bool型结果
	******/
	template<class T, class Alloc>
	bool vector<T, Alloc>::operator==(const vector& v) const
	{
		if ( (finish_ -  start_ ) != v.size() )
			return false;
		else
		{
			iterator ptr_temp1 = start_;
			iterator ptr_temp2 = v.start_;

			for(; ptr_temp1 != finish_; ptr_temp1++, ptr_temp2++)
				if (*ptr_temp1 != *ptr_temp2)
					return false;

			return true;
		}
	}

	/*****
	功能：判断两个vector对象是否不同
	参数：被用来比较的vector对象v
	返回值：是否不同的bool型结果
	******/
	template<class T, class Alloc>
	bool vector<T, Alloc>::operator!=(const vector& v) const
	{
		return !(*this == v);
	}

		/*****
		功能：判断两个vector对象是否相同的【友元函数】
		参数：被用来比较的两个vector对象v1和v2
		返回值：是否相同的bool型结果
		******/
		//friend是声明修饰符，所以在实现的时候就不必添加了
		template<class T, class Alloc>
		bool operator==(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2)
		{
			return v1.operator =(v2);
		}


		/*****
		功能：判断两个vector对象是否不同的【友元函数】
		参数：被用来比较的两个vector对象v1和v2
		返回值：是否相同的bool型结果
		******/
		//friend是声明修饰符，所以在实现的时候就不必添加了
		template<class T, class Alloc>
		bool operator!=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2)
		{
			return !(v1==v2);
		}
	/********************************************************************************************************************************/




	/***********************************************************与容量相关*****************************************************/
	/*****
	功能：resize函数，重新定义尺寸
	参数：所要定义的新的size
	返回值：无
	******/
	template<class T, class Alloc>
	void vector<T, Alloc>::resize(size_type new_size)
	{
		resize(size_type, T());
	}

	/*****
	功能：resize函数，重新定义尺寸
	参数：所要定义的新的size，
	返回值：无
	******/
	//resize的用法，当newsize比原来的size小的话，那么就只保留钱new_size个，如果，newsize比size大的话，
	//就将newsize比size多的那些加在vector的尾部
	template<class T, class Alloc>
	void vector<T, Alloc>::resize(size_type new_size, const T& val)
	{
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), val);
	}
	/********************************************************************************************************************************/



	/**********************************************************与修改容器相关**************************************************/
	template<class T, class Alloc>
	void vector<T, Alloc>::clear()
	{
		//待整理。。。。
	}


	template<class T, class Alloc>
	void vector<T, Alloc>::swap(vector& v)
	{
		//待整理。。。。
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::push_back(const value_type& val)
	{
		insert_aux(finish_, val);
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::pop_back()
	{
		//待整理。。。。

	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator positon, const value_type& val)
	{
		//待整理。。。。
		return start_;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::insert(iterator positon, const size_type& n, const value_type& val)
	{
		//待整理。。。。
		return start_;
	}
	

	/*****
	功能：删除容器内特定
	参数：所要删除的位置position
	返回值：下一个元素对应的迭代器
	******/
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
	{
		return erase(position, position+1);
	}

	/*****
	功能：删除容器内特定范围内的元素
	参数：所要删除的范围起始first和终止last
	返回值：last位置处的元素的迭代器
	******/
	//erase其实就是将没有被删除的向前移动
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{
		//因为vector的iterator是原生指针，所以可以满足+=n和-=n的用法
		//如果last与end()相同，说明要删除的都是”后半部分“，我们就不要再调用copy了，因为copy本来就含有模板，尽量减少编译后代码的膨胀性
		if(last != end())
			copy(last, finish_, first);
		//只进行析构对象，但是不释放空间
		for(distance_type n = last - first; n > 0 ; n--)
		{
			--finish_;
			destroy(finish_);
		}
		return first;
	}
	/***************************************************************************************************************************/




	/********************************************private函数**************************************************************************/
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

	template<class T, class Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T& val)
	{
		//判断是否有备用空间
		if (finish_ != end_of_storage_)
		{
			//如果有空间的情况下扎到队尾应该特殊考虑，stl源码剖析中finish_-2当position等于finish_的时候，使得区间错误，区间左大右小。
			if(position == finish_)
			{
				*position = val;
				finish_++;
				return;
			}
			//因为只插入一个元素，所以直接用construct没什么影响
			construct(finish_, *(finish_ - 1));
			++finish_;
			//没有必要使用中间变量
			//T val_copy = val;
			//解释一下为什么非要将position~finish_-2复制到finish_-1，而不是position~finish_-3复制到finish_-2还能少复制一个元素，
			//这是因为我们并不知道区间有多长，如果capacity只有2，那么finish_-3显然是错误的，处处要考虑边界！
			copy_backward(position, finish_ - 2, finish_ - 1);

			*position = val;
		}
		else
		{
			const size_type old_size = size();
			//这个地方也改一下，STL源码剖析中说到当old_size为0的时候，将new_size设为1，但是这样使得
			//当old_size小的非常泥泞，每次都要创建新空间，转移元素，释放久空间
			const size_type new_size = old_size == 0 ? 1 : 2 * old_size;


			iterator new_start_ = data_allocator::allocate(new_size);
			iterator new_finish_ = new_start_;

			new_finish_ = uninitialized_copy(start_, position, new_start_);

			//在position的位置上添加新元素（因为要在position的位置上插入，所以原position位置上的元素向后移一位）;
			construct(new_finish_, val);
			//调整水位
			new_finish_++;

			new_finish_ = uninitialized_copy(position, finish_, new_finish_);

			//析构原区间上的元素，并释放空间
			destroy(start_, finish_);
			deallocate();

			start_ = new_start_;
			finish_ = new_finish_;
			end_of_storage_ = start_ + new_size;
		}
	}
	/**************************************************************************************************************************/


}

#endif	_SMART_VECTOR_H_
