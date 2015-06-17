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

		/*****************************************������صĹ��캯�������ƹ��캯����assignment operator***********************/
		deque_iterator() : cur(0), first(0), last(0), node(0) {}

		deque_iterator(pointer x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) {}

		deque_iterator(const self& deq_iter) : cur(deq_iter.cur), first(deq_iter.first), last(deq_iter.last), node(deq_iter->node) {}

		self& operator = (const self& deq_iter) : cur(deq_iter.cur), first(deq_iter.first), last(deq_iter.last), node(deq_iter.node) {}
		/*************************************************************************************************************************/

		/****************************************������������Ҫ�ĸ��ֲ�����********************************************************/
		//������������Ҫ�ĸ����߼��ȽϷ�
		//�ع�һ��list�еĵ�����������ֻ��operator==��operator��=�����Ǻ����ľ���ṹ�йأ���deque�ı����������롰vector�����ƣ�
		//������һЩ�µ�>��<��<+�Ȳ���
		bool operator == (const self& deq_iter) {return cur == deq_iter.cur;}
		bool operator != (const self& deq_iter) 
		{
			//tiny_stl�ܵ���Դ����operator<��Ӱ�죬����ֻ�Ƚ�deq_iter�Ϳ��ԣ���Ϊ�������е�ÿ�����cur���ǲ�ͬ�ģ���ʹ
			//�ǲ�ͬ��map_pointer����Ϊ���Ǵ��ڴ�ռ��з����ȥ��
			return !(*this == deq_iter);
		}
		bool operator > (const self& deq_iter)
		{
			return  node == deq_iter.node ? cur > deq_iter.cur : node > deq_iter.node ; 
		}

		bool operator <= (const self& deq_iter)
		{
			return !( *this > deq_iter );
		}

		bool operator < (const self& deq_iter)
		{
			return !(deq_iter > *this);
		}

		bool operator >= (const self& deq_iter)
		{
			return !(*this < deq_iter);
		}
		/*************************************************************************************************************************/

		/***********************************************��ص����ز�����*********************************************************/
		distance_type operator - ( const self& deq_iter )
		{
			//������Ҫ˵����һ��������������*this��deq_iter֮��ľ��룬��������*this��deq_iter֮���ж��ٸ����ݣ��������
			//����֮���ж��ٸ����ݣ���ôӦ����(cur-first)֮�����1����������Ǿ��룬���������ֱ��д��(cur-first)
			return (distance_type(buffer_size()) * (node - deq_iter.node - 1) + (cur - first) + (deq_iter.last - deq_iter.cur));
		}

		self& operator ++ ()
		{
			cur++;
			if (cur == last)
				set_node(node + 1);

			cur = first;
		}
		
		self operator ++ (int)
		{
			iterator temp = *this;
			++*this;
			return temp;
		}
		self& operator -- ()
		{
			if (cur == first)
			{
				set_node(node - 1);
				cur = last;
			}

			cur--;
			return *this;
		}
		self operator -- (int);
		self& operator += (distance_type n);
		self operator + (distance_type n);
		self& operator -= (distance_type n);
		self operator - (distance_type n);
		reference operator * ();
		pointer operator -> ();
		reference operator [] (distance_type n);


		/*************************************************************************************************************************/

		//deque_iterator��Ҫ���ض�������������ת��������set_node
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + distance_type(buffer_size());
			//Ϊ�˱��������Ǹ���cur���ҽ�cur���ó�NULL�������ڶԡ�û�и�ֵ��cur�����е��õ�ʱ��ͻ�������������Ϳ��Է���û��
			//��cur���и�ֵ�������
			cur = NULL;
		}
	};
}


#endif