#ifndef _SMART_STACK_H_
#define _SMART_STACK_H_
#include "smart_deque.h"
//���stack���ڲ������ǲ��õ���deque��Ϊ����sequence(��ΪstackҲ��һ������ʽ����)
//������Ҫע����ǣ���Ϊstack�ǡ��޸�ĳ��Ľӿڣ��γ�����һ�ַ�ò���������������ʵĳ�Ϊ�����(adapter)��
//����stack��Ϊcontainer adapter��������container
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
		//�󶨵���Ե����
		friend bool operator ==<> (const stack& lhs, const stack& rhs);
		friend bool operator < <> (const stack& lhs, const stack& rhs);

	public:
		//����5�����õ����ͣ�vlaue_type,pointer,reference,size_type,distance_type
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::distance_type distance_type;
		typedef typename Sequence::pointer pointer;
		typedef typename Sequence::reference reference;
		//��Ϊ��top������һ��������Ҫ���������ֵΪconst_reference������������ʹ��һ��const_reference
		typedef typename Sequence::const_reference const_reference;
		//������Ҫ˵���ǣ�stack�Ĺ��캯���Ƚ����⣬��ϸ������Ĵ���ʾ��;
		//���Կ���stack��C++ 11���£��Ĺ��캯��ֻ������
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
		//����������캯�����õ��ǡ���ʾ���Ĺ��췽ʽ��������ʽ�Ļ���������������
		//����������������ʽ���캯����ע���������ַ�ʽ���õ��Ǹ��ƹ��캯����������assignment operator
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


	//Ϊʲôʣ�µ����ز��������趨��non-member�����أ�����ʲôԭ���أ����Ѿ�����ɫ�ˣ������޾޸���ʲô�𸴣�
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