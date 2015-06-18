#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include <cstddef>
#include <stdexcept>

namespace smart_stl
{
	//������Ҫ���deque���ֵ����壬deque�ı����ʹ�÷�������vector��һ��������������һ�ζ������Ŀռ���ɵģ���������
	//vector��ͬ���������ڡ�ͷβ���Ĵ�ȡЧ�ʸ��ߣ������עͷβ���ֵĻ�������ʹ��deque
	//deque�еĵ������ǱȽ�����ĵ���������ΪҪ���ǵ���һ����������һ���λ�������һ����
	inline size_t deque_buf_size(size_t sz)
	{
		return sz > 512 ? size_t(1) : size_t(512 / sz) ;
	}

	//�û�Ӧ�ò���ȥ��ÿ��bucket���ж��ٸ�Ԫ�أ�������û������ģ������൱���ڲ��ṹ���û���ʹ�õ�ʱ��Ҳû��ֱ��ȥ
	//�趨ÿ��bucket���ж��ٸ�Ԫ��
	//ʱ�̼�סdequeӵ��һ���п������п����ϵ�ÿ����ָ�򻺳�������������Ĭ������512bytes�Ŀռ䣬�����ص������Ǳ�512bytes��
	//�Ļ����������о�ֻ�洢�������͵�һ������

	//�ܽ�һ��deque���������ص㣬�õ�������finish���ǿ��Ժ���Ԫ�صģ��ڻ���������first��last���б�ǣ���finish����������cur
	//ָ����Ǹû������С�û��Ԫ�صĵ�һ���㡿����start����������curָ����Ǹû������С���һ��Ԫ�ء�
	template<class T, class Ref, class Ptr>
	struct deque_iterator
	{
		//ƫ�ػ�
		//�������iteratorӦ��������deque�����ݽṹ�õ�
		//û�㶮sgi_stl�������ʲô�ã�����
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

		self operator -- (int)
		{
			iterator temp = &this;
			--*this;
			return temp;
		}

		//deque�����������ز���������Ϊ���ӵ�һ��������Ķ����Ը�����������
		self& operator += (distance_type n);
		{
			//����Ӹöλ�������first��ʼƫ�Ƶ�λ��
			distance_type offset = n + (cur - first);
			//�ж����ƫ�Ƶ�λ���Ƿ����ڱ����У���Ϊ��ʼλ������first����ô�����ֻ��ƫ��0��buffer_size�ľ���
			if(offset >= 0 && offset < buffer_size())
				cur = first + offset;
			else
			{
				//����else��ʾҪת�Ƶ���λ�ò���Ŀǰ�����ڵĻ������У��������Ǿ���Ҫ�жϽڵ��ƫ�ƾ���node_offset
				//-offset��������ʵ��offsetȥ����ֵ��������֪������ת�ľ��룬ͬʱ-offset-1�Ϳ������ɽ�Ҫת�Ƶĵ㣬�ŵ�ǰһ����������lastǰ��
				//λ�ã��������ӷ�������ļ���
				distance_type node_offset = 
					offset > 0 ? distance_type(offset / distance_type(buffer_size())) : -distance_type((-offset - 1) / distance_type(buffer_size())) - 1;

				set_node(node + node_offset);

				//��סoffset����firstΪ��㣬�ù�ʽ��node_offsetΪ����ʱ�����㣬ͨ��������Ϊ����ʱҲ������ģ�����ֱ�ӽ������øñ��ʽ���ǿ��Ե�
				cur = first + (offset - node_offset * distance_type(buffer_size()));
			}
			return *this;
		}

		self operator + (distance_type n)
		{
			iterator temp = *this;
			temp+=n;
			return temp;
		}

		self& operator -= (distance_type n)
		{
			*this -= -n;
		}

		self operator - (distance_type n)
		{
			iterator temp = *this;
			temp -= n;
			return temp;
		}

		reference operator * () {return *cur;}
		pointer operator -> () {return &(operator*());}
		reference operator [] (distance_type n) {return *(*this + n);}


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


	//��Ϊ��duque������ݽṹ�У�����Ҫ�õ��������ڲ�ͬ���͵�������������ֱ����alloc����ʹ���Ǹ�ͳһ�Ľӿ�simple_alloc
	//
	template<class T, class Alloc = alloc>
	class duque
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef ptrdiff_t distance_type;

		typedef const T* const_pointer;
		typedef const T& const_reference;

		typedef pointer map_pointer;
		typedef size_t size_type;

		//����map���������ͻ�������������
		typedef simple_alloc<T, Alloc> node_allocator;
		typedef simple_alloc<pointer, Alloc> map_allocator;

		typedef deque_iterator<T, T&, T*> iterator;
		//��ʱ�뵽������������const_iterator�Ŀ��ܣ�Ӧ�������Ǳ��������е����ݵ�ʱ�򣬷��õ�����ȥ�������е�����
		typedef deque_iterator<T, const T&, const T*> const_iterator;


	private:
		iterator start_;
		iterator finish_;

		map_pointer map;
		size_type map_size;

	public:
		/*****�빹�캯�������ƹ��캯����assignment operator�����������������������ϵ�����Ԫ�أ����ҽ�map�����õ��Ļ������ͷţ�***/
		duque( ) : start_(0), finish_(0), map_size(0), map_pointer(0) {}
		explicit deque(size_type n);
		deque(size_type n, const value_type& val);
		duque(const deque& deq);
		deque& operator = (const deque& deq);
		~deque();
		/*********************************************************************************************************************************/


		/*********************************************���߼��Ƚ����*********************************************************************/
		friend bool operator == (const deque& lhs, const deque& rhs);
		friend bool operator != (const deque& lhs, const deque& rhs);
		/*********************************************************************************************************************************/

		/*********************************************����������***********************************************************************/
		iterator begin() {return start_;}
		const_iterator begin() const {return start_;}
		iterator end() {return finish_;}
		const_iterator end() const {return finish_;}
		/*********************************************************************************************************************************/

		/*********************************************���������*************************************************************************/
		size_type size() {return finish_ - start_;}
		bool empty() {return finish_ == start_;}
		size_type max_size() {return size_type(-1)}
		/*********************************************************************************************************************************/

		/*********************************************�����Ԫ�����*********************************************************************/
		reference front() {return *start_;}
		reference back() {return *(finish_ - 1);}
		const_reference front() const {return *start_;}
		const_reference back() const {return *(finish_ - 1);}
		//����һ�����õ�Ԫ����ָ����λ��pos��û��ִ�б߽���
		reference operator [] (size_type n)
		{
			return start_[distance_type(n)];
		}
		const_reference operator [] (size_type n) const
		{
			return start_[distance_type(n)];
		}

		reference at(size_type pos)
		{
			if (pos >= size())
				std::_Throw std:out_of_range();
			else
			{
				return start_[distance_type(n)];
			}
			
		}
		/*********************************************************************************************************************************/

		/*********************************************��ı��������**********************************************************************/
		void clear();
		iterator insert(iterator position, const value_type& val);
		void insert(iterator position, size_type n, const value_type& val);
		template<class InputIterator>
		void insert(iterator position, InputIterator first, InoputIterator last);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

		void resize(size_type n, const value_type& val);
		void swap(const deque& deq);
		void push_back(const  value_type& val);
		void push_front(const value_type& val);
		void pop_back();
		void pop_front();
		/*********************************************************************************************************************************/
	};
}


#endif