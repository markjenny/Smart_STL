#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"
#include "smart_functional.h"

namespace smart_stl
{
	//定义list中的节点结构
	template<class T>
	struct list_node
	{
		T data;
		list_node<T>* prev;
		list_node<T>* next;

		list_node(const T&, list_node* p, list_node* n):data(T), prev(p), next(n) {}

		bool operator == (const list_node& n)
		{
			return data == n.data && prev == n.prev && next == n.next;
		}
	};

	//定义list中的迭代器
	template<class T>
	struct list_iterator:public bidirectional_iterator<T, size_t>
	{
		//这个T是与list_iterator_模板类中的T是相同的
		typedef list_iterator<T> iterator;

		typedef list_node<T>* list_nodePtr;

		list_nodePtr nodePtr;

		//关于迭代器所要满足的所有操作符
	public:
		//写出构造函数，便于同构nodePtr作为参数可以直接调用
		list_iterator() {}
		list_iterator(list_nodePtr x): nodePtr(x) {}
		list_iterator(const iterator& x ):nodePtr(x.nodePtr) {}
		// 		bool operator != (const iterator& x) const {return nodeP != x.nodeP;}
		// 		bool operator == (const iterator& x) const {return nodeP == x.nodeP;}

		iterator& operator ++ () {nodePtr = nodePtr->next; return *this;}
		//后缀的++操作中不允许进行连续的++++，所以将返回值设置为const
		const iterator operator ++ (int) 
		{
			//初始时候我写的代码是这样的，这是因为我没有搞懂迭代器和指针的区别----“迭代器不仅仅是指针！”
			//list_nodePtr temp = nodeP;
			//nodeP = (list_nodePtr)nodeP.next;
			//return temp;
			iterator temp = *this;
			//直接利用上一个函数中已经实现的++操作符
			++*this;
			///////////////////////////////////////////////////////////////////////////////返回临时变量、、、、、、、、、、、、、
			return temp;
		}

		iterator& operator -- () {nodePtr = nodePtr->prev; return *this;}
		const iterator operator -- (int) {iterator temp = *this; --*this; return temp;}

		//有问题的operator*
		reference operator * ()
		{
			return nodePtr->data;
		}
		//以下是【成员存取运算子】的标准写法！！！
		pointer operator -> () {return &(operator*());}

		template<class T>
		friend bool operator == (const list_iterator<T>& lhs, const list_iterator<T>& rhs) { return lhs.nodePtr == rhs.nodePtr;}
		template<class T>
		friend bool operator != (const list_iterator<T>& lhs, const list_iterator<T>& rhs) {return lhs.nodePtr != rhs.nodePtr;}
	};


	template<class T, class Alloc = simple_alloc<list_node<T>>>
	class list
	{
		//list最重要的三个函数insert和erase和transfer，其他的函数主要都是依据这三个
		//任何容器所需要的5个类型
	public:
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;

		//为list中的节点定义相关的类型
		typedef list_node<T> node_type;
		typedef node_type* link_type;

		//定义迭代器、指针、引用
		typedef list_iterator<T> iterator;
		typedef const list_iterator<T> const_iterator;
		typedef const T* const_pointer;
		typedef const T& const_reference;

	private:
		typedef Alloc node_allocator;
	private:
		//用于与双向链表产生联系的node，该节点即是end()处的节点，该节点的特点是，如果初始化一个空链表的话，那么一定会有一个节点，否则我们
		//在后期加入节点的时候就无法操作，但是我们有要保证list看起来是空的，所以我们把这个节点设置为end的节点，一旦有节点加入，就将该节点指向
		//链表的first
		//link_type node;
		//表示链表中的一个点应该是迭代器，而不应该是其中指向list_node的指针
		iterator nodeIter;

	public:
		//public的相关函数
		/*************************************与构造、析构、固执构造相关************************************************/
		list();		//产生空链表
		explicit list(size_type n, const value_type& val = value_type());
		template<class InputIterator>
		list(InputIterator first, InputIterator last);
		//下面是复制构造函数、赋值操作符函数、析构函数
		list(const list& l);
		//赋值操作符返回值为list&是为了可以进行连续赋值
		list& operator = (const list& l);
		~list();
		/****************************************************************************************************************/




		/**************************************与逻辑比较相关***********************************************************/
		template<class T,class Alloc>
		friend bool operator == (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
		template<class T, class Alloc>
		friend bool operator != (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
		/****************************************************************************************************************/


		/***************************************与容量相关**************************************************************/
		size_type size();
		bool empty() const {return nodeIter.nodePtr->next == nodeIter.nodePtr;}

		/****************************************************************************************************************/

		/****************************************与迭代器相关*********************************************************/
		iterator begin() {return iterator(nodeIter.nodePtr->next);}
		iterator end() {return nodeIter;}
		/****************************************************************************************************************/

		/****************************************与访问元素相关*********************************************************/
		reference front();
		reference back();
		const_reference front() const ;
		const_reference back() const;
		/****************************************************************************************************************/


		/******************************************与改变容器相关*******************************************************/
		void push_back(const value_type& val);
		void pop_back();
		void push_front(const value_type& val);
		void pop_front();

		iterator insert(iterator position, const value_type& val);
		void insert(iterator position, size_type n, const value_type& val);
		template<class InputIterator>
		void insert(iterator positon, InputIterator first, InputIterator last);

		void clear();
		void swap(list& l);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

		//以下是list特有的函数
		void splice(iterator positon, list& l);
		void splice(iterator position, list& l, iterator i);
		void splice(iterator positon, list& l, iterator first, iterator last);

		void remove(const value_type& val);
		template<class Predicate>
		void remove_if(Predicate pred);

		void unique();
		template<class BinaryPredicate>
		void unique(BinaryPredicate binary_pred);

		void merge(list& l);
		template<class Compare>
		void merge(list& l, Compare comp);

		void sort();
		//哲理的Compare就可以试仿函数，注重算法的思想而不是输入
		template<class Compare>
		void sort(Compare comp);

		void reverse();
		/****************************************************************************************************************/

		/*********************************************private函数********************************************************/
	private:
		//首先是四个配置节点的函数：配置一个节点空间+释放一个节点空间+配置空间并构造节点+析构节点并释放空间
		link_type get_node() {return node_allocator::allocate();}
		void put_node(link_type p) {node_allocator::deallocate(p);}

		link_type create_node(const value_type& val)
		{
			link_type temp = get_node();
			smart_stl::construct(&(temp->data), val);
			return temp;
		}

		void destroy_node(link_type p)
		{
			destroy(p);
			put_node(p);
		}

		//构建空链表
		void empty_initialized();

		//构造函数的辅助函数
		template<class InputIterator>
		void list_aux(InputIterator first, InputIterator last, _false_type);

		template<class Integer>
		void list_aux(Integer n, const value_type& val, _true_type);

		//list中最重要的一个私有函数transfer，因为这个函数十分容易用错，所以写成私有，而且把它写成共有需要
		//用很多的安全检查，而且没有必要
		void transfer(iterator position, iterator first, iterator last);
		/****************************************************************************************************************/
	};
	/************************************************【list的相关实现】***********************************************/
	/*************************************与构造、析构、固执构造相关************************************************/
	template<class T, class Alloc>
	list<T, Alloc>::list()
	{
		empty_initialized();
	}

	template<class T, class Alloc>
	list<T, Alloc>::list(size_type n, const value_type& val = value_type())
	{
		typedef typename is_Integer<size_type>::class_type is_Int;
		list_aux(n, val, is_Int());
	}

	template<class T, class Alloc>
	template<class InputIterator>
	list<T, Alloc>::list(InputIterator first, InputIterator last)
	{
		typedef typename is_Integer<InputIterator>::class_type is_Int;
		list_aux(first, last, is_Int());
	}


	template<class T, class Alloc>
	list<T, Alloc>::list(const list& l)
	{
		empty_initialized();
		iterator list_end = iterator(l.nodeIter);
		iterator list_start = iterator(l.nodeIter.nodePtr->next);


		for (; list_start != list_end; list_start++)
			push_back(*list_start);
	}

	template<class T, class Alloc>
	list<T, Alloc>& list<T, Alloc>::operator =(const list& l)
	{
		empty_initialized();
		iterator list_end = l.nodePtr;
		iterator list_start = iterator(l.nodePtr);

		for (; list_start != list_end; list_start++)
			push_back(*list_start);

		return *this;
	}

	template<class T, class Alloc>
	list<T, Alloc>::~list()
	{
		iterator list_start = iterator(nodeIter.nodePtr->next);
		for (; list_start != nodeIter; )
		{
			iterator list_temp = iterator(list_start++);
			destroy_node(list_temp.nodePtr);
			//list_start++;
		}
		destroy_node(nodeIter.nodePtr);
	}

	/****************************************************************************************************************/

	/**************************************与逻辑比较相关***********************************************************/
	//注意：比较两个链表是否相等的时候并不是要比较两个链表的迭代器都是相等的，而是链表中元素相同，“链表”展示给用户
	//使用的时候是一个非环装的链表！
	template<class T, class Alloc>
	bool operator == (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
	{
		typename list<T, Alloc>::iterator lhs_startIter = typename list<T, Alloc>::iterator(lhs.nodeIter.nodePtr->next);
		typename list<T, Alloc>::iterator rhs_startIter = typename list<T, Alloc>::iterator(rhs.nodeIter.nodePtr->next);
		for (; lhs_startIter != lhs.nodeIter && rhs_startIter != rhs.nodeIter; lhs_startIter++, rhs_startIter++)
		{
			if (*lhs_startIter != *rhs_startIter)
				return false;
		}
		if(lhs_startIter != lhs.nodeIter || rhs_startIter != rhs.nodeIter)
			return false;
		return true;
	}
	template<class T, class Alloc>
	bool operator != (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
	{
		return !(lhs == rhs);
	}
	/****************************************************************************************************************/

	/***************************************与容量相关**************************************************************/
	template<class T, class Alloc>
	typename list<T, Alloc>::size_type list<T, Alloc>::size()
	{
		distance_type n = distance(begin(), end());
		return (size_type)n;
	}

	/****************************************************************************************************************/


	/****************************************与访问元素相关*********************************************************/
	template<class T, class Alloc>
	typename list<T, Alloc>::reference list<T, Alloc>::front()
	{
		return nodeIter.nodePtr->next->data;
	}


	//和源码不同的地方
	template<class T, class Alloc>
	typename list<T, Alloc>::reference list<T, Alloc>::back()
	{
		return nodeIter.nodePtr->prev->data;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_reference list<T, Alloc>::front() const
	{
		return nodeIter.nodePtr->next->data;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_reference list<T, Alloc>::back() const
	{
		return nodeIter.nodePtr->prev->data;
	}

	/****************************************************************************************************************/


	/******************************************与改变容器相关*******************************************************/
	template<class T, class Alloc>
	void list<T, Alloc>::push_back(const value_type& val)
	{
		insert(end(), val);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::pop_back()
	{
		iterator temp = end();
		temp--;
		erase(temp);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::push_front(const value_type& val)
	{
		insert(begin(), val);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::pop_front()
	{
		erase(begin());
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, const value_type& val)
	{
		link_type temp = create_node(val);

		temp->next = position.nodePtr;
		temp->prev = (position.nodePtr)->prev;

		((position.nodePtr)->prev)->next = temp;
		position.nodePtr->prev = temp;

		return iterator(temp);
	}

	//链表的插入操作的好处就是，他的复杂度为O(1)，所以我们在插入n个的时候可以考虑采用循环插入的方式，这点和vector有着很大的不同
	//故我采用循环的方式
	template<class T, class Alloc>
	void list<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
	{
		while (n--)
		{
			insert(position, val);
		}
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void list<T, Alloc>::insert(iterator positon, InputIterator first, InputIterator last)
	{
		//因为是inputIterator所以我们没有办法计算他们两个之间的距离
		for(; first != last; first++)
			insert(positon, *first);
	}
	//
	//需要注意的一点是，clear时是空链表，要保持有空链表的end
	template<class T, class Alloc>
	void list<T, Alloc>::clear()
	{
		link_type cur = nodeIter.nodePtr;

		while (cur != nodeIter.nodePtr)
		{
			link_type temp = cur;
			cur = cur->next;

			destroy_node(temp);
		}
		//此时只剩一个节点了，但是该节点的next和prev不知道指向哪里，所以空指针还要指向自己
		nodeIter.nodePtr->next = nodeIter.nodePtr;
		nodeIter.nodePtr->prev = nodeIter.nodePtr;
	}


	template<class T, class Alloc>
	void list<T, Alloc>::swap(list& l)
	{
		smart_stl::swap(nodeIter, l.nodeIter);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position)
	{
		//在与list相关的操作中，我们先定义出该点的前一个节点和后一个节点往往使得编写代码的时候逻辑更容易保持清晰，并且也容易让他人读懂，对于单链表
		//对于单链表我们可以定义前一个指针和该指针
		link_type next_node = position.nodePtr->next;
		link_type prev_node = position.nodePtr->prev;

		prev_node->next = next_node;
		next_node->prev = prev_node;

		destroy_node(position.nodePtr);
		return iterator(next_node);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last)
	{
		for (; first != last; )
		{
			first = erase(first);
		}
		return first;
	}

	//以下是list特有的函数
	//总的来说，splice这个函数不是那么友好。。。。。，容易用错，比如position在[first,last)中
	template<class T, class Alloc>
	void list<T, Alloc>::splice(iterator positon, list& l)
	{
		if (0 != l.size())
			transfer(positon, l.begin(), l.end());
	}

	template<class T, class Alloc>
	void list<T, Alloc>::splice(iterator position, list& l, iterator i)
	{
		//存在l和*this是同一个链表
		iterator j = i;
		j++;

		if (i == position || j == position)
			return;

		transfer(position, i, j);
	}
	template<class T, class Alloc>
	void list<T, Alloc>::splice(iterator positon, list& l, iterator first, iterator last)
	{
		if(first == last)
			return;
		transfer(positon, first, last);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::remove(const value_type& val)
	{
		iterator temp = begin();
		for (; temp != nodeIter; )
		{
			if (*temp == val)
				temp = erase(temp);
			else
				temp++;
		}
	}

	template<class T, class Alloc>
	template<class Predicate>
	void list<T, Alloc>::remove_if(Predicate pred)
	{
		iterator temp = begin();
		for (; temp != nodeIter;)
		{
			if (pred(*temp))
				temp = erase(temp);
			else
				temp++;
		}
	}

	//只有相同且连续的元素才会被删除剩一个
	template<class T, class Alloc>
	void list<T, Alloc>::unique()
	{
		iterator first = begin();
		iterator last = end();

		if (nodeIter.nodePtr->next == nodeIter.nodePtr || nodeIter.nodePtr->next->next == nodeIter.nodePtr)
			return;

		iterator next = first;
		while(++next != last)
		{
			if (*first == *next)
			{
				erase(first);
			}
			first = next;
		}
	}


	template<class T, class Alloc>
	template<class BinaryPredicate>
	void list<T, Alloc>::unique(BinaryPredicate binary_pred)
	{
		iterator first = begin();
		iterator last = end();

		if (nodeIter.nodePtr->next = nodeIter.nodePtr || nodeIter.nodePtr->next->next = nodeIter.nodePtr)
			return;

		iterator next = first;
		while(++next != last)
		{
			if (binary_pred(*first, *next))
			{
				erase(first);
			}
			first = next;
		}
	}


	template<class T, class Alloc>
	void list<T, Alloc>::merge(list& l)
	{
		merge(l, less<T>());
	}

	template<class T, class Alloc>
	template<class Compare>
	void list<T, Alloc>::merge(list& l, Compare comp)
	{
		iterator first1 = begin();
		iterator last1 = end();

		iterator first2 = l.begin();
		iterator last2 = l.end();

		while(first1 != last1 && first2 != last2)
		{
			if (comp(*first1, *first2))
			{
				first1++;
			}
			else
			{
				iterator temp = first2;
				temp++;
				transfer(first1, first2, temp);
				first2 = temp;
			}
		}
		if (first2 != last2)
			transfer(last1, first2, last2);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::sort()
	{
		sort(less<T>());
	}
	// 	这里的Compare就可以试仿函数，注重算法的思想而不是输入
	template<class T, class Alloc>
	template<class Compare>
	void list<T, Alloc>::sort(Compare comp)
	{
		//list的sort算法最厉害的地方就是不利用递归来完成归并算法，以减少深度递归带来的程序栈溢出
		//【fill以内都是已经占有的了】，即是有相应的counter中出现过元素
		//如果是空链表或者是只有一个元素，那么没有必要排序
		if ( nodeIter.nodePtr->next == nodeIter.nodePtr || nodeIter.nodePtr->next->next == nodeIter.nodePtr )
			return;
		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill = 0;
		while(!empty())
		{
			int i = 0;
			carry.splice(carry.begin(), *this, begin());
			while(i < fill && !counter[i].empty())
			{
				counter[i].merge(carry, comp);
				carry.swap(counter[i++]);
			}
			//退出循环一般是有两种情况，一种是counter[i]里面没有值，那么根据这个算法，没有值就直接放在这里，
			//另外一种退出循环的可能是现在的fill不满足【进位】的情况了
			carry.swap(counter[i]);
			if (i == fill)
				fill++;
		}

		for(int i = 1; i < fill; i++)
			counter[i].merge(counter[i-1]);

		(*this).swap(counter[fill-1]);

	}

	// reverse也是通过transfer实现的
	template<class T, class Alloc>
	void list<T, Alloc>::reverse()
	{
		//如果是空链表或者只有一个元素的链表，那么不需要进行reverse
		if(nodeIter.nodePtr->next == nodeIter.nodePtr || nodeIter.nodePtr->next->next == nodeIter.nodePtr)
			return;
		iterator first = begin();
		first++;

		while(first != nodeIter)
		{
			iterator old = first;
			first++;
			transfer(begin(), old, first);
		}
	}
	/****************************************************************************************************************/


	/*********************************************private函数********************************************************/
	template<class T, class Alloc>
	void list<T, Alloc>::empty_initialized()
	{
		nodeIter.nodePtr = get_node();
		//空链表的时候将next和prev都指向自己
		nodeIter.nodePtr->next = nodeIter.nodePtr;
		nodeIter.nodePtr->prev = nodeIter.nodePtr;
	}

	template<class T, class Alloc>
	template<class Integer>
	void list<T, Alloc>::list_aux(Integer n, const value_type& val, _true_type)
	{
		empty_initialized();
		while(n--)
			push_back(val);
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void list<T, Alloc>::list_aux(InputIterator first, InputIterator last, _false_type)
	{
		empty_initialized();
		for(; first != last; first++)
			push_back(*first);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::transfer(iterator position, iterator first, iterator last)
	{
		//该函数的作用是将[first，last)之间的元素插入到position的位置，所以当position是last的时候，没有必要这么做
		//了，而且需要说的是，这个position和[first,last)可以是不用的链表
		if (position != last)
		{
			//首先确定position前的位置，因为我们插入一个区间
			link_type temp = position.nodePtr->prev;
			link_type first_prev = first.nodePtr->prev;
			link_type last_prev = last.nodePtr->prev;

			//将[first, last)的区域从原来的列表中区分
			first_prev->next = last.nodePtr;
			last.nodePtr->prev = first_prev;

			//将提取出来的区域[first, last)组装到position的前面
			temp->next = first.nodePtr;
			first.nodePtr->prev = temp;

			last_prev->next = position.nodePtr;
			position.nodePtr->prev = last_prev;
		}
	}
	/****************************************************************************************************************/
}
#endif