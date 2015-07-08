#ifndef _SMART_QUEUE_H_
#define _SMART_QUEUE_H_
#include "smart_deque.h"
namespace smart_stl
{
	//默认参数可以放在函数的声明中或实现中
	template<class T, class Sequence>
	class queue;

	template<class T, class Sequence>
	bool operator == (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs);

	template<class T, class Sequence>
	bool operator < (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs);

	template<class T, class Sequence = deque<T>>
	class queue
	{
	public:
		friend bool operator == <> (const queue& lhs, const queue& rhs);
		friend bool operator < <> (const queue& lhs, const queue& rhs);
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::pointer pointer;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::distance_type distance_type;
		//因为front的相关函数需要这个类型
		typedef typename Sequence::const_reference const_reference;
	private:
		Sequence c;
	public:
		queue() {}
		explicit queue(const Sequence& tempC) : c(tempC) {}

		//相关的函数都是与stack类似的，只是stack对应top，queue对应front和back
		//与元素相关的操作
		reference front() {return c.front();}
		const_reference front() {return c.front();}
		reference back() {return c.back();}
		const_reference back() {return c.back();}

		//与容量相关的操作
		bool empty() {return c.empty();}
		size_type size() {return c.size();}

		//对于正常的container还有与迭代器相关的操作，但是由于queue中没有迭代器相关的操作，所以没有这个操作

		//与改变容器相关的操作
		void push(const value_type& val) {c.push_back(val);}
		void pop() {c.pop_front();}
	}

	template<class T, class Sequence>
	bool operator == (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs)
	{
		return lhs.c == rhs.c;
	}

	template<class T, class Sequence>
	bool operator < (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs)
	{
		return lhs.c < rhs.c;
	}

	//下面这写non-member形式的operator重载和stack中时相同的，我们可以看出代码中尽量避免了都是friend中的函数
	template<class T, class Sequence>
	bool operator != (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs)
	{
		return !(lhs.c == rhs.c);
	}

	template<class T, class Sequence>
	bool operator >= (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs)
	{
		return !(lhs.c < rhs.c);
	}

	template<class T, class Sequence>
	bool operator > (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs)
	{
		return rhs.c < lhs.c;
	}

	template<class T, class Sequence>
	bool operator <= (const queue<T, Sequence>& lhs, const queue<T, Sequence>& rhs)
	{
		return !(lhs.c > rhs.c);
	}



}

#endif