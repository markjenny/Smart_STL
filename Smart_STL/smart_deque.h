#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include <cstddef>

namespace smart_stl
{
	//������Ҫ���deque���ֵ����壬deque�ı����ʹ�÷�������vector��һ��������������һ�ζ������Ŀռ���ɵģ���������
	//vector��ͬ���������ڡ�ͷβ���Ĵ�ȡЧ�ʸ��ߣ������עͷβ���ֵĻ�������ʹ��deque
	//deque�еĵ������ǱȽ�����ĵ���������ΪҪ���ǵ���һ����������һ���λ�������һ����
	inline size_t deque_buf_size(size_t sz)
	{
		return n != 0 ? n : ( sz > 512 ? size_t(1) : size_t(512 / sz) );
	}

	//�û�Ӧ�ò���ȥ��ÿ��bucket���ж��ٸ�Ԫ�أ�������û������ģ������൱���ڲ��ṹ���û���ʹ�õ�ʱ��Ҳû��ֱ��ȥ
	//�趨ÿ��bucket���ж��ٸ�Ԫ��
	//ʱ�̼�סdequeӵ��һ���п������п����ϵ�ÿ����ָ�򻺳�������������Ĭ������512bytes�Ŀռ䣬�����ص������Ǳ�512bytes��
	//�Ļ����������о�ֻ�洢�������͵�һ������
	template<class T, class Ref, class Ptr>
	struct deque_iterator
	{
		//ƫ�ػ�
		//�������iteratorӦ��������deque�����ݽṹ�õ�
		typedef deque_iterator<T, T&, T*> iterator;
		typedef deque_iterator<T, const T&, const T*> const_iterator;

		static size_t buffer_size() {return deque_buf_size();}

		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef ptrdiff_t distance_type;

		//����Ҫ����������
		typedef size_t size_type;
		typedef T** map_pointer;		//deque�����ر���Ϊmap
		//������ά��deque_iterator�ġ�ԭʼ�С���
		typedef deque_iterator self;

		//deque_iterator��Ҫ����س�Ա
		T* cur;		//��ǰ��������ָ�Ļ������ĵ�ǰ��
		T* first;		//��ǰ��������ָ�Ļ���������ʼ��
		T* last;		//��ǰ��������ָ�Ļ���������ֹ�㣨��������
		map_pointer node;		//�п���map�е�node��


		//������������Ҫ�ĸ��ֲ�����
		//������������Ҫ�ĸ����߼��ȽϷ�
		//�ع�һ��list�еĵ�����������ֻ��operator==��operator��=�����Ǻ����ľ���ṹ�йأ���deque�ı����������롰vector�����ƣ�
		//������һЩ�µ�>��<��<+�Ȳ���
		bool operator == (const self& deq_iter) {return cur == deq_iter.cur;}
		bool operator != (const self& deq_iter);
		bool operator > (const self& deq_iter);
		bool operator >= (const self& deq_iter);
		bool operator < (const self& deq_iter);
		bool operator <= (const self& deq_iter);

		//deque_iterator��Ҫ���ض�������������ת��������set_node
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + distance_type(buffer_size());
		}
	};
}


#endif