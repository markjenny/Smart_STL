#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"
#include "smart_functional.h"

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
		//list����Ҫ����������insert��erase��transfer�������ĺ�����Ҫ��������������
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
		template<class T,class Alloc>
		friend bool operator == (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
		template<class T, class Alloc>
		friend bool operator != (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
		/****************************************************************************************************************/


		/***************************************���������**************************************************************/
		size_type size();
		bool empty() const {return nodeIter.nodePtr->next == nodeIter.nodePtr;}

		/****************************************************************************************************************/

		/****************************************����������*********************************************************/
		iterator begin() {return iterator(nodeIter.nodePtr->next);}
		iterator end() {return nodeIter;}
		/****************************************************************************************************************/

		/****************************************�����Ԫ�����*********************************************************/
		reference front();
		reference back();
		const_reference front() const ;
		const_reference back() const;
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

		//list������Ҫ��һ��˽�к���transfer����Ϊ�������ʮ�������ô�����д��˽�У����Ұ���д�ɹ�����Ҫ
		//�úܶ�İ�ȫ��飬����û�б�Ҫ
		void transfer(iterator position, iterator first, iterator last);
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

	/**************************************���߼��Ƚ����***********************************************************/
	//ע�⣺�Ƚ����������Ƿ���ȵ�ʱ�򲢲���Ҫ�Ƚ���������ĵ�����������ȵģ�����������Ԫ����ͬ��������չʾ���û�
	//ʹ�õ�ʱ����һ���ǻ�װ������
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

	/***************************************���������**************************************************************/
	template<class T, class Alloc>
	typename list<T, Alloc>::size_type list<T, Alloc>::size()
	{
		distance_type n = distance(begin(), end());
		return (size_type)n;
	}

	/****************************************************************************************************************/


	/****************************************�����Ԫ�����*********************************************************/
	template<class T, class Alloc>
	typename list<T, Alloc>::reference list<T, Alloc>::front()
	{
		return nodeIter.nodePtr->next->data;
	}


	//��Դ�벻ͬ�ĵط�
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


	/******************************************��ı��������*******************************************************/
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

	//����Ĳ�������ĺô����ǣ����ĸ��Ӷ�ΪO(1)�����������ڲ���n����ʱ����Կ��ǲ���ѭ������ķ�ʽ������vector���źܴ�Ĳ�ͬ
	//���Ҳ���ѭ���ķ�ʽ
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
		//��Ϊ��inputIterator��������û�а취������������֮��ľ���
		for(; first != last; first++)
			insert(positon, *first);
	}
	//
	//��Ҫע���һ���ǣ�clearʱ�ǿ�����Ҫ�����п������end
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
		//��ʱֻʣһ���ڵ��ˣ����Ǹýڵ��next��prev��֪��ָ��������Կ�ָ�뻹Ҫָ���Լ�
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
		//����list��صĲ����У������ȶ�����õ��ǰһ���ڵ�ͺ�һ���ڵ�����ʹ�ñ�д�����ʱ���߼������ױ�������������Ҳ���������˶��������ڵ�����
		//���ڵ��������ǿ��Զ���ǰһ��ָ��͸�ָ��
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

	//������list���еĺ���
	//�ܵ���˵��splice�������������ô�Ѻá����������������ô�����position��[first,last)��
	template<class T, class Alloc>
	void list<T, Alloc>::splice(iterator positon, list& l)
	{
		if (0 != l.size())
			transfer(positon, l.begin(), l.end());
	}

	template<class T, class Alloc>
	void list<T, Alloc>::splice(iterator position, list& l, iterator i)
	{
		//����l��*this��ͬһ������
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

	//ֻ����ͬ��������Ԫ�زŻᱻɾ��ʣһ��
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
	// 	�����Compare�Ϳ����Էº�����ע���㷨��˼�����������
	template<class T, class Alloc>
	template<class Compare>
	void list<T, Alloc>::sort(Compare comp)
	{
		//list��sort�㷨�������ĵط����ǲ����õݹ�����ɹ鲢�㷨���Լ�����ȵݹ�����ĳ���ջ���
		//��fill���ڶ����Ѿ�ռ�е��ˡ�����������Ӧ��counter�г��ֹ�Ԫ��
		//����ǿ����������ֻ��һ��Ԫ�أ���ôû�б�Ҫ����
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
			//�˳�ѭ��һ���������������һ����counter[i]����û��ֵ����ô��������㷨��û��ֵ��ֱ�ӷ������
			//����һ���˳�ѭ���Ŀ��������ڵ�fill�����㡾��λ���������
			carry.swap(counter[i]);
			if (i == fill)
				fill++;
		}

		for(int i = 1; i < fill; i++)
			counter[i].merge(counter[i-1]);

		(*this).swap(counter[fill-1]);

	}

	// reverseҲ��ͨ��transferʵ�ֵ�
	template<class T, class Alloc>
	void list<T, Alloc>::reverse()
	{
		//����ǿ��������ֻ��һ��Ԫ�ص�������ô����Ҫ����reverse
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


	/*********************************************private����********************************************************/
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
	void list<T, Alloc>::transfer(iterator position, iterator first, iterator last)
	{
		//�ú����������ǽ�[first��last)֮���Ԫ�ز��뵽position��λ�ã����Ե�position��last��ʱ��û�б�Ҫ��ô��
		//�ˣ�������Ҫ˵���ǣ����position��[first,last)�����ǲ��õ�����
		if (position != last)
		{
			//����ȷ��positionǰ��λ�ã���Ϊ���ǲ���һ������
			link_type temp = position.nodePtr->prev;
			link_type first_prev = first.nodePtr->prev;
			link_type last_prev = last.nodePtr->prev;

			//��[first, last)�������ԭ�����б�������
			first_prev->next = last.nodePtr;
			last.nodePtr->prev = first_prev;

			//����ȡ����������[first, last)��װ��position��ǰ��
			temp->next = first.nodePtr;
			first.nodePtr->prev = temp;

			last_prev->next = position.nodePtr;
			position.nodePtr->prev = last_prev;
		}
	}
	/****************************************************************************************************************/
}
#endif