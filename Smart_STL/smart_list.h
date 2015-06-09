#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"
#include "smart_alloc.h"

namespace smart_stl
{
	//定义list中的节点结构
	template<class T>
	struct list_node
	{
		typedef T data;
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

	private:
		list_nodePtr nodeP;

		//关于迭代器所要满足的所有操作符
	public:
		bool operator != (const iterator& x) const {return nodeP != x.nodeP;}
		bool operator == (const iterator& x) const {return nodeP == x.nodeP;}

		iterator& operator ++ () {nodeP = (list_nodePtr)nodeP.next; return *this;}
		iterator& operator ++ (int) 
		{
			//初始时候我写的代码是这样的，这是因为我没有搞懂迭代器和指针的区别----“迭代器不仅仅是指针！”
			//list_nodePtr temp = nodeP;
			//nodeP = (list_nodePtr)nodeP.next;
			//return temp;
			iterator temp = *this;
			//直接利用上一个函数中已经实现的++操作符
			++*this;
			return temp;
		}

		iterator& operator -- () {nodeP = (list_nodePtr)nodeP.prev; return *this;}
		iterator& operator -- (int) {iterator temp = *this; --*this; return temp;}

		reference operator * () {return (*nodeP).data;}
		//以下是【成员存取运算子】的标准写法！！！
		pointer operator -> () {return &(operator*());}

		template<class T>
		friend bool operator == (const list_iterator<T>& lhs, const list_iterator<T>& rhs) { return lhs.nodeP == rhs.nodeP;}
		template<class T>
		friend bool operator != (const list_iterator<T>& lhs, const list_iterator<T>& rhs) {return lhs.nodeP == rhs.nodeP;}
	};


	template<class T, class Alloc = alloc>
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
		typedef simple_alloc<node_type, Alloc> node_allocator;
	private:
		//用于与双向链表产生联系的node，该节点即是end()处的节点，该节点的特点是，如果初始化一个空链表的话，那么一定会有一个节点，否则我们
		//在后期加入节点的时候就无法操作，但是我们有要保证list看起来是空的，所以我们把这个节点设置为end的节点，一旦有节点加入，就将该节点指向
		//链表的first
		link_type node;

	public:
		//public的相关函数
		/*************************************与构造、析构、固执构造相关************************************************/
		list();		//产生空链表
		list(size_type n, const value_type& val = value_type());
		<class InputIterator>
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
		bool empty() const {return node->next == node;}

		/****************************************************************************************************************/

		/****************************************与迭代器相关*********************************************************/
		iterator begin();
		iterator end();
		/****************************************************************************************************************/

		/****************************************与访问元素相关*********************************************************/
		reference front();
		reference back();
		/****************************************************************************************************************/
		/******************************************与改变容器相关*******************************************************/
		void push_back(const value_type& val);
		void pop_back();
		void push_front(const value_type& val);
		void pop_back();

		iterator insert(iterator position, const value_type& val);
		void insert(iterator position, size_type n, const value_type& val);
		<class InputIterator>
		void insert(iterator positon, InputIterator first, InputIterator last);

		void clear();
		void swap(list& l);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

		//以下是list特有的函数
		void splice(iterator positon, list& l);
		void splice(iterator position, list& l, iterator i);
		void splice(iterator positon, list& l, iterator first);

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
	};
}
#endif