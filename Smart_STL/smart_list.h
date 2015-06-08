#ifndef _SMART_LIST_H_
#define _SMART_LIST_H_
#include "smart_iterator_base.h"

namespace smart_stl
{
	//����list�еĽڵ�ṹ
	template<class T>
	struct list_node
	{
		typedef T data;
		list_node<T>* prev;
		list_node<T>* next;
	};

	//����list�еĵ�����
	template<class T>
	struct list_iterator:public bidirectional_iterator<T, size_t>
	{
		//���T����list_iterator_ģ�����е�T����ͬ��
		typedef list_iterator<T> iterator;

		typedef list_node<T>* list_nodePtr;

	private:
		list_nodePtr nodeP;

		//���ڵ�������Ҫ��������в�����
	public:
		bool operator != (const iterator& x) const {return nodeP != x.nodeP;}
		bool operator == (const iterator& x) const {return nodeP == x.nodeP;}

		iterator& operator ++ () {nodeP = (list_nodePtr)nodeP.next; return *this;}
		iterator& operator ++ (int) 
		{
			//��ʼʱ����д�Ĵ����������ģ�������Ϊ��û�и㶮��������ָ�������----����������������ָ�룡��
			//list_nodePtr temp = nodeP;
			//nodeP = (list_nodePtr)nodeP.next;
			//return temp;
			iterator temp = *this;
			//ֱ��������һ���������Ѿ�ʵ�ֵ�++������
			++*this;
			return temp;
		}

		iterator& operator -- () {nodeP = (list_nodePtr)nodeP.prev; return *this;}
		iterator& operator -- (int) {iterator temp = *this; --*this; return temp;}

		reference operator * () {return (*nodeP).data;}
		//�����ǡ���Ա��ȡ�����ӡ��ı�׼д��������
		pointer operator -> () {return &(operator*());}
	};
}
#endif