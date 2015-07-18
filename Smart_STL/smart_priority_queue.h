#ifndef _SMART_PRIORITY_QUEUE_H_
#define _SMART_PRIORITY_QUEUE_H_
#include "smart_vector.h"
#include "smart_functional.h"
#include "smart_heap.h"

namespace smart_stl
{
	template<class T, class Sequence, class Compare>
	priority_queue;

	template<class T, class Sequence, class Compare>
	void swap(const priority_queue<T, Sequence, Compare>& lhs, const priority_queue<T, Sequence, Compare>& rhs);
 

	//默认的compare是按照less来的
	template<class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type>>
	class priority_queue	//这个不需要析构函数，因为他的元素只有Sequence c，系统会调用c的析构函数来析构
	{
	public:
			friend void swap<>(priority_queue& lhs, priority_queue& rhs);

			typedef typename Sequence::value_type value_type;
			typedef typename Sequence::reference reference;
			typedef typename Sequence::pointer pointer;
			typedef typename Sequence::size_type size_type;
			typedef typename Sequence::distance_type;
			typedef typename Sequence::const_reference;		//这个因为要给top()的其中一个函数用
	private:
		Sequence c;					//底层容器
		Compare comp;			//比较规则，主要是用于heap的比较
	public:
		priority_queue():() {}
		explicit priority_queue(const Compare& x) : (), comp(x) {}
		//利用[first, last)构建对的时候我们首先使用的是vector的底层，有其中不太明白的地方，对于一个priority_queue来说，
		//在定义他的初始类型的时候，我们就已经确定了它的Compare类型，这样再重新没有意义呢
		template<class InputIterator>
		priority_queue(InputIterator first, InputIterator last, const Compare&x ):(first, last), comp(x)
		{
			make_heap(first, last, comp);
		}

		template<class InputIterator>
		priority_queue(InputIterator first, InputIterator last):(first, last)
		{
			make_heap(first, last, comp);
		}

		priority_queue(const priority_queue& pq):c(pq.c), comp(pq.comp) {}
		priority_queue& operator = (const priority_queue& pq) : c(pq.c), comp(pq.comp) {return *this;};

		//由于priority_queue的意义在于取优先最大/最小权值的排列在前面，所以不需要back，没有意义
		reference top()  {return c.front();}
		const_reference top() const {return c.front();}

		size_type size() {return c.size();}
		bool empty() {return c.empty();}

		void push(const value_type& val)
		{
			c.push_back(val);
			push_heap(c.begin(), c.end(), comp);
		}

		void pop()
		{
			pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}

		void swap(const priority_queue& pq)
		{
			smart_stl::swap(c, pq.c);
			smart_stl::swap(comp, pq.comp);
		}

	};

	template<class T, class Sequence, class Compare>
	void swap(const priority_queue<T, Sequence, Compare>& lhs, const priority_queue<T, Sequence, Compare>& rhs)
	{
		lhs.swap(rhs);
	}

		
}

#endif