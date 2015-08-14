#ifndef _SMART_RB_TREE_H_
#define _SMART_RB_TREE_H_
#include <stddef.h>
#include <utility>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
//本节为构造底层数据红黑树，以学习为主，因为不太清除红黑树的删除过程（《算法》中关于节点删除的思想是理解的，但是具体还不太清楚，以此练手）
namespace smart_stl
{
	//用于定义节点颜色
	typedef bool rb_tree_color_type;
	const rb_tree_color_type rb_tree_red = false;		//bool类型为false的表示红色节点
	const rb_tree_color_type br_tree_black = true;		//黑色节点又true表示

	//effectivec++中的：将于class type无关的代码抽离模板，并将base类可以做得事情全部放到base类中
	struct rb_tree_node_base
	{
		typedef rb_tree_node_base* base_ptr;
		typedef rb_tree_color_type color_type;

		color_type color;
		base_ptr parent;
		base_ptr left;
		base_ptr right;

		//能在这个base上做的事情就在这个base上做，这个是什么原因我不是太清楚，可能是因为多态？
		//寻找最大值，因为红黑书是一个2-3树，而2-3树转换成二叉树即是二叉搜索树
		static base_ptr minimum(base_ptr x)
		{
			while(x->left != NULL)
				x = x->left;
			return x;
		}

		static base_ptr maximum(base_ptr x)
		{
			while (x->right != NULL)
				x = x->right;
			return x;
		}
	};

	//红黑书中带有类型参数的部分
	template<class Value>
	struct rb_tree_node : public rb_tree_node_base
	{
		//在rb_tree_node中建立这个link_type的意义？
		typedef rb_tree_node<Value>* link_type;
		Value node_value;
	};

	//设置基层迭代器（基层迭代器和基层节点建立链接）
	struct rb_tree_iterator_base		//这个iterator_base主要是以node_base为原型
	{
		typedef rb_tree_node_base::base_ptr base_ptr;
		//retree中的迭代器是可以前后走的，但是不具备定位能力，前进后退的顺序取决于二叉树的排列顺序
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t distance_type;
		//剩下三个value_type、pointer和reference都是参数类型相关的，所以这里没有设定
		base_ptr node_base;		//用来于迭代器产生一个链接，类似list中的用法
		void increment();		//用于迭代器的前进
		void decrement();	//用于迭代器的后退
	};

	//只要不是在插入和删除的时候，红黑树其实和一个BST是相同的
	void rb_tree_iterator_base::increment()
	{
		//总算用了一段时间搞懂了increment，其实这歌和二叉树中的还是差不多的，但是有一非常特殊的地方：在stl中的rb_tree的设计：在其中加入了header
		//结构，这个结构是header！它的右儿子是最大值，左儿子是最小值，他的父节点竟然是root，这时就会出现特殊的情况：root没有右儿子
		if(node_base->right != NULL)
		{
			node_base = node_base->right;		//取右子树，然后找最小的那个
			while(node_base != NULL)
				node_base = node_base->left;
		}
		else
		{
			//如果右子树没有，那么自己就是一颗子树的最大点，那么要找比他还大的，就要保证他是一个点的左子树的最大点，这样才能完成++操作
			base_ptr node_base_parent  = node_base->parent;
			while (node_base == node_base_parent->right)
			{
				node_base = node_base_parent;
				node_base_parent = node_base->parent;
			}

			//其实这个应该是对应说，如果node_base是当前树中的最大的一个点，那么他应该是++到header的地方，这里《源码剖析》里说的特殊情况应该是不对的
			if (node_base->right != node_base_parent)
				node_base = node_base_parent;
		}
	}

	void rb_tree_iterator_base::decrement()
	{
		//特殊情况：如果这个点是end(),也就是header点的话（header点和root有一个区别就是hearder是红色的，而根节点总是黑色的）
		if (node_base->color == rb_tree_red && node_base->parent->parent == node_base)
			node_base = node_base->right;
		if (node_base->left != 0)	//该节点有左子树， 那么就找左子树中最大的
		{
			node_base = node_base->left;
			while(node_base->right != NULL)
				node_base = node_base->right;
		}
		else
		{
			//如果没有左儿子，就说明它是一个子树中的最小值，那么找比他大一点点的就应该是该子树的父节点，并且这个子树是父节点的右子树
			base_ptr node_base_parent = node_base->parent;
			while (node_base == node_base_parent->left)
			{
				node_base = node_base_parent;
				node_base_parent = node_base->parent;
			}
			node_base = node_base_parent;
		}
	}

	template<class Value, class Ref, class Ptr>
	struct rb_tree_iterator;
	
	template<class Value, class Ref, class Ptr>
	bool operator == (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs);

	template<class Value, class Ref, class Ptr>
	bool operator != (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs);

	//根据基层迭代器构建完成版迭代器
	template<class Value, class Ref, class Ptr>
	struct rb_tree_iterator : public rb_tree_iterator_base
	{
		typedef Value value_type;
		typedef Ref reference;
		typedef PTR pointer;
		//为什么要构建下面这些东西我还是没太搞懂
		typedef rb_tree_iterator<Value, Value&, Value*> iterator;
		typedef rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
		typedef rb_tree_iterator<Value, Ref, Ptr> self;
		/////////////////////////////////////////////////////
		typedef rb_tree_iterator<Value>* link_type;

		//迭代器构建的套路一般是先typedef各种类型，然后构造函数，各种重载操作符：++，--，*，->， == ，！=
		rb_tree_iterator(){}
		//下面这两中用法和slist简直是太像了，但是为什么我还不太清楚，书看的还是少
		rb_tree_iterator(link_type x) {node_base = x;}
		rb_tree_iterator(const iterator&) {node_base = iterator.node_base;}

		reference operator* (){ return link_type(node_base)->node_value;}
		pointer operator->() {return &(operator*());}

		self& operator++()
		{
			increment();		//用于迭代器的前进
			return *this;
		}
		self operator++(int)
		{
			self temp = *this;
			increment();
			return temp;
		}

		self& operator-- ()
		{
			decrement();	//用于迭代器的后退
			return *this;
		}

		self operator++ (int)
		{
			self temp = *this;
			decrement();	//用于迭代器的后退
			return self;
		}

		friend bool operator == (const self& lhs, const self& rhs);
		friend bool operator != (const self& lhs, const self& rhs);
	};

	template<class Value, class Ref, class Ptr>
	bool operator == (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs)
	{
		return lhs.base_ptr == rhs.base_ptr;
	}

	template<class Value, class Ref, class Ptr>
	bool operator != (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs)
	{
		return lhs.base_ptr != rhs.base_ptr;
	}

	//构建红黑的数据结构,KeyofValue应该是一个仿函数
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc = alloc>
	class rb_tree
	{
	public:
		//首先是只有rb_tree需要用到的一些type
		typedef void* void_pointer;
		typedef rb_tree_node_base* base_ptr;
		typedef rb_tree_node<Value> rbtree_node;
		typedef simple_alloc<rbtree_node, Alloc> rb_tree_node_allocator;
		typedef rb_tree_color_type color_type;
		typedef rbtree_node* link_type;
		typedef Key key_type;

		//接着定义每个数据都需要的type：value_type，pointer等
		typedef Value value_type;
		typedef Value& reference;
		typedef Value* pointer;
		typedef const Value* const_pointer;
		typedef const Value& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;

	private:
		//构造节点等等，这点带有链表的数据结构都是一个德行
		link_type get_node() //这个名字带波浪线是几个意思？我在这个类里面定义之后应该会覆盖的
		{
			return rb_tree_node_allocator::allocate();
		}

		void put_node(link_type p)
		{
			rb_tree_node_allocator::deallocate(p);
		}

		//创建节点
		link_type create_node(const value_type& x)
		{
			link_type temp = get_node();
			construct(&(temp->node_value), x);
			return temp;
		}

		//复制节点：主要是复制节点的颜色和值
		link_type clone_node(link_type x)
		{
			link_type temp = create_node(x->node_value);
			temp->color = x->color;
			temp->left = 0; //(在C++中，任何类型都是有0值得，另外BJ不是很喜欢宏~)
			temp->right = 0;
			return temp;
		}

		//删除节点
		void destroy_node(link_type x)
		{
			destroy(&x->node_value);
			put_node(x);
		}

	private:
		//用三笔数据来表示整颗红黑树
		size_type amountOfNode;
		link_type header;
		Compare key_compare;		//节点的比较规则（一般应该都是BST）

	private:
		//这里又利用了多态性，并且root、left等都是获得本体，那么就是引用，但是为什么《源码剖析中要用（link_type&）来转换呢？》
		//与hearder相关
		link_type& root() 
		{
			return (link_type)(header->parent);		//因为我们在其中的parent是base类型，需要转换一下指针的解释方式
		}
		link_type& leftmost()
		{
			return (link_type)(header->left);
		}
		link_type& rightmost()
		{
			return (link_type)(header->right);
		}

		//获取一个节点的相关信息
		link_type& left(link_type& x)
		{
			return (link_type)(x->left);
		}
		link_type& right(link_type& x)
		{
			return (link_type)(x->right);
		}
		link_type& parent(link_type& x)
		{
			return (link_type)(x->parent);
		}
		reference value(link_type& x)
		{
			return x->node_value;
		}
		//因为这个key不可能改变，所以在函数的起始处加入const
		const Key& key(link_type& x)
		{
			return KeyofValue()(x->node_value);
		}
		color_type& color(link_type& x)
		{
			return (color_type)(x->color);
		}

		//之前获取一个节点的信息是利用的派生类的指针，根据多态性可以考虑基类的指针
		link_type& left(base_ptr& x)
		{
			return (link_type)(x->left);
		}
		link_type& right(base_ptr& x)
		{
			return (link_type)(x->right);
		}
		link_type& parent(base_ptr& x)
		{
			return (link_type)(x->parent);
		}
		color_type& color(base_ptr& x)
		{
			return (color_type)(x->color);
		}
		value_type& value(base_ptr& x)
		{
			return ((link_type)x)->node_value;
		}
		const Key& key(base_ptr& x)
		{
			return KeyofValue()(value(x));
		}
		link_type minimum(link_type x)		//个人觉得既然是iterator的完整版就不需要建立什么和base_ptr的关系
		{
			//在effective C++中的说法：link_type is a base_ptr
			return (link_type) rb_tree_node_base::minimum(x);		//因为可能出现覆盖
		}
		link_type maximum(link_type x)
		{
			return (link_type) rb_tree_node_base::maximum(x);
		}

		//以上的函数都是可以通过之前所定义的那些类型来搞定，现在需要定义迭代器
		public:
			typedef rb_tree_iterator<value_type, reference, pointer> iterator;
		private:
			//功能：在son位置上插入x值的节点，son的父节点为parent
			iterator _insert(base_ptr son, base_ptr parent, const value_type& x);
			//没搞懂copy得意义
			link_type _copy(link_type x, link_type p);
			void _erase(link_type x);		//因为要考虑到删除节点，那么久会调用这个点的node_value的析构函数，这样使用liink_type更加必要
			void _init()
			{
				header = get_node();
				//构造header上所需要的各种信息，因为不会用到header的值，所以这里不用初始化；
				color(header) = rb_tree_red;
				root() = 0;
				rightmost() = header;
				leftmost() = header;
			}
		public:
			//现在需要构建普遍的容器所需要的六个方面的函数
			//1.与构造，析构等有关；
			rb_tree(const Compare& comp = Compare()) : amountOfNode(0), key_compare(comp) {_init();}
			~rb_tree()
			{
				clear();
				destroy_node(header);
			}
			rb_tree& operator= (const rb_tree& x);
			//2.与逻辑比较有关；
			//3.与容器的size有关；
			bool empty() const 
			{
				return amountOfNode == 0;
			}
			size_type size() 
			{
				return amountOfNode;
			}
			size_type max_size()
			{
				return size_type(-1);
			}
			//下面这是什么鬼？
			Compare key_comp() const {return key_compare;}
			//4.与容器的迭代器有关；
			iterator begin()
			{
				return leftmost();
			}
			iterator end()
			{
				return header;
			}
			//5.与容器的元素有关；但是对于树应该是没有的
			//6.与改变容器相关；
			pair<iterator, bool> insert_unique(const value_type& x);		//保持
			iterator insert_equal(const value_type& x);
			void clear();
	};
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyofValue, Compare, Alloc>::iterator rb_tree<Key,Value, KeyofValue, Compare, Alloc>::insert_equal(const value_type&x)
	{
		link_type parent = header;
		link_type son = root();

		while (son != 0)
		{
			//BST形式的搜索，向下
			parent = son;
			son = key_compare(KeyofValue()(x), key(son)) ? left(son) : right(son);
		}
		return _insert(son, parent, x);
	}

	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	pair<typename rb_tree<Key, Value, KeyofValue, Compare, Alloc>::iterator, bool> 
		rb_tree<Key, Value, KeyofValue, Compare, Alloc>::insert_unique(const value_type& x)
	{
		link_type parent = header;
		link_type son = root();
		bool comp = true;
		while (son != 0)
		{
			parent = son;
			comp = key_compare(KeyofValue()(x), key(son)) ? left(son) : right(son);
			son = comp ? left(son) : right(son); 
		}

		//此时parent为叶子节点，son为0的位置
		iterator j = iterator(parent);		//因为要用到它进行++
		if(comp)
		{
			if (j == begin())
				return pair<iterator, bool>(_insert(son, parent, x), bool);
			else
				j--;
		}

		if(key_compare(key(j.node)), KeyofValue()(x));
			return pair<iterator, bool>(_insert(son, parent, x), true);

		return pair<iterator, bool>(j, false);
	}



}

#endif