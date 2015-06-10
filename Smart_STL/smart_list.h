#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"

namespace smart_stl
{
	//����list�еĽڵ�ṹ
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

	//����list�еĵ�����
	template<class T>
	struct list_iterator:public bidirectional_iterator<T, size_t>
	{
		//���T����list_iterator_ģ�����е�T����ͬ��
		typedef list_iterator<T> iterator;

		typedef list_node<T>* list_nodePtr;

		list_nodePtr nodePtr;

		//���ڵ�������Ҫ��������в�����
	public:
		//д�����캯��������ͬ��nodePtr��Ϊ��������ֱ�ӵ���
		list_iterator() {}
		list_iterator(list_nodePtr x): nodePtr(x) {}
		list_iterator(const iterator& x ):nodePtr(x.nodePtr) {}
// 		bool operator != (const iterator& x) const {return nodeP != x.nodeP;}
// 		bool operator == (const iterator& x) const {return nodeP == x.nodeP;}

		iterator& operator ++ () {nodePtr = nodePtr->next; return *this;}
		//��׺��++�����в��������������++++�����Խ�����ֵ����Ϊconst
		const iterator operator ++ (int) 
		{
			//��ʼʱ����д�Ĵ����������ģ�������Ϊ��û�и㶮��������ָ�������----����������������ָ�룡��
			//list_nodePtr temp = nodeP;
			//nodeP = (list_nodePtr)nodeP.next;
			//return temp;
			iterator temp = *this;
			//ֱ��������һ���������Ѿ�ʵ�ֵ�++������
			++*this;
			///////////////////////////////////////////////////////////////////////////////������ʱ������������������������������
			return temp;
		}

		iterator& operator -- () {nodePtr = nodePtr->prev; return *this;}
		const iterator operator -- (int) {iterator temp = *this; --*this; return temp;}

		//�������operator*
		reference operator * ()
		{
			return nodePtr->data;
		}
		//�����ǡ���Ա��ȡ�����ӡ��ı�׼д��������
		pointer operator -> () {return &(operator*());}

		template<class T>
		friend bool operator == (const list_iterator<T>& lhs, const list_iterator<T>& rhs) { return lhs.nodePtr == rhs.nodePtr;}
		template<class T>
		friend bool operator != (const list_iterator<T>& lhs, const list_iterator<T>& rhs) {return lhs.nodePtr != rhs.nodePtr;}
	};


	template<class T, class Alloc = simple_alloc<list_node<T>>>
	class list
	{
		//�κ���������Ҫ��5������
	public:
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;

		//Ϊlist�еĽڵ㶨����ص�����
		typedef list_node<T> node_type;
		typedef node_type* link_type;

		//�����������ָ�롢����
		typedef list_iterator<T> iterator;
		typedef const list_iterator<T> const_iterator;
		typedef const T* const_pointer;
		typedef const T& const_reference;

	private:
		typedef Alloc node_allocator;
	private:
		//������˫�����������ϵ��node���ýڵ㼴��end()���Ľڵ㣬�ýڵ���ص��ǣ������ʼ��һ��������Ļ�����ôһ������һ���ڵ㣬��������
		//�ں��ڼ���ڵ��ʱ����޷�����������������Ҫ��֤list�������ǿյģ��������ǰ�����ڵ�����Ϊend�Ľڵ㣬һ���нڵ���룬�ͽ��ýڵ�ָ��
		//�����first
		//link_type node;
		//��ʾ�����е�һ����Ӧ���ǵ�����������Ӧ��������ָ��list_node��ָ��
		iterator nodeIter;

	public:
		//public����غ���
		/*************************************�빹�졢��������ִ�������************************************************/
		list();		//����������
		explicit list(size_type n, const value_type& val = value_type());
		template<class InputIterator>
		list(InputIterator first, InputIterator last);
		//�����Ǹ��ƹ��캯������ֵ��������������������
		list(const list& l);
		//��ֵ����������ֵΪlist&��Ϊ�˿��Խ���������ֵ
		list& operator = (const list& l);
		~list();
		/****************************************************************************************************************/




		/**************************************���߼��Ƚ����***********************************************************/
		template<class T>
		friend bool operator == (const list<T>& lhs, const list<T>& rhs);
		template<class T>
		friend bool operator != (const list<T>& lhs, const list<T>& rhs);
		/****************************************************************************************************************/


		/***************************************���������**************************************************************/
		size_type size() const;
		bool empty() const {return nodeIter.nodePtr->next == nodeIter.nodePtr;}

		/****************************************************************************************************************/

		/****************************************����������*********************************************************/
		iterator begin() {return iterator(nodeIter.nodePtr->next);}
		iterator end() {return nodeIter;}
		/****************************************************************************************************************/

		/****************************************�����Ԫ�����*********************************************************/
		reference front();
		reference back();
		/****************************************************************************************************************/


		/******************************************��ı��������*******************************************************/
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

		//������list���еĺ���
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
		//�����Compare�Ϳ����Էº�����ע���㷨��˼�����������
		template<class Compare>
		void sort(Compare comp);

		void reverse();
		/****************************************************************************************************************/

		/*********************************************private����********************************************************/
		private:
			//�������ĸ����ýڵ�ĺ���������һ���ڵ�ռ�+�ͷ�һ���ڵ�ռ�+���ÿռ䲢����ڵ�+�����ڵ㲢�ͷſռ�
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

		//����������
		void empty_initialized();

		//���캯���ĸ�������
		template<class InputIterator>
		void list_aux(InputIterator first, InputIterator last, _false_type);

		template<class Integer>
		void list_aux(Integer n, const value_type& val, _true_type);
		/****************************************************************************************************************/
	};
	/************************************************��list�����ʵ�֡�***********************************************/
	/*************************************�빹�졢��������ִ�������************************************************/
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
		//�������ʱ��next��prev��ָ���Լ�
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

	/**************************************���߼��Ƚ����***********************************************************/
// 	template<class T>
// 	friend bool operator == (const list<T>& lhs, const list<T>& rhs);
// 	template<class T>
// 	friend bool operator != (const list<T>& lhs, const list<T>& rhs);
	/****************************************************************************************************************/

	/***************************************���������**************************************************************/
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