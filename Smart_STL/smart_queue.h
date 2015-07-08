#ifndef _SMART_QUEUE_H_
#define _SMART_QUEUE_H_
#include "smart_deque.h"
namespace smart_stl
{
	//Ĭ�ϲ������Է��ں����������л�ʵ����
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
		//��Ϊfront����غ�����Ҫ�������
		typedef typename Sequence::const_reference const_reference;
	private:
		Sequence c;
	public:
		queue() {}
		explicit queue(const Sequence& tempC) : c(tempC) {}

		//��صĺ���������stack���Ƶģ�ֻ��stack��Ӧtop��queue��Ӧfront��back
		//��Ԫ����صĲ���
		reference front() {return c.front();}
		const_reference front() {return c.front();}
		reference back() {return c.back();}
		const_reference back() {return c.back();}

		//��������صĲ���
		bool empty() {return c.empty();}
		size_type size() {return c.size();}

		//����������container�������������صĲ�������������queue��û�е�������صĲ���������û���������

		//��ı�������صĲ���
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

	//������дnon-member��ʽ��operator���غ�stack��ʱ��ͬ�ģ����ǿ��Կ��������о��������˶���friend�еĺ���
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