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
 

	//Ĭ�ϵ�compare�ǰ���less����
	template<class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type>>
	class priority_queue	//�������Ҫ������������Ϊ����Ԫ��ֻ��Sequence c��ϵͳ�����c����������������
	{
	public:
			friend void swap<>(priority_queue& lhs, priority_queue& rhs);

			typedef typename Sequence::value_type value_type;
			typedef typename Sequence::reference reference;
			typedef typename Sequence::pointer pointer;
			typedef typename Sequence::size_type size_type;
			typedef typename Sequence::distance_type;
			typedef typename Sequence::const_reference;		//�����ΪҪ��top()������һ��������
	private:
		Sequence c;					//�ײ�����
		Compare comp;			//�ȽϹ�����Ҫ������heap�ıȽ�
	public:
		priority_queue():() {}
		explicit priority_queue(const Compare& x) : (), comp(x) {}
		//����[first, last)�����Ե�ʱ����������ʹ�õ���vector�ĵײ㣬�����в�̫���׵ĵط�������һ��priority_queue��˵��
		//�ڶ������ĳ�ʼ���͵�ʱ�����Ǿ��Ѿ�ȷ��������Compare���ͣ�����������û��������
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

		//����priority_queue����������ȡ�������/��СȨֵ��������ǰ�棬���Բ���Ҫback��û������
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