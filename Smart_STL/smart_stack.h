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
	bool operator == (const stack<T, Sequence>& x, const stack<T, Sequence>& y);
	template<class T, class Sequence>
	bool operator != (const stack<T, Sequence>& x, const stack<T, Sequence>& y);

	template<class T, class Sequence = deque<T>>
	class stack
	{
		//绑定的有缘函数
		friend bool operator ==<> (const stack& x, const stack& y);
		friend bool operator != <> (const stack& x, const stack& y);

	public:
		//定义5个常用的类型：vlaue_type,pointer,reference,size_type,distance_type
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
	};
}
#endif