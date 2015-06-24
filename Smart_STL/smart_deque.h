#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"
#include "smart_uninitialized.h" 
#include "smart_construct.h"
#include <cstddef>
#include <stdexcept>
#include <algorithm>

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

		static size_t buffer_size() {return deque_buf_size(sizeof(value_type));}

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

		deque_iterator(const self& deq_iter) : cur(deq_iter.cur), first(deq_iter.first), last(deq_iter.last), node(deq_iter.node) {}

		self& operator = (const self& deq_iter) 
		{
			cur = deq_iter.cur;
			first = deq_iter.first;
			last = deq_iter.last;
			node = deq_iter.node;
			return *this;
		}
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
			{
				set_node(node + 1);
				cur = first;
			}
			return *this;
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
		self& operator += (distance_type n)
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

		reference operator * () 
		{
			return *cur;
		}
		pointer operator -> () {return &(operator*());}
		reference operator [] (distance_type n) {return *(*this + n);}


		/*************************************************************************************************************************/

		//deque_iterator��Ҫ���ض�������������ת��������set_node
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + distance_type(deque_buf_size(sizeof(value_type)));
			//Ϊ�˱��������Ǹ���cur���ҽ�cur���ó�NULL�������ڶԡ�û�и�ֵ��cur�����е��õ�ʱ��ͻ�������������Ϳ��Է���û��
			//��cur���и�ֵ�������
			cur = NULL;
		}
	};


	//��Ϊ��duque������ݽṹ�У�����Ҫ�õ��������ڲ�ͬ���͵�������������ֱ����alloc����ʹ���Ǹ�ͳһ�Ľӿ�simple_alloc
	//
	template<class T, class Alloc = alloc>
	class deque
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef ptrdiff_t distance_type;

		typedef const T* const_pointer;
		typedef const T& const_reference;

		typedef pointer* map_pointer;
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
		//start_��finish_�Դ��ĸ�ֵ���캯������������ص��趨
		deque( ) : start_(), finish_(), map_size(0), map_pointer(0) { fill_initialize(0, T()); }
		explicit deque(size_type n) : start_(), finish_(), map(0), map_size(0) {fill_initialize(n, T());}
		//һ��Ҫ��is_Integer����������������������������   
		//deque(size_type n, const value_type& val):start_(), finish_(), map(0), map_size(0) {fill_initialize(n, val);}
		deque(size_type n, const value_type& val);
		template<class InputIterator>
		deque(InputIterator first, InputIterator last);
		deque(const deque& deq);
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
		void insert(iterator position, InputIterator first, InputIterator last);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

		void resize(size_type n, const value_type& val);
		void swap(const deque& deq);
		void push_back(const  value_type& val);
		void push_front(const value_type& val);
		void pop_back();
		void pop_front();
		/*********************************************************************************************************************************/


		/*****************************private����*****************************************************************************************/
		private:
			enum {INITIAL_MAP_SIZE = 8};
			//���������Ĺ��캯��Ҳ��Ҫʹ����������ģ�ֻҪ������n����Ϊ0�ͺ���
			void fill_initialize(size_type n, const value_type& val);
			void create_map_and_nodes(size_type num_elements);
			//Ϊÿ��map�еĵ���仺����
			pointer allocate_node();
			template<class Integer>
			void deque_aux(Integer n, const value_type& val, _true_type);
			template<class InputIterator>
			void deque_aux(InputIterator first, InputIterator last, _false_type);
			//�ͷŻ�����
			void deallocate_node(pointer p);
			//�ͷ��п���
			void deallocate_map(map_pointer mp);
	};

	/*****************************************************��deque�����ʵ�֡�************************************************************/

	//deque(size_type n, const value_type& val);
	template<class T, class Alloc>
	deque<T, Alloc>::deque(size_type n, const value_type& val)
	{
		typedef typename is_Integer<size_type>::class_type is_Int;
		deque_aux(n, val, is_Int());
	}

	template<class T, class Alloc>
	template<class InputIterator>
	deque<T, Alloc>::deque(InputIterator first, InputIterator last)
	{
		typedef typename is_Integer<InputIterator>::class_type is_Int;
		deque_aux(first, last, is_Int());
	}

	template<class T, class Alloc>
	deque<T, Alloc>::deque(const deque& deq)
	{
		size_type new_sz = deq.finish_ - deq.start_;
		create_map_and_nodes(new_sz);
		iterator temp = start_;
		uninitialized_copy(deq.start_, deq.finish_, start_);
	}

	template<class T, class Alloc>
	deque<T, Alloc>& deque<T, Alloc>::operator = (const deque& deq)
	{
		size_type new_sz = finish_ - start_;
		create_map_and_nodes(new_sz);
		iterator temp = start_;
		uninitialized_copy(deq.start_, deq.finish_, start_);
		return *this;
	}

	template<class T, class Alloc>
	deque<T, Alloc>::~deque()
	{
		//����deque��������к���
		destroy(start_, finish_);
		//�ͷŸ����ڵ��ϵĻ�����
		for (map_pointer temp = start_.node; temp != finish_.node; temp++)
		{
			deallocate_node(*temp);
		}
		deallocate_node(*finish_.node);
		//�ͷ��п���map
		deallocate_map(map);
	}

	template<class T, class Alloc>
	bool operator == (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs)
	{
		const_iterator tempLhs = lhs.start_;
		const_iterator tempRhs = rhs.start_;

		for (; tempLhs != lhs.finish_ && tempRhs != rhs.finish_; tempLhs++, tempRhs++)
		{
			if (*tempLhs != *tempRhs)
				return false;
		}

		if(tempLhs != lhs.finish_ || tempRhs != rhs.finish_)
			return false;

		return true;
	}

	template<class T, class Alloc>
	bool operator != (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::fill_initialize(size_type n, const value_type& val)
	{
		//�����±��ʱ�򣬶�start_��finish_�Ѿ���������
		create_map_and_nodes(n);
		map_pointer cur;
		//����map���л������ĵ�
		for (cur = start_.node; cur < finish_.node; cur++)
			uninitialized_fill(*cur, *(cur + deque_buf_size(sizeof(value_type))), val);

		uninitialized_fill(finish_.first, finish_.cur, val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::create_map_and_nodes(size_type num_elements)
	{
		size_type num_nodes = num_elements / deque_buf_size(sizeof(value_type)) + 1;

		map_size = max(INITIAL_MAP_SIZE, num_nodes + 2);
		map = map_allocator::allocate(map_size);

		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur = nstart;
		for (; cur <= nfinish; cur++)
		{
			*cur = allocate_node();
		}

		start_.set_node(nstart);
		finish_.set_node(nfinish);
		start_.cur = start_.first;
		finish_.cur = finish_.first + num_elements % (deque_buf_size(sizeof(value_type)));
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::pointer deque<T, Alloc>::allocate_node()
	{
		return node_allocator::allocate(deque_buf_size(sizeof(value_type)));
	}

	template<class T, class Alloc>
	template<class Integer>
	void deque<T, Alloc>::deque_aux(Integer n, const value_type& val, _true_type)
	{
		fill_initialize(n, val);
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void deque<T, Alloc>::deque_aux(InputIterator first, InputIterator last, _false_type)
	{
		size_type new_sz = last - first;
		create_map_and_nodes(new_sz);
		iterator temp = start_;
		uninitialized_copy(first, last, start_);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::deallocate_node(pointer p)
	{
		node_allocator::deallocate(p, deque_buf_size(sizeof(value_type)));
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::deallocate_map(map_pointer mp)
	{
		map_allocator::deallocate(mp, map_size);
	}

}


#endif