#ifndef _SMART_SLIST_H_
#define _SMART_SLIST_H_
#include <cstddef>
#include "smart_iterator_base.h"
namespace smart_stl
{
	/*ǰ����Ҫ˵���Ķ���������֪��list��һ��˫���������slist��һ���������������û�취֪��ĳһ�ڵ��ǰ����ʲô�������Ҫ���Ǵ�ͷ���
	��ʼһ����ȥѰ�ң������ҪO(N)��ʱ�䣬Ч��û��list�ߣ�����ֻ����ͷ��㸽������insert��erase�������ط���������������ǲ�̫���ǵ�ѡ��*/

	//���ȹ���slist�ڵ�ṹ�����������ǲ����ˡ�effective C++���е�����44����������޹أ�non-typeparameters���Ĵ������templates

	//����list_node_base������T�޹صĴ��������γ���list_node_base
	struct slist_node_base
	{
		slist_node_base* next;
	};

	template<class T>
	struct slist_node : public slist_node_base  //is-a�Ĺ�ϵ
	{
		T data;
	};

	//����ȫ�ֺ�������֪ĳһ�㣬�ڸõ����µ㣨�����ʹ����OO�Ķ�̬��
	inline slist_node_base* slist_make_link(slist_node_base* prev, slist_node_base* new_node)
	{
		new_node->next = prev->next;
		prev->next = new_node;
		return new_node;
	}

	inline size_t size(slist_node_base* pHead )
	{
		size_t result = 0;
		while(pHead != NULL)
		{
			result++;
			pHead = pHead->next;
		}
		return result;
	}

	//����slist�������Ļ����ṹ
	struct slist_iterator_base
	{
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;
		typedef forward_iterator_tag iterator_category;

		slist_node_base* node;

		//���캯��
		explicit slist_iterator_base(slist_node_base* temp) : node(temp) {}

		void inc() {node = node->next;}

		bool operator == (const slist_iterator_base& rhs)	 { return node == rhs.node;}
		bool operator != (const slist_iterator_base& rhs) { return node != rhs.node; }
	};

	//����slist��������ģ���������
	template<class T, class Ref, class Ptr>		//�һ����е�û�㶮ΪʲôҪ��ô����ֱ��ʹ��T��������
	struct slist_iterator : public slist_iterator_base
	{
		//�����������Ĳ���һ���趨��������
		typedef slist_iterator<T, T&, T*> iterator;
		typedef slist_iterator<T, const T&, const T*> const_iterator;
		typedef slist_iterator<T, Ref, Ptr> self;

		//�����������Ĳ���������ù��캯�������ƹ��캯����
	};

}

#endif