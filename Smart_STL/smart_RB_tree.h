#ifndef _SMART_RB_TREE_H_
#define _SMART_RB_TREE_H_
#include "smart_iterator_base.h"
#include <stddef.h>
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

}

#endif