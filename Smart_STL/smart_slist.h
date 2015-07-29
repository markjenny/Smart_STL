#ifndef _SMART_SLIST_H_
#define _SMART_SLIST_H_
#include <cstddef>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_construct.h"
namespace smart_stl
{
	/*前期需要说明的东西，我们知道list是一个双向的链表，而slist是一个单项的链表，我们没办法知道某一节点的前面是什么，这个需要我们从头结点
	开始一点点的去寻找，这就需要O(N)的时间，效率没有list高，所以只有在头结点附近进行insert和erase，其他地方进行这个操作都是不太明智的选择*/

	//首先构建slist节点结构，在这里我们采用了《effective C++》中的条款44：将与参数无关（non-typeparameters）的代码抽离templates

	//对于list_node_base它将与T无关的代码抽出来形成了list_node_base
	struct slist_node_base
	{
		slist_node_base* next;
	};

	template<class T>
	struct slist_node : public slist_node_base  //is-a的关系
	{
		T data;
	};

	//构建全局函数，已知某一点，在该点后加新点（这里就使用了OO的多态）
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

	//构建slist迭代器的基础结构
	struct slist_iterator_base
	{
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;
		typedef forward_iterator_tag iterator_category;

		slist_node_base* node;

		//构造函数
		explicit slist_iterator_base(slist_node_base* temp) : node(temp) {}

		void inc() {node = node->next;}

		bool operator == (const slist_iterator_base& rhs)	 { return node == rhs.node;}
		bool operator != (const slist_iterator_base& rhs) { return node != rhs.node; }
	};

	//构建slist迭代器的模板参数部分
	template<class T, class Ref, class Ptr>		//我还是有点没搞懂为什么要这么做，直接使用T不可以吗
	struct slist_iterator : public slist_iterator_base
	{
		//构建迭代器的步骤一：设定各种类型，但是我有点搞不懂为什么采用iterator和self什么时候用什么时候不用
		typedef slist_iterator<T, T&, T*> iterator;
		typedef slist_iterator<T, const T&, const T*> const_iterator;
		typedef slist_iterator<T, Ref, Ptr> self;

		typedef Ref reference;
		typedef Ptr pointer;
		typedef T value_type;
		typedef slist_node<T> slist_node;

		//构建迭代器的步骤二：设置构造函数，复制构造函数；slist的构造方式和list是相同的，都是这三种构造函数
		slist_iterator() : slist_iterator_base(0) {}
		slist_iterator(slist_node* x) : slist_iterator_base(x) {}
		slist_iterator(const iterator& x) : slist_iterator_base(x.node) {}

		//将与模板参数无关的代码抽离了templates，并且很好的利用了OO的思想
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
		//因为是是forward——iterator，所以没有operator--
		//其中还有一点需要注意的就是，我们没有在slist_iterator中对==和！=进行重载，因为slist_iterator进行比较的本质就是检测所指向的
		//节点是否相同，那么当slist_iterator对象进行比较是，会自动调用基类中的operator == and operator !=，所以可以直接用
	};

	//构造slist的数据结构
	template<class T, class Alloc = alloc>
	class slist
	{
	public:
		//定义五个基本类型及其他类型
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;

		//其他类型
		typedef const T* const_pointer;
		typedef const T& const_reference;

		//定义iterator
		typedef slist_iterator<T, T&, T*> iterator;
		typedef slist_iterator<T, const T&, const T*> const_iterator;
	private:
		typedef slist_node<T> slist_node;
		typedef slist_node_base slist_node_base;
		typedef slist_iterator_base slist_iterator_base;
		//指定配置器
		typedef simple_alloc<slist_node, Alloc> slist_node_allocator;

		slist_node_base head;

	public:
		//与构造析构等相关的函数
		slist()  { head.next = 0; }
		~slist() {clear(); }
		//与逻辑比较相关
		//与容器的尺寸有关
		size_type size() const {return slist_size(head.next);}
		bool empty() const {return head.next == 0;}
		//与容器的迭代器相关
		//这里的head可以起到一个节省内存的作用，因为struct中就只有一个指针，所以我们可以直接设置slist_node_base这个类型
		iterator begin() {return iterator((slist_node*)head.next);}
		iterator end() {return iterator(0);}		//0就是作为NULL

		const_iterator begin() const {return iterator((slist_node*)head.next);}
		const_iterator end() const {return iterator(0);}
		//与容器的元素有关
		reference front() {return ((slist_node*)head)->data;}
		const_reference front() const  {return ((slist_node*)head)->data;}
		//与改变容器相关
		//slist关于队尾的操作都是没有的，因为效率太低了，例如back,push_back,pop_back等
		void swap(const slist& rhs);
		void push_front(const value_type& x);
		void pop_front();
		void clear();

	private:
		//与public函数相关的私有函数
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


	//与改变容器相关
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