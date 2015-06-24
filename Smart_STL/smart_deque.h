#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"
#include "smart_uninitialized.h" 
#include "smart_construct.h"
#include <cstddef>
#include <stdexcept>
#include <algorithm>

namespace smart_stl
{
	//我们想要理解deque出现的意义，deque的表面的使用方法就像“vector”一样，但是它是由一段段连续的空间组成的，表面上与
	//vector相同，但是它在“头尾”的存取效率更高，如果关注头尾部分的话，可以使用deque
	//deque中的迭代器是比较特殊的迭代器，因为要考虑到从一个段跳到上一个段或者是下一个段
	inline size_t deque_buf_size(size_t sz)
	{
		return sz > 512 ? size_t(1) : size_t(512 / sz) ;
	}

	//用户应该不用去管每个bucket中有多少个元素，这样是没有意义的，这是相当于内部结构，用户在使用的时候也没有直接去
	//设定每个bucket中有多少个元素
	//时刻记住deque拥有一个中控区，中控区上的每个点指向缓冲区，缓冲区中默认是有512bytes的空间，如果相关的类型是比512bytes大
	//的话，缓冲区中就只存储这种类型的一个数据

	//总结一下deque迭代器的特点，该迭代器的finish中是可以含有元素的，在缓冲区中用first和last进行标记，【finish迭代器】的cur
	//指向的是该缓冲区中【没有元素的第一个点】，【start迭代器】的cur指向的是该缓冲区中【第一个元素】
	template<class T, class Ref, class Ptr>
	struct deque_iterator
	{
		//偏特化
		//下面这个iterator应该是留给deque的数据结构用的
		//没搞懂sgi_stl中这个有什么用？？？
		typedef deque_iterator<T, T&, T*> iterator;
		typedef deque_iterator<T, const T&, const T*> const_iterator;

		static size_t buffer_size() {return deque_buf_size(sizeof(value_type));}

		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef ptrdiff_t distance_type;

		//所需要的其他定义
		typedef size_t size_type;
		typedef T** map_pointer;		//deque的主控被称为map
		//尽量来维持deque_iterator的“原始感”？
		typedef deque_iterator self;

		//deque_iterator需要的相关成员
		T* cur;		//当前迭代器所指的缓冲区的当前点
		T* first;		//当前迭代器所指的缓冲区的起始点
		T* last;		//当前迭代器所指的缓冲区的终止点（不包括）
		map_pointer node;		//中控区map中的node点

		/*****************************************各种相关的构造函数、复制构造函数、assignment operator***********************/
		deque_iterator() : cur(0), first(0), last(0), node(0) {}

		deque_iterator(pointer x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) {}

		deque_iterator(const self& deq_iter) : cur(deq_iter.cur), first(deq_iter.first), last(deq_iter.last), node(deq_iter.node) {}

		self& operator = (const self& deq_iter) 
		{
			cur = deq_iter.cur;
			first = deq_iter.first;
			last = deq_iter.last;
			node = deq_iter.node;
			return *this;
		}
		/*************************************************************************************************************************/

		/****************************************迭代器中所需要的各种操作符********************************************************/
		//迭代器中所需要的各种逻辑比较符
		//回顾一下list中的迭代器，它是只有operator==和operator！=，这是和它的具体结构有关，而deque的表面现象是与“vector”类似，
		//所以有一些新的>，<，<+等操作
		bool operator == (const self& deq_iter) {return cur == deq_iter.cur;}
		bool operator != (const self& deq_iter) 
		{
			//tiny_stl受到了源码中operator<的影响，这里只比较deq_iter就可以，因为缓冲区中的每个点的cur都是不同的，即使
			//是不同的map_pointer，因为都是从内存空间中分配出去的
			return !(*this == deq_iter);
		}
		bool operator > (const self& deq_iter)
		{
			return  node == deq_iter.node ? cur > deq_iter.cur : node > deq_iter.node ; 
		}

		bool operator <= (const self& deq_iter)
		{
			return !( *this > deq_iter );
		}

		bool operator < (const self& deq_iter)
		{
			return !(deq_iter > *this);
		}

		bool operator >= (const self& deq_iter)
		{
			return !(*this < deq_iter);
		}
		/*************************************************************************************************************************/

		/***********************************************相关的重载操作符*********************************************************/
		distance_type operator - ( const self& deq_iter )
		{
			//这里需要说明的一点就是我们求得是*this和deq_iter之间的距离，而不是求*this和deq_iter之间有多少给数据，如果是求
			//二者之间有多少个数据，那么应该在(cur-first)之间加上1，但是求得是距离，所以这里就直接写成(cur-first)
			return (distance_type(buffer_size()) * (node - deq_iter.node - 1) + (cur - first) + (deq_iter.last - deq_iter.cur));
		}

		self& operator ++ ()
		{
			cur++;
			if (cur == last)
			{
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		
		self operator ++ (int)
		{
			iterator temp = *this;
			++*this;
			return temp;
		}

		self& operator -- ()
		{
			if (cur == first)
			{
				set_node(node - 1);
				cur = last;
			}
			cur--;
			return *this;
		}

		self operator -- (int)
		{
			iterator temp = &this;
			--*this;
			return temp;
		}

		//deque迭代器中重载操作符中最为复杂的一个，下面的懂可以根据这个来完成
		self& operator += (distance_type n)
		{
			//计算从该段缓冲区中first起始偏移的位置
			distance_type offset = n + (cur - first);
			//判断这个偏移的位置是否是在本段中；因为起始位置是在first，那么你最多只能偏移0和buffer_size的距离
			if(offset >= 0 && offset < buffer_size())
				cur = first + offset;
			else
			{
				//进入else表示要转移到的位置不在目前所处于的缓冲区中，这样我们就需要判断节点的偏移距离node_offset
				//-offset的作用是实现offset去绝对值，这样就知道它跳转的距离，同时-offset-1就可以理解成将要转移的点，放到前一个缓冲区的last前的
				//位置，这样更加方便后续的计算
				distance_type node_offset = 
					offset > 0 ? distance_type(offset / distance_type(buffer_size())) : -distance_type((-offset - 1) / distance_type(buffer_size())) - 1;

				set_node(node + node_offset);

				//记住offset是以first为起点，该公式对node_offset为正的时候满足，通过考虑它为负数时也是满足的，所以直接仅仅调用该表达式就是可以的
				cur = first + (offset - node_offset * distance_type(buffer_size()));
			}
			return *this;
		}

		self operator + (distance_type n)
		{
			iterator temp = *this;
			temp+=n;
			return temp;
		}

		self& operator -= (distance_type n)
		{
			*this -= -n;
		}

		self operator - (distance_type n)
		{
			iterator temp = *this;
			temp -= n;
			return temp;
		}

		reference operator * () 
		{
			return *cur;
		}
		pointer operator -> () {return &(operator*());}
		reference operator [] (distance_type n) {return *(*this + n);}


		/*************************************************************************************************************************/

		//deque_iterator所要的特定迭代器，“跳转缓冲区”set_node
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + distance_type(deque_buf_size(sizeof(value_type)));
			//为了避免我忘记更改cur，我将cur设置成NULL，这样在对“没有赋值的cur”进行调用的时候就会产生错误，这样就可以发现没有
			//对cur进行赋值这个问题
			cur = NULL;
		}
	};


	//因为在duque这个数据结构中，我们要用到两个关于不同类型的配置器，所以直接上alloc，不使用那个统一的接口simple_alloc
	//
	template<class T, class Alloc = alloc>
	class deque
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef ptrdiff_t distance_type;

		typedef const T* const_pointer;
		typedef const T& const_reference;

		typedef pointer* map_pointer;
		typedef size_t size_type;

		//关于map的配置器和缓冲区的配置器
		typedef simple_alloc<T, Alloc> node_allocator;
		typedef simple_alloc<pointer, Alloc> map_allocator;

		typedef deque_iterator<T, T&, T*> iterator;
		//暂时想到利用下面这种const_iterator的可能：应该是我们遍历容器中的数据的时候，放置迭代器去更改其中的数据
		typedef deque_iterator<T, const T&, const T*> const_iterator;


	private:
		iterator start_;
		iterator finish_;

		map_pointer map;
		size_type map_size;

	public:
		/*****与构造函数、复制构造函数、assignment operator、析构函数（析构缓冲区上的所有元素，并且将map及所用到的缓冲区释放）***/
		//start_和finish_自带的赋值构造函数可以满足相关的设定
		deque( ) : start_(), finish_(), map_size(0), map_pointer(0) { fill_initialize(0, T()); }
		explicit deque(size_type n) : start_(), finish_(), map(0), map_size(0) {fill_initialize(n, T());}
		//一定要用is_Integer来区分下面两个！！！！！！！   
		//deque(size_type n, const value_type& val):start_(), finish_(), map(0), map_size(0) {fill_initialize(n, val);}
		deque(size_type n, const value_type& val);
		template<class InputIterator>
		deque(InputIterator first, InputIterator last);
		deque(const deque& deq);
		deque& operator = (const deque& deq);
		~deque();
		/*********************************************************************************************************************************/


		/*********************************************与逻辑比较相关*********************************************************************/
		friend bool operator == (const deque& lhs, const deque& rhs);
		friend bool operator != (const deque& lhs, const deque& rhs);
		/*********************************************************************************************************************************/

		/*********************************************与迭代器相关***********************************************************************/
		iterator begin() {return start_;}
		const_iterator begin() const {return start_;}
		iterator end() {return finish_;}
		const_iterator end() const {return finish_;}
		/*********************************************************************************************************************************/

		/*********************************************与容量相关*************************************************************************/
		size_type size() {return finish_ - start_;}
		bool empty() {return finish_ == start_;}
		size_type max_size() {return size_type(-1)}
		/*********************************************************************************************************************************/

		/*********************************************与访问元素相关*********************************************************************/
		reference front() {return *start_;}
		reference back() {return *(finish_ - 1);}
		const_reference front() const {return *start_;}
		const_reference back() const {return *(finish_ - 1);}
		//返回一个引用的元素在指定的位置pos。没有执行边界检查
		reference operator [] (size_type n)
		{
			return start_[distance_type(n)];
		}
		const_reference operator [] (size_type n) const
		{
			return start_[distance_type(n)];
		}

		reference at(size_type pos)
		{
			if (pos >= size())
				std::_Throw std:out_of_range();
			else
			{
				return start_[distance_type(n)];
			}
			
		}
		/*********************************************************************************************************************************/

		/*********************************************与改变容器相关**********************************************************************/
		void clear();
		iterator insert(iterator position, const value_type& val);
		void insert(iterator position, size_type n, const value_type& val);
		template<class InputIterator>
		void insert(iterator position, InputIterator first, InputIterator last);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

		void resize(size_type n, const value_type& val);
		void swap(const deque& deq);
		void push_back(const  value_type& val);
		void push_front(const value_type& val);
		void pop_back();
		void pop_front();
		/*********************************************************************************************************************************/


		/*****************************private函数*****************************************************************************************/
		private:
			enum {INITIAL_MAP_SIZE = 8};
			//不带参数的构造函数也是要使用这个函数的，只要把他的n设置为0就好了
			void fill_initialize(size_type n, const value_type& val);
			void create_map_and_nodes(size_type num_elements);
			//为每个map中的点分配缓冲区
			pointer allocate_node();
			template<class Integer>
			void deque_aux(Integer n, const value_type& val, _true_type);
			template<class InputIterator>
			void deque_aux(InputIterator first, InputIterator last, _false_type);
			//释放缓冲区
			void deallocate_node(pointer p);
			//释放中控区
			void deallocate_map(map_pointer mp);
	};

	/*****************************************************【deque的相关实现】************************************************************/

	//deque(size_type n, const value_type& val);
	template<class T, class Alloc>
	deque<T, Alloc>::deque(size_type n, const value_type& val)
	{
		typedef typename is_Integer<size_type>::class_type is_Int;
		deque_aux(n, val, is_Int());
	}

	template<class T, class Alloc>
	template<class InputIterator>
	deque<T, Alloc>::deque(InputIterator first, InputIterator last)
	{
		typedef typename is_Integer<InputIterator>::class_type is_Int;
		deque_aux(first, last, is_Int());
	}

	template<class T, class Alloc>
	deque<T, Alloc>::deque(const deque& deq)
	{
		size_type new_sz = deq.finish_ - deq.start_;
		create_map_and_nodes(new_sz);
		iterator temp = start_;
		uninitialized_copy(deq.start_, deq.finish_, start_);
	}

	template<class T, class Alloc>
	deque<T, Alloc>& deque<T, Alloc>::operator = (const deque& deq)
	{
		size_type new_sz = finish_ - start_;
		create_map_and_nodes(new_sz);
		iterator temp = start_;
		uninitialized_copy(deq.start_, deq.finish_, start_);
		return *this;
	}

	template<class T, class Alloc>
	deque<T, Alloc>::~deque()
	{
		//析构deque上面的所有函数
		destroy(start_, finish_);
		//释放各个节点上的缓冲区
		for (map_pointer temp = start_.node; temp != finish_.node; temp++)
		{
			deallocate_node(*temp);
		}
		deallocate_node(*finish_.node);
		//释放中控区map
		deallocate_map(map);
	}

	template<class T, class Alloc>
	bool operator == (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs)
	{
		const_iterator tempLhs = lhs.start_;
		const_iterator tempRhs = rhs.start_;

		for (; tempLhs != lhs.finish_ && tempRhs != rhs.finish_; tempLhs++, tempRhs++)
		{
			if (*tempLhs != *tempRhs)
				return false;
		}

		if(tempLhs != lhs.finish_ || tempRhs != rhs.finish_)
			return false;

		return true;
	}

	template<class T, class Alloc>
	bool operator != (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::fill_initialize(size_type n, const value_type& val)
	{
		//构建新表的时候，对start_和finish_已经构建好了
		create_map_and_nodes(n);
		map_pointer cur;
		//遍历map中有缓冲区的点
		for (cur = start_.node; cur < finish_.node; cur++)
			uninitialized_fill(*cur, *(cur + deque_buf_size(sizeof(value_type))), val);

		uninitialized_fill(finish_.first, finish_.cur, val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::create_map_and_nodes(size_type num_elements)
	{
		size_type num_nodes = num_elements / deque_buf_size(sizeof(value_type)) + 1;

		map_size = max(INITIAL_MAP_SIZE, num_nodes + 2);
		map = map_allocator::allocate(map_size);

		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur = nstart;
		for (; cur <= nfinish; cur++)
		{
			*cur = allocate_node();
		}

		start_.set_node(nstart);
		finish_.set_node(nfinish);
		start_.cur = start_.first;
		finish_.cur = finish_.first + num_elements % (deque_buf_size(sizeof(value_type)));
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::pointer deque<T, Alloc>::allocate_node()
	{
		return node_allocator::allocate(deque_buf_size(sizeof(value_type)));
	}

	template<class T, class Alloc>
	template<class Integer>
	void deque<T, Alloc>::deque_aux(Integer n, const value_type& val, _true_type)
	{
		fill_initialize(n, val);
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void deque<T, Alloc>::deque_aux(InputIterator first, InputIterator last, _false_type)
	{
		size_type new_sz = last - first;
		create_map_and_nodes(new_sz);
		iterator temp = start_;
		uninitialized_copy(first, last, start_);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::deallocate_node(pointer p)
	{
		node_allocator::deallocate(p, deque_buf_size(sizeof(value_type)));
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::deallocate_map(map_pointer mp)
	{
		map_allocator::deallocate(mp, map_size);
	}

}


#endif