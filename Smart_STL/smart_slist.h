#ifndef _SMART_SLIST_H_
#define _SMART_SLIST_H_
#include <cstddef>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_construct.h"
namespace smart_stl
{
	/*ǰ����Ҫ˵���Ķ���������֪��list��һ��˫���������slist��һ���������������û�취֪��ĳһ�ڵ��ǰ����ʲô�������Ҫ���Ǵ�ͷ���
	��ʼһ����ȥѰ�ң������ҪO(N)��ʱ�䣬Ч��û��list�ߣ�����ֻ����ͷ��㸽������insert��erase�������ط���������������ǲ�̫���ǵ�ѡ��*/

	//���ȹ���slist�ڵ�ṹ�����������ǲ����ˡ�effective C++���е�����44����������޹أ�non-typeparameters���Ĵ������templates

	//����list_node_base������T�޹صĴ��������γ���list_node_base
	struct slist_node_base
	{
		slist_node_base* next;
	};

	template<class T>
	struct slist_node : public slist_node_base  //is-a�Ĺ�ϵ
	{
		T data;
	};

	//����ȫ�ֺ�������֪ĳһ�㣬�ڸõ����µ㣨�����ʹ����OO�Ķ�̬��
	inline slist_node_base* slist_make_link(slist_node_base* prev, slist_node_base* new_node)
	{
		new_node->next = prev->next;
		prev->next = new_node;
		return new_node;
	}

	inline size_t slist_size(slist_node_base* pHead )
	{
		size_t result = 0;
		while(pHead != NULL)
		{
			result++;
			pHead = pHead->next;
		}
		return result;
	}

	//����slist�������Ļ����ṹ
	struct slist_iterator_base
	{
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;
		typedef forward_iterator_tag iterator_category;

		slist_node_base* node;

		//���캯��
		explicit slist_iterator_base(slist_node_base* temp) : node(temp) {}

		void inc() {node = node->next;}

		bool operator == (const slist_iterator_base& rhs)	 { return node == rhs.node;}
		bool operator != (const slist_iterator_base& rhs) { return node != rhs.node; }
	};

	//����slist��������ģ���������
	template<class T, class Ref, class Ptr>		//�һ����е�û�㶮ΪʲôҪ��ô����ֱ��ʹ��T��������
	struct slist_iterator : public slist_iterator_base
	{
		//�����������Ĳ���һ���趨�������ͣ��������е�㲻��Ϊʲô����iterator��selfʲôʱ����ʲôʱ����
		typedef slist_iterator<T, T&, T*> iterator;
		typedef slist_iterator<T, const T&, const T*> const_iterator;
		typedef slist_iterator<T, Ref, Ptr> self;

		typedef Ref reference;
		typedef Ptr pointer;
		typedef T value_type;
		typedef slist_node<T> slist_node;

		//�����������Ĳ���������ù��캯�������ƹ��캯����slist�Ĺ��췽ʽ��list����ͬ�ģ����������ֹ��캯��
		slist_iterator() : slist_iterator_base(0) {}
		slist_iterator(slist_node* x) : slist_iterator_base(x) {}
		slist_iterator(const iterator& x) : slist_iterator_base(x.node) {}

		//����ģ������޹صĴ��������templates�����Һܺõ�������OO��˼��
		reference operator* () { return ((slist_node*)node)->data; }
		pointer operator->() { return &(operator*()); }

		self& operator ++ ()
		{
			inc();
			return *this;
		}

		self operator ++ (int)
		{
			self temp = *this;
			inc();
			return temp;
		}
		//��Ϊ����forward����iterator������û��operator--
		//���л���һ����Ҫע��ľ��ǣ�����û����slist_iterator�ж�==�ͣ�=�������أ���Ϊslist_iterator���бȽϵı��ʾ��Ǽ����ָ���
		//�ڵ��Ƿ���ͬ����ô��slist_iterator������бȽ��ǣ����Զ����û����е�operator == and operator !=�����Կ���ֱ����
	};

	//����slist�����ݽṹ
	template<class T, class Alloc = alloc>
	class slist
	{
	public:
		//��������������ͼ���������
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;

		//��������
		typedef const T* const_pointer;
		typedef const T& const_reference;

		//����iterator
		typedef slist_iterator<T, T&, T*> iterator;
		typedef slist_iterator<T, const T&, const T*> const_iterator;
	private:
		typedef slist_node<T> slist_node;
		typedef slist_node_base slist_node_base;
		typedef slist_iterator_base slist_iterator_base;
		//ָ��������
		typedef simple_alloc<slist_node, Alloc> slist_node_allocator;

		slist_node_base head;

	public:
		//�빹����������صĺ���
		slist()  { head.next = 0; }
		~slist() {clear(); }
		//���߼��Ƚ����
		//�������ĳߴ��й�
		size_type size() const {return slist_size(head.next);}
		bool empty() const {return head.next == 0;}
		//�������ĵ��������
		//�����head������һ����ʡ�ڴ�����ã���Ϊstruct�о�ֻ��һ��ָ�룬�������ǿ���ֱ������slist_node_base�������
		iterator begin() {return iterator((slist_node*)head.next);}
		iterator end() {return iterator(0);}		//0������ΪNULL

		const_iterator begin() const {return iterator((slist_node*)head.next);}
		const_iterator end() const {return iterator(0);}
		//��������Ԫ���й�
		reference front() {return ((slist_node*)head)->data;}
		const_reference front() const  {return ((slist_node*)head)->data;}
		//��ı��������
		//slist���ڶ�β�Ĳ�������û�еģ���ΪЧ��̫���ˣ�����back,push_back,pop_back��
		void swap(const slist& rhs);
		void push_front(const value_type& x);
		void pop_front();
		void clear();

	private:
		//��public������ص�˽�к���
		slist_node* get_node() {return slist_node_allocator::allocate(1);}
		void put_node(slist_node* p) {slist_node_allocator::deallocate(p, 1);}

		slist_node* create_node(const value_type& x)
		{
			slist_node* tempNode = get_node();
			construct(&(tempNode->data), x);
			tempNode->next = NULL;
			return tempNode;
		}

		void destroy_node(slist_node* p)
		{
			destroy(&(p->data));
			put_node(p);
		}

	};


	//��ı��������
	template<class T, class Alloc>
	void slist<T, Alloc>::swap(const slist& rhs)
	{
		swap(head, rhs.head);
	}

	template<class T, class Alloc>
	void slist<T, Alloc>::push_front(const value_type& x)
	{
		slist_make_link(&head, create_node(x));
	}

	template<class T, class Alloc>
	void slist<T, Alloc>::pop_front()
	{
		slist_node* temp = (slist_node*)head.next;
		head.next = temp->next;
		destroy_node(temp);
	}

	template<class T, class Alloc>
	void slist<T, Alloc>::clear()
	{
		slist_node* temp = (slist_node*)head.next;
		while (temp != NULL)
		{
			slist_node* next = /*static_cast<slist_node*>*/(temp->next);
			destroy_node(temp);
			temp = next;
		}
	}

}

#endif