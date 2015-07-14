#ifndef _SMART_HEAP_H_
#define _SMART_HEAP_H_
#include "smart_iterator_base.h"

namespace smart_stl
{

	//samrt_heap���ĸ����ܣ�push_heap��pop_heap��make_heap��sort_heap
	//heap����ؽṹʹ�������涨��1��first~last֮����heap�ĵײ�������2��lastǰ��Ԫ���Ǵ������Ԫ��

	//�һ�û�и����Ϊʲô���ģ��Ҫ���Ԫ�ص����ͣ�ֱ����*��last-1������������
	template<class RandomAccessIterator, class Distance, class T>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, const T& val)
	{
		//���Ҳ���ǡ��㷨4���е�swim�����ı��Σ����������swim��Ч�ʻ���Ҫ����һ�㣬��û�н��н���
		//�����ǽ����ڵ�ֵ����holeIndex��ʡȥ��һ�εĸ�ֵ�¼����ȽϾ��ʣ�������
		T val_copy = val;
		Distance parent = (holeIndex - 1)/2;
		while(holeIndex > topIndex && *(first + parent) < val_copy)
		{
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1)/2;
		}
		*(first + holeIndex) = val_copy;
	}

	template<class RandomAccessIterator, class Distance, class T>
	inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
	{
		//�������ȷ���������ڽ��е�����������ʱ�������ȷʵ��distance_type�����ͣ����Ǻ����ּ�ȥ1���������ܻ������������������
		//������distance_type()����ǿ��ת��
		_push_heap(first, Distance(first - last - 1), Distance(0), T(*(last - 1)));
	}


	template<class RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		//������õļ��������ʹ���
		push_heap_aux(first, last, distance_type(first), value_type(first));
	}

	//=======================Ĭ�ϵ�push_heap��max-heap������ӿ����Զ�����������============================
	template<class RandomAccessIterator, class Distance, class T, class Compare comp>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, const T& val, Compare comp)
	{
		//���Ҳ���ǡ��㷨4���е�swim�����ı��Σ����������swim��Ч�ʻ���Ҫ����һ�㣬��û�н��н���
		//�����ǽ����ڵ�ֵ����holeIndex��ʡȥ��һ�εĸ�ֵ���ȽϾ��ʣ�������
		T val_copy = val;
		Distance parent = (holeIndex - 1)/2;
		while(holeIndex > topIndex && comp(*(first + parent), val_copy))
		{
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1)/2;
		}
		*(first + holeIndex) = val_copy;
	}

	template<class RandomAccessIterator, class Distance, class T, class Compare>
	inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
	{
		//�������ȷ���������ڽ��е�����������ʱ�������ȷʵ��distance_type�����ͣ����Ǻ����ּ�ȥ1���������ܻ������������������
		//������distance_type()����ǿ��ת��
		_push_heap(first, Distance(first - last - 1), Distance(0), T(*(last - 1)), comp);
	}


	template<class RandomAccessIterator, class Compare>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		//������õļ��������ʹ���
		push_heap_aux(first, last, distance_type(first), value_type(first), comp);
	}




	//stl�е�pop_heap������û�н��Ѷ���Ԫ��ȡ����������ֱ�ӷ����˵ײ�����(vector)��ĩβ������Ҫ��֪Ԫ�ػ���
	//Ҫʹ��back()��ȡ��Ԫ�ؾ�ʹ��pop_back()�����ǲ������ã����ò��ܱ�֤pop_heap�Ľ�׳
	
	//len�Ǳ�ʾ�����һ��Ԫ�ص�λ�ã������±꣩��ע��len��ʵ������������һ��Ԫ�ص��±�
	template<class RandomAccessIterator, class Distance, class T>
	void _pop_heap(RandomAccessIterator first, Distance len, Distance holeIndex, const T& value)
	{
		//��������������ǵ����ѣ�adjust
		*(first + len) = *(first);
		//��Ϊ����Ҫ�õ����ֵ�������ȱ��棬����holeIndex������仯
		Distance topIndex = holeIndex;
		Distance childIndex = 2*holeIndex + 1;
		while(childIndex <= len - 1)
		{
			if(childIndex < len - 1 && *(first + childIndex) < *(first + len +1))
				childIndex++;
			*(first + holeIndex) = *(first + childIndex);
			holeIndex = childIndex;
			childIndex = 2 * holeIndex + 1;
		}

		//����ѭ�������õ�hole�����Ѿ�û���ӽڵ��ˣ����Ѿ���Ҷ�ӽڵ��ˣ����ڿ�ʼswim
		_push_heap(first, holeIndex, , value);
	}

	template<class RandomAccessIterator, class Distance, class T>
	inline void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
	{
		_pop_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)));
	}

	//[first,last)��������Ǵ��������������
	template<class RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		pop_heap_aux(first, last, distance_type(first), value_type(first));
	}


	//=======================Ĭ�ϵ�pop_heap��max-heap������ӿ����Զ�����������============================
	template<class RandomAccessIterator, class Distance, class T, class Compare>
	void _pop_heap(RandomAccessIterator first, Distance len, Distance holeIndex, const T& value, Compare comp)
	{
		//��������������ǵ����ѣ�adjust
		*(first + len) = *(first);
		Distance childIndex = 2*holeIndex + 1;
		//��Ϊ����Ҫ�õ����ֵ�������ȱ��棬����holeIndex������仯
		Distance topIndex = holeIndex;
		while(childIndex <= len - 1)
		{
			if(childIndex < len - 1 && comp(*(first + childIndex), *(first + childIndex + 1)))
				childIndex++;
			*(first + holeIndex) = *(first + childIndex);
			holeIndex = childIndex;
			childIndex = 2 * holeIndex + 1;
		}

		//����ѭ�������õ�hole�����Ѿ�û���ӽڵ��ˣ����Ѿ���Ҷ�ӽڵ��ˣ����ڿ�ʼswim
		_push_heap(first, holeIndex, topIndex, value, comp);
	}

	template<class RandomAccessIterator, class Distance, class T, class Compare>
	inline void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
	{
		_pop_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)), comp);
	}

	//[first,last)��������Ǵ��������������
	template<class RandomAccessIterator, class Compare>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		pop_heap_aux(first, last, distance_type(first), value_type(first), comp);
	}

}


#endif	_SMART_HEAP_H_