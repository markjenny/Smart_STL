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

		/*****
		功能：重新适配容量大小
		参数：重新分配的容量n
		返回值：无
		******/
		void reserve(size_type n);

		/*****
		功能：C++11新增加的功能：删除未使用的容量
		参数：无
		返回值：无
		******/
		void shrink_to_fit();

		/***************************************************************************************************************************/

		/************************************************************与访问元素有关************************************************/
		//个人认为reference oprator[]调用begin()，而const_reference operator[]调用cbegin()是为了更好滴理解函数的逻辑
		reference operator[](distance_type i)  {return *(begin() + i);}
		//在访问元素付给某个变量的
		const_reference operator[](distance_type i) const {return *(cbegin() + i);}
		reference front() {return (*begin());}
		reference back() {return *(end() - 1);}

		/*****
		功能：C++11新增加的功能：返回容器指针
		参数：无
		返回值：pointer
		******/
		pointer data() {return start_;}
		/***************************************************************************************************************************/

		/**********************************************************与修改容器相关**************************************************/
		/*****
		功能：clear函数，析构容器中的所有元素，但是不释放已分配的空间
		参数：无
		返回值：无
		******/
		void clear();

		/*****
		功能：交换两个容器的内容
		参数：无
		返回值：无
		******/
		void swap(vector& v);
		/*****
		功能：push_back函数
		参数：插入的元素val
		返回值：无
		******/
		void push_back(const value_type& val);

		/*****
		功能：弹出最后一个元素
		参数：插入的元素val
		返回值：无
		******/
		void pop_back();
		/*****
		功能：在特定position位置上加入val元素
		参数：所要插入的位置position，插入的元素val
		返回值：插入position位置元素对应的迭代器
		******/
		//插入迭代器，在position位置前进行插入，并返回该位置前迭代器
		iterator insert(iterator positon, const value_type& val);

		/*****
		功能：在特定position位置上加入n个val元素
		参数：所要插入的位置position，插入的元素val，所要插入的元素个数n
		返回值：插入position位置元素对应的迭代器
		******/
		//第二种迭代器是将position位置前加入n个值为val的迭代器，加入n个迭代器，这个n更方便让我们理解这个n的物理含义
		void insert(iterator positon, size_type n, const value_type& val);

		/*****
		功能：在特定position位置上插入[first,last）上的元素
		参数：所要插入的位置position，所要插入的范围
		返回值：插入position位置元素对应的迭代器
		******/
		//第三种迭代器是将一个迭代器表示范围的元素插入到该迭代器中
		template<class InputIterator>
		void insert(iterator position, InputIterator first, InputIterator last);
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
		//insert_aux的另外两个重载函数，分别应用于insert(position,first,last)和insert(position, n, val)
		template<class InputIterator>
		void insert_aux(iterator position, InputIterator first, InputIterator last, _false_type);

		template<class Integer>
		void insert_aux(iterator position, Integer n, const value_type& val, _true_type);



	};
	/**********************************【vector相关实现】***************************************************/
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

	template<class T, class Alloc>
	vector<T, Alloc>::vector(const vector& v)
	{
		allocate_and_copy(v.start_, v.finish_);
	}

	template<class T, class Alloc>
	vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& v)
	{
		//安全检查
		if (*this != v)
			allocate_and_copy(v.start_, v.finish_);
		return *this;
	}


	template<class T, class Alloc>
	vector<T,Alloc>::~vector()
	{
		//我们在析构一个对象的时候，大致的思路都是这样的，先进行销毁、然后将该快内存块返还给内存池
		smart_stl::destroy(start_, finish_);
		deallocate();
	}


	/***************************************************************************************************************************/




	/****************************************************与比较相关*************************************************************/
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

	template<class T, class Alloc>
	bool vector<T, Alloc>::operator!=(const vector& v) const
	{
		return !(*this == v);
	}

	template<class T, class Alloc>
	bool operator==(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2)
	{
		return v1.operator =(v2);
	}


	template<class T, class Alloc>
	bool operator!=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2)
	{
		return !(v1==v2);
	}
	/********************************************************************************************************************************/




	/***********************************************************与容量相关*****************************************************/
	template<class T, class Alloc>
	void vector<T, Alloc>::resize(size_type new_size)
	{
		resize(size_type, T());
	}

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

	template<class T, class Alloc>
	void vector<T, Alloc>::reserve(size_type n)
	{
		if( n <= capacity() )
			return;

		pointer new_start_ = data_allocator::allocate(n);
		pointer new_finish_ = uninitialized_copy(start_, finish_, new_start_);

		destroy(start_, , finish_);
		deallocate();

		start_ = new_start_;
		finish_ = new_finish_;
		end_of_storage_ = start_ + n;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::shrink_to_fit()
	{
		//end_of_storage本来就应该指导容器外,对于二级适配器，end_of_storage-finish_如果小于128bytes那么也一定能放在规定的内存池中，
		//因为各个链表都是8bytes的倍数，使得链表储存以8bytes为内存块显得更加有道理
		data_allocator::deallocate(finish_, (end_of_storage_ - finish_));
		end_of_storage_ = finish_;
	}
	/********************************************************************************************************************************/



	/**********************************************************与修改容器相关**************************************************/
	template<class T, class Alloc>
	void vector<T, Alloc>::clear()
	{
		//需要注意的是，我们将容器内的元素进行clear之后，只是析构了，并没有释放
		erase(start_, end());
	}


	template<class T, class Alloc>
	void vector<T, Alloc>::swap(vector& v)
	{
		if(*this != v)
		{
			swap(start_, v.start_);
			swap(finish_, v.finish_);
			swap(end_of_storage_, v.end_of_storage_);
		}
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::push_back(const value_type& val)
	{
		insert_aux(finish_, val);
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::pop_back()
	{
		--finish_;
		destroy(finish_);
	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type& val)
	{
		insert_aux(position, val);
		return position;
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
	{
		insert_aux(position, first, last, typename is_Integer<InputIterator>::class_type());
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
	{
		insert_aux(position, n, val, typename is_Integer<size_type>::class_type());
	}
	

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
	{
		return erase(position, position+1);
	}

	//erase其实就是将没有被删除的向前移动
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{
		//因为vector的iterator是原生指针，所以可以满足+=n和-=n的用法
		//如果last与end()相同，说明要删除的都是”后半部分“，我们就不要再调用copy了，因为copy本来就含有模板，尽量减少编译后代码的膨胀性
		if(last != end())	
			//copy(last, finish_, first);
			uninitialized_copy(last, finish_, first);
		//只进行析构对象，但是不释放空间
// 		for(distance_type n = last - first; n > 0 ; n--)
// 		{
// 			--finish_;
// 			destroy(finish_);
// 		}
		destroy(finish_ - (last - first), finish_)
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
			const size_type new_size = old_size == 0 ? 8 : 2 * old_size;


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


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::insert_aux(iterator position, InputIterator first, InputIterator last, _false_type)
	{
		size_type space_left = end_of_storage_ - finish_;
		size_type space_need = last - first;

		if (space_left >= space_need)
		{
			copy_backward(position, finish_, finish_ + space_need );

			uninitialized_copy(first, last, position);

			finish_+=(last - first);
		}
		else//备用空间不够
		{
			size_type old_size = size();
			size_type new_size = ( 0 == size() ) ? 8 : 2 * old_size;

			iterator new_start_ = data_allocator::allocate(new_size);
			iterator new_finish_ = new_start_;

			new_finish_ = uninitialized_copy(start_, position, new_start_);
			new_finish_ = uninitialized_copy(first, last, new_finish_);
			new_finish_ = uninitialized_copy(position, finish_, new_finish_);

			destroy(start_, finish_);
			deallocate();

			start_ = new_start_;
			finish_ = new_finish_;
			end_of_storage_ = new_start_ + new_size;
		}
	}

	template<class T, class Alloc>
	template<class Integer>
	void vector<T, Alloc>::insert_aux(iterator position, Integer n, const value_type& val, _true_type)
	{
		//首先计算所剩容量是否可以放下所要insert的那些数据
		size_type space_left = end_of_storage_ - finish_;
		size_type space_need = n;

		if (space_left >= space_need)
		{
			copy_backward(position, finish_, finish_ + space_need);
			uninitialized_fill_n(position, n, val);

			finish_ += space_need;
		}
		else
		{
			size_type old_size = size();
			size_type new_size = ( 0 == old_size ) ? 8 : 2 * old_size;

			iterator new_start_ = data_allocator::allocate(n);
			iterator new_finish_ = new_start_;

			new_finish_ = uninitialized_copy(start_, position, new_start_);
			new_finish_ = uninitialized_fill_n(new_finish_, n, val);
			new_finish_ = uninitialized_copy(position, finish_, new_finish_);

			destroy(start_, finish_);
			deallocate();

			start_ = new_start_;
			finish_ = new_finish_;
			end_of_storage_ = new_start_ + new_size;
		}
	}
	/**************************************************************************************************************************/


}

#endif	_SMART_VECTOR_H_
