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
	bool operator == (const stack<T, Sequence>& x, const stack<T, Sequence>& y);
	template<class T, class Sequence>
	bool operator != (const stack<T, Sequence>& x, const stack<T, Sequence>& y);

	template<class T, class Sequence = deque<T>>
	class stack
	{
		//�󶨵���Ե����
		friend bool operator ==<> (const stack& x, const stack& y);
		friend bool operator != <> (const stack& x, const stack& y);

	public:
		//����5�����õ����ͣ�vlaue_type,pointer,reference,size_type,distance_type
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
	};
}
#endif