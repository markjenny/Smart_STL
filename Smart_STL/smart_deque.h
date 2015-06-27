#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"
#include "smart_uninitialized.h" 
#include "smart_construct.h"
#include "smart_algorithm.h"
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

		deque_iterator(const iterator& deq_iter) : cur(deq_iter.cur), first(deq_iter.first), last(deq_iter.last), node(deq_iter.node) {}

		self& operator = (const iterator& deq_iter) 
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
		bool operator == (const iterator& deq_iter) {return cur == deq_iter.cur;}
		bool operator != (const iterator& deq_iter) 
		{
			//tiny_stl受到了源码中operator<的影响，这里只比较deq_iter就可以，因为缓冲区中的每个点的cur都是不同的，即使
			//是不同的map_pointer，因为都是从内存空间中分配出去的
			return !(*this == deq_iter);
		}
		bool operator > (const iterator& deq_iter)
		{
			return  node == deq_iter.node ? cur > deq_iter.cur : node > deq_iter.node ; 
		}

		bool operator <= (const iterator& deq_iter)
		{
			return !( *this > deq_iter );
		}

		bool operator < (const iterator& deq_iter)
		{
			return !(deq_iter > *this);
		}

		bool operator >= (const iterator& deq_iter)
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
			//更正，我对这个set_node理解错误，它真正的目的应该是仅设定node，顺带限定了first和last，如果将cur设定成NULL，后面的reallocate_map使用就不是很顺利
			//cur = NULL;
		}

		template<class T, class Ref, class Ptr>
		friend deque_iterator<T, Ref, Ptr> operator + (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n);
		template<class T, class Ref, class Ptr>
		friend deque_iterator<T, Ref, Ptr> operator + (typename deque_iterator<T, Ref, Ptr>::distance_type n, const deque_iterator<T, Ref, Ptr>& it);
		template<class T, class Ref, class Ptr>
		friend deque_iterator<T, Ref, Ptr> operator - (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n);
		template<class T, class Ref, class Ptr>
		friend typename deque_iterator<T, Ref, Ptr>::distance_type operator - (const deque_iterator<T, Ref, Ptr>& it1, const deque_iterator<T, Ref, Ptr>& it2);
	};

	template<class T, class Ref, class Ptr>
	deque_iterator<T, Ref, Ptr> operator + (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n)
	{
		//左操作数都是const类型的，所以要重载一个operator+
		//using deque_iterator<T, Ref, Ptr>;
		deque_iterator<T, Ref, Ptr> temp(it);
		deque_iterator<T, Ref, Ptr>::distance_type offset = (temp.cur - temp.first) + n;
		deque_iterator<T, Ref, Ptr>::distance_type nodePageSize = deque_iterator<T, Ref, Ptr>::distance_type(deque_buf_size(sizeof(typename deque_iterator<T, Ref, Ptr>::value_type)));
		//在同一个bucket中
		if(offset >=0 && offset < nodePageSize)
			temp.cur += n;
		else
		{
			//分成了正增长和负增长
			deque_iterator<T, Ref, Ptr>::distance_type offset_node = 
				offset > 0 ? offset / nodePageSize
				: -deque_iterator<T, Ref, Ptr>::distance_type((-offset -1) / nodePageSize) - 1;
			deque_iterator<T, Ref, Ptr>::set_node(offset_node);
			temp.cur = temp.first + offset - offset_node * nodePageSize;
		}
		return temp;
	}

	template<class T, class Ref, class Ptr>
	deque_iterator<T, Ref, Ptr> operator + (typename deque_iterator<T, Ref, Ptr>::distance_type n, const deque_iterator<T, Ref, Ptr>& it)
	{
		return it + n;
	}

	template<class T, class Ref, class Ptr>
	deque_iterator<T, Ref, Ptr> operator - (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n)
	{
		return it + (-n);
	}

	template<class T, class Ref, class Ptr>
	typename deque_iterator<T, Ref, Ptr>::distance_type operator - (const deque_iterator<T, Ref, Ptr>& it1, const deque_iterator<T, Ref, Ptr>& it2)
	{
		typedef deque_iterator<T, Ref, Ptr>::distance_type distance_type;
		typedef deque_iterator<T, Ref, Ptr>::value_type value_type;
		return distance_type( ( it1.cur - it1.first ) + (it1.node - it2.node - 1) * distance_type(deque_buf_size(sizeof(value_type))) + ( it2.last - it2.cur ) );
	}


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
		template<class T, class Alloc>
		friend bool operator == (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
		template<class T, class Alloc>
		friend bool operator != (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
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
		size_type max_size() {return size_type(-1);}
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
			void push_back_aux(const value_type& val);
			void reserve_map_back(size_type nodes_to_add = 1);
			void reallocate_map(size_type nodes_to_add, bool add_at_front);
			void push_front_aux(const value_type& val);
			void reserve_map_front(size_type nodes_to_add = 1);
			void pop_back_aux();
			void pop_front_aux();
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
		distance_type new_elements = deq.finish_ - deq.start_;
		create_map_and_nodes(new_elements);
		iterator temp1 = start_;
		for (iterator temp2 = deq.start_; temp2 != deq.finish_; temp2++)
		{
			construct(&*temp2, *temp1);
			temp1++;
		}
	}

	template<class T, class Alloc>
	deque<T, Alloc>& deque<T, Alloc>::operator = (const deque& deq)
	{
		distance_type new_elements = deq.finish_ - deq.start_;
		create_map_and_nodes(new_elements);
		iterator temp1 = start_;
		for (iterator temp2 = deq.start_; temp2 != deq.finish_; temp2++)
		{
			construct(&*temp2, *temp1);
			temp1++;
		}
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


	/*********************************************与改变容器相关**********************************************************************/
	//deque的clear是要消除全部的元素，但是保留一个缓冲区

	template<class T, class Alloc>
	void deque<T, Alloc>::clear()
	{
		//针对头尾节点以外的缓冲区(因为这写缓冲区中一定是满的)
		for (map_pointer nodeTemp = start_.node + 1; nodeTemp < finish_.node; nodeTemp++)
		{
			//对缓冲区中的元素进行析构
			destroy(*nodeTemp, *nodeTemp + (distance_type)deque_buf_size(sizeof(value_type)));
			node_allocator::deallocate(*nodeTemp, (distance_type)deque_buf_size(sizeof(value_type)));
		}
		//头尾都有缓冲区，我们就要保留一个缓冲区
		if (start_.node != finish_.node)
		{
			destroy(start_.cur, start_.last);
			destroy(finish_.first, finish_.cur);
			node_allocator::deallocate(*finish_.node, (distance_type)deque_buf_size(sizeof(value_type));
		}
		else
			//只有一个缓冲区的时候要注意了，是start.cur到ffinish_.cur！！
			destroy(start_.cur, finish_.cur);
		//start_.cur就是现在start的状态，所以不用进行重新分配cur
		finish_ = start_;
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::insert(iterator position, const value_type& val)
	{
		if (position.cur == start_.cur)
		{
			push_front(val);
			return start_;
		}
		else if (position.cur == finish_.cur)
		{
			push_back(val);
			iterator temp = finish_;
			temp--;
			return temp;
		}
		else
			inset_aux(position, val);
	}
	三、void insert(iterator position, size_type n, const value_type& val);
	template<class InputIterator>
	四、void insert(iterator position, InputIterator first, InputIterator last);

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator position)
	{
		//next的作用其实不是为了返回迭代器，而是为了copy元素，因为我们只是要移动少量的元素，你就不知道那边的迭代器会失效
		iterator next = position;
		next++;

		//前半部分的元素
		distance_type index = position - start_;

		if(index < size() >> 1)
		{
			//表示前面的元素较少，可以移动前面的元素
			copy_backward(start_, position, next);
			pop_front();
		}
		else
		{
			//后半部分的点比较少，可以相对移动后半部分的点
			copy(next, finish_, position);
			pop_back();
		}
		//这里需要注意一下，因为我们不知道是移动了前面的元素还是后面的元素，所以迭代器会失效，最保险的做法还是从start数的迭代器
		return start_ + index;
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator first, iterator last)
	{
		if (first == start_ && last == finish_)
		{
			clear();
			return finish_;
		}
		else
		{
			//计算移除[first, last)之后前面的元素
			size_type n = last - first;
			size_type elements_before = first - start_;
			if (elements_before < (finish_ - start_ - n) >> 1)
			{
				//前面元素比较少
				copy_backward(start_, first, last);
				iterator new_start_ = start_ + n;
				destroy(start_, new_start_);				//析构元素；

				//释放缓冲区
				for (map_pointer temp = start_.node; temp < new_start_.node; temp++)
					deallocate_node(*temp);

				start_ = new_start_;
			}
			else
			{
				//后半部分的元素比较少
				copy(last, finish_, first);
				iterator new_finish_ = finish_ - n;
				destroy(new_finish_, finish_);				//析构元素；
				
				for(map_pointer temp = finish_.node; temp > new_finish_.node; temp--)
					deallocate_node(*temp);

				finish_ = new_finish_;
				
			}
		}
		return start_ + n;
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::resize(size_type n, const value_type& val)
	{
		if (n < size())
			return;
		//利用插入迭代器
		size_type new_n = n - size();
		insert(finish_, new_n, val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::swap(const deque& deq)
	{
		smart_stl::swap(map, deq.map);
		smart_stl::swap(map_size, deq.map_size);
		smart_stl::swap(start_, deq.start_);
		smart_stl::swap(finish_, deq.finish_);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::push_back(const  value_type& val)
	{
		//首先检测finish_对应的缓冲区中是否是最后一个元素，如果是最后一个元素的话那么就需要重新配置缓冲区给下一个map_node
		if(finish_.cur != finish_.last - 1)
		{
			//因为是uninitialized memory，所以要进行construct，因为有指针，所以采用new placement的方法
			construct(finish_.cur, val);
			++finish_.cur;
		}
		else
			push_back_aux(val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::push_front(const value_type& val)
	{
		//关于为什么在push_front的时候，只有看缓冲区中有一个配用元素，而在push_back的时候要看有两个配用元素？因为finish中的cur是空的，一直要保持是空的
		//只要有一个配用元素就可以放置
		if(start_.cur != start_.first)
		{
			construct(start_.cur, val);
			--start_.cur;
		}
		else
			//没有备用空间了，需要另外使用空间
			push_front_aux(val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::pop_back()
	{
		if (finish_.cur != finish_.first)
		{
			//表明缓冲区中还有至少一个元素
			--(finish_.cur);
			destroy(finish_.cur);
		}
		else
			pop_back_aux();
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::pop_front()
	{
		//缓冲区中至少有两个元素
		if (start_.cur != start_.last - 1)
		{
			destroy(start_.cur);
			(start_.cur)++;
		}
		else
			pop_front_aux();
	}
	/*********************************************************************************************************************************/


	template<class T, class Alloc>
	bool operator == (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs)
	{
		typename deque<T, Alloc>::iterator tempLhs = lhs.start_;
		typename deque<T, Alloc>::iterator tempRhs = rhs.start_;

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

	template<class T, class Alloc>
	void deque<T, Alloc>::push_back_aux(const value_type& val)
	{
		//查看是否有必要重新整理map中控点或重新分配map区
		reserve_map_back();
		//增加新的缓冲区
		*(finish_.node + 1) = allocate_node();
		try
		{
			//异常处理
			construct(finish_.cur, val);
			finish_.set_node(finish_.node + 1);
			finish_.cur = finish_.first;
		}
		catch(...)
		{
			node_allocator::deallocate(*(finish_.node + 1));
			//安全处理后要抛出异常
			throw;
		}
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reserve_map_back(size_type nodes_to_add = 1)
	{
		if ( (finish_.node - map +1) + nodes_to_add > map_size)
			reallocate_map(nodes_to_add, false);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reallocate_map(size_type nodes_to_add, bool add_at_front)
	{
		size_type old_node_size = finish_.node - start_.node + 1;
		size_type new_node_size = old_node_size + nodes_to_add;
		//判断是否有足够的空间，若有，那么不必新建空间
		map_pointer new_nstart_;
		if (map_size > 2 * new_node_size)
		{
			new_nstart_ = map + (map_size - new_node_size) / 2 +
				(true == add_at_front) ? nodes_to_add : 0;

			if (new_nstart_ < start_.node)
				copy(start_.node, finish_.node + 1, new_nstart_);
			else
				copy_backward(start_.node, finish_.node + 1, new_nstart_);
		}
		else
		{
			size_type new_map_size = map_size + max(map_size, new_node_size) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);

			new_nstart_ = new_map + (new_map_size - new_node_size) / 2 + 
				(true == add_at_front) ? nodes_to_add : 0;
			//把原map的内容拷贝过来
			copy(start_.node, finish_.node + 1, new_nstart_);
			//析构原来的map中控区
			//注意这里直接释放内容就可以了，因为存放的是16进制数，不存在对象，所以不用析构
			map_allocator::deallocate(map, map_size);

			map = new_map;
			map_size = new_map_size;
		}

		//重新设定start_和finish_的node，因为node已经重新分配了，所以start_和finish_中node对应的地方已经产生变化了，所以要重新分配
		start_.set_node(new_nstart_);
		finish_.set_node(new_nstart_ + old_node_size - 1);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::push_front_aux(const value_type& val)
	{
		reserve_map_front();
		*(start_.node - 1) = allocate_node();

		try
		{
			start_.set_node(start_.node - 1);
			start_.cur = start_.last - 1;
			construct(start_.cur, val);
		}
		catch(...)
		{
			//commit or rollback
			start_.set_node(start_.node + 1);
			start_.cur = start_.first;
			deallocate_node(*(start_.node - 1));
			throw;
		}
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reserve_map_front(size_type nodes_to_add  = 1 )
	{
		if (start_.node - map < nodes_to_add)
			reallocate_map(nodes_to_add, true);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::pop_back_aux()
	{
		//finish_中缓冲区没有元素
		deallocate_node(finish_.first);
		finish_.set_node(finish_.node - 1);
		finish_.cur = finish_.last - 1;
		destroy(finish_.cur);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::pop_front_aux()
	{
		destroy(start_.cur);
		start_.set_node(start_.node + 1);
		start_.cur = start_.first;
		node_allocator(*(start_.node - 1));
	}

}


#endif