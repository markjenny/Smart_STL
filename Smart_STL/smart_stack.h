#ifndef _SMART_STACK_H_
#define _SMART_STACK_H_
#include "smart_deque.h"
//针对stack的内部，我们采用的是deque作为它的sequence(因为stack也是一个序列式容器)
//但是需要注意的是，因为stack是“修改某物的接口，形成另外一种风貌”，具有这样性质的成为配接器(adapter)，
//所以stack成为container adapter，而不是container
namespace smart_stl
{
	template<class T, class Sequence>
	class stack;

	template<class T, class Sequence>
	bool operator == (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs);
	template<class T, class Sequence>
	bool operator != (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs);

	template<class T, class Sequence = deque<T>>
	class stack
	{
		//绑定的有缘函数
		friend bool operator ==<> (const stack& lhs, const stack& rhs);
		friend bool operator < <> (const stack& lhs, const stack& rhs);

	public:
		//定义5个常用的类型：vlaue_type,pointer,reference,size_type,distance_type
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::distance_type distance_type;
		typedef typename Sequence::pointer pointer;
		typedef typename Sequence::reference reference;
		//因为在top的其中一个函数中要定义出返回值为const_reference，所以我们在使用一个const_reference
		typedef typename Sequence::const_reference const_reference;
		//另外需要说的是，stack的构造函数比较特殊，详细间下面的代码示例;
		//可以看到stack（C++ 11以下）的构造函数只有两个
		/*********************************************************************
		#include <stack>
		#include <deque>
		#include <iostream>

		int main()
		{
			std::stack<int> c1;
			c1.push(5);
			std::cout << c1.size() << '\n';

			std::stack<int> c2(c1);
			std::cout << c2.size() << '\n';

			std::deque<int> deq {3, 1, 4, 1, 5};
			std::stack<int> c3(deq);
			std::cout << c3.size() << '\n';
		}
		*************************************************/
	private:
		Sequence c;
	public:
		stack():c() {}
		//下面这个构造函数采用的是“显示”的构造方式。采用隐式的话会出现这样的情况
		//尽量不采用这种隐式构造函数，注意下面这种方式采用的是复制构造函数，而不是assignment operator
		/*
		deque<int> a(10,22);
		stack<int> b = a;
		*/
		explicit stack(const Sequence& Seq_container):c(Seq_container) {}

		bool empty() const {return c.empty();}
		size_type size() const {return c.size();}

		reference top() {return c.back();}
		const_reference top const {return c.back();}

		void push(const value_type& val) {return c.push_back(val);}
		void pop() {return c.pop_back();}
	};

	template<class T, class Sequence>
	bool operator == (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs)
	{
		return (x.c == y.c);
	}

	template<class T, class Sequence>
	bool operator < (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs)
	{
		return (lhs.c < rhs.c);
	}


	//为什么剩下的重载操作符就设定成non-member的了呢？这是什么原因呢？（已经问蓝色了，看看巨巨给我什么答复）
	template<class T, class Sequence>
	bool operator != (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs)
	{
		return !(lhs.c == rhs.c);
	}

	template<class T, class Sequence>
	bool operator > (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs)
	{
		return rhs.c < lhs.c;
	}

	template<class T, class Sequence>
	bool operator >= (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs)
	{
		return !(lhs.c < rhs.c);
	}

	template<class T, class Sequence>
	bool operator <= (const stack<T, Sequence>& lhs, const stack<T, Sequence>& rhs)
	{
		return !(lhs.c > rhs.c);
	}
}
#endif