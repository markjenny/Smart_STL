#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"

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
		template<class T>
		friend bool operator == (const list<T>& lhs, const list<T>& rhs);
		template<class T>
		friend bool operator != (const list<T>& lhs, const list<T>& rhs);
		/****************************************************************************************************************/


		/***************************************与容量相关**************************************************************/
		size_type size() const;
		bool empty() const {return nodeIter.nodePtr->next == nodeIter.nodePtr;}

		/****************************************************************************************************************/

		/****************************************与迭代器相关*********************************************************/
		iterator begin() {return iterator(nodeIter.nodePtr->next);}
		iterator end() {return nodeIter;}
		/****************************************************************************************************************/

		/****************************************与访问元素相关*********************************************************/
		reference front();
		reference back();
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
		list_aux(first, last, is_Int);
	}

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
	list<T, Alloc>::list(const list& l)
	{
		empty_initialized();
		iterator list_end = l.nodePtr;
		iterator list_start = iterator(l.nodePtr->next);


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
// 	template<class T>
// 	friend bool operator == (const list<T>& lhs, const list<T>& rhs);
// 	template<class T>
// 	friend bool operator != (const list<T>& lhs, const list<T>& rhs);
	/****************************************************************************************************************/

	/***************************************与容量相关**************************************************************/
	template<class T, class Alloc>
	typename list<T, Alloc>::size_type list<T, Alloc>::size() const 
	{
		distance_type n = distance(begin(), end());
		return (size_type)n;
	}

	/****************************************************************************************************************/

	template<class T, class Alloc>
	void list<T, Alloc>::push_back(const value_type& val)
	{
		insert(end(), val);
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
}
#endif