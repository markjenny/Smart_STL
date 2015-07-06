#ifndef _SMART_DEQUE_H_ 
#define _SMART_DEQUE_H_
#include "smart_alloc.h"
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_type_traits.h"
#include "smart_uninitialized.h" 
#include "smart_construct.h"
#include "smart_algorithm.h"
#include <cstddef>
#include <stdexcept>
#include <algorithm>

namespace smart_stl
{
	//������Ҫ����deque���ֵ����壬deque�ı����ʹ�÷�������vector��һ��������������һ�ζ������Ŀռ���ɵģ���������
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

		deque_iterator(const iterator& deq_iter) : cur(deq_iter.cur), first(deq_iter.first), last(deq_iter.last), node(deq_iter.node) {}

		self& operator = (const iterator& deq_iter) 
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
		bool operator == (const iterator& deq_iter) {return cur == deq_iter.cur;}
		bool operator != (const iterator& deq_iter) 
		{
			//tiny_stl�ܵ���Դ����operator<��Ӱ�죬����ֻ�Ƚ�deq_iter�Ϳ��ԣ���Ϊ�������е�ÿ�����cur���ǲ�ͬ�ģ���ʹ
			//�ǲ�ͬ��map_pointer����Ϊ���Ǵ��ڴ�ռ��з����ȥ��
			return !(*this == deq_iter);
		}
		bool operator > (const iterator& deq_iter)
		{
			return  node == deq_iter.node ? cur > deq_iter.cur : node > deq_iter.node ; 
		}

		bool operator <= (const iterator& deq_iter)
		{
			return !( *this > deq_iter );
		}

		bool operator < (const iterator& deq_iter)
		{
			return !(deq_iter > *this);
		}

		bool operator >= (const iterator& deq_iter)
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
			iterator temp = *this;
			--*this;
			return temp;
		}

		//deque�����������ز���������Ϊ���ӵ�һ��������Ķ����Ը�����������
		self& operator += (distance_type n)
		{
			//����Ӹöλ�������first��ʼƫ�Ƶ�λ��
			distance_type offset = n + (cur - first);
			//�ж����ƫ�Ƶ�λ���Ƿ����ڱ����У���Ϊ��ʼλ������first����ô�����ֻ��ƫ��0��buffer_size�ľ���
			if(offset >= 0 && offset < (distance_type)buffer_size())
				cur = first + offset;
			else
			{
				//����else��ʾҪת�Ƶ���λ�ò���Ŀǰ�����ڵĻ������У��������Ǿ���Ҫ�жϽڵ��ƫ�ƾ���node_offset
				//-offset��������ʵ��offsetȥ����ֵ��������֪������ת�ľ��룬ͬʱ-offset-1�Ϳ�������ɽ�Ҫת�Ƶĵ㣬�ŵ�ǰһ����������lastǰ��
				//λ�ã��������ӷ�������ļ���
				distance_type node_offset = 
					offset > 0 ? distance_type(offset / distance_type(buffer_size())) : -distance_type((-offset - 1) / distance_type(buffer_size())) - 1;

				set_node(node + node_offset);

				//��סoffset����firstΪ��㣬�ù�ʽ��node_offsetΪ����ʱ�����㣬ͨ��������Ϊ����ʱҲ������ģ�����ֱ�ӽ������øñ���ʽ���ǿ��Ե�
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
			return *this += -n;
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
			//�������Ҷ����set_node���������������Ŀ��Ӧ���ǽ��趨node��˳���޶���first��last�������cur�趨��NULL�������reallocate_mapʹ�þͲ��Ǻ�˳��
			//cur = NULL;
		}

		template<class T, class Ref, class Ptr>
		friend deque_iterator<T, Ref, Ptr> operator + (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n);
		template<class T, class Ref, class Ptr>
		friend deque_iterator<T, Ref, Ptr> operator + (typename deque_iterator<T, Ref, Ptr>::distance_type n, const deque_iterator<T, Ref, Ptr>& it);
		template<class T, class Ref, class Ptr>
		friend deque_iterator<T, Ref, Ptr> operator - (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n);
		template<class T, class Ref, class Ptr>
		friend typename deque_iterator<T, Ref, Ptr>::distance_type operator - (const deque_iterator<T, Ref, Ptr>& it1, const deque_iterator<T, Ref, Ptr>& it2);
	};

	template<class T, class Ref, class Ptr>
	deque_iterator<T, Ref, Ptr> operator + (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n)
	{
		//�����������const���͵ģ�����Ҫ����һ��operator+
		//using deque_iterator<T, Ref, Ptr>;
		deque_iterator<T, Ref, Ptr> temp(it);
		deque_iterator<T, Ref, Ptr>::distance_type offset = (temp.cur - temp.first) + n;
		deque_iterator<T, Ref, Ptr>::distance_type nodePageSize = deque_iterator<T, Ref, Ptr>::distance_type(deque_buf_size(sizeof(typename deque_iterator<T, Ref, Ptr>::value_type)));
		//��ͬһ��bucket��
		if(offset >=0 && offset < nodePageSize)
			temp.cur += n;
		else
		{
			//�ֳ����������͸�����
			deque_iterator<T, Ref, Ptr>::distance_type offset_node = 
				offset > 0 ? offset / nodePageSize
				: -deque_iterator<T, Ref, Ptr>::distance_type((-offset -1) / nodePageSize) - 1;
			deque_iterator<T, Ref, Ptr>::set_node(offset_node);
			temp.cur = temp.first + offset - offset_node * nodePageSize;
		}
		return temp;
	}

	template<class T, class Ref, class Ptr>
	deque_iterator<T, Ref, Ptr> operator + (typename deque_iterator<T, Ref, Ptr>::distance_type n, const deque_iterator<T, Ref, Ptr>& it)
	{
		return it + n;
	}

	template<class T, class Ref, class Ptr>
	deque_iterator<T, Ref, Ptr> operator - (const deque_iterator<T, Ref, Ptr>& it, typename deque_iterator<T, Ref, Ptr>::distance_type n)
	{
		return it + (-n);
	}

	template<class T, class Ref, class Ptr>
	typename deque_iterator<T, Ref, Ptr>::distance_type operator - (const deque_iterator<T, Ref, Ptr>& it1, const deque_iterator<T, Ref, Ptr>& it2)
	{
		typedef deque_iterator<T, Ref, Ptr>::distance_type distance_type;
		typedef deque_iterator<T, Ref, Ptr>::value_type value_type;
		return distance_type( ( it1.cur - it1.first ) + (it1.node - it2.node - 1) * distance_type(deque_buf_size(sizeof(value_type))) + ( it2.last - it2.cur ) );
	}


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
		template<class T, class Alloc>
		friend bool operator == (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
		template<class T, class Alloc>
		friend bool operator != (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
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
		size_type max_size() {return size_type(-1);}
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
			void push_back_aux(const value_type& val);
			void reserve_map_back(size_type nodes_to_add = 1);
			void reallocate_map(size_type nodes_to_add, bool add_at_front);
			void push_front_aux(const value_type& val);
			void reserve_map_front(size_type nodes_to_add = 1);
			void pop_back_aux();
			void pop_front_aux();
			iterator insert_aux(iterator position, const value_type& val);

			void insert_aux(iterator position, size_type n, const value_type& val, _true_type);
			template<class InputIterator>
			void inser_aux(iterator position, iterator first, iterator last, _false_type);

			iterator reserve_elements_at_front(size_type n);
			iterator reserve_elements_at_back(size_type);

			void insert_at_the_middle(iterator position, size_type n, const value_type& val);

			void reserve_new_elements_at_front(size_type n);
			void reserve_new_elements_at_back(size_type n);
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
		distance_type new_elements = deq.finish_ - deq.start_;
		create_map_and_nodes(new_elements);
		iterator temp1 = start_;
		for (iterator temp2 = deq.start_; temp2 != deq.finish_; temp2++)
		{
			construct(&*temp2, *temp1);
			temp1++;
		}
	}

	template<class T, class Alloc>
	deque<T, Alloc>& deque<T, Alloc>::operator = (const deque& deq)
	{
		distance_type new_elements = deq.finish_ - deq.start_;
		create_map_and_nodes(new_elements);
		iterator temp1 = start_;
		for (iterator temp2 = deq.start_; temp2 != deq.finish_; temp2++)
		{
			construct(&*temp2, *temp1);
			temp1++;
		}
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


	/*********************************************��ı��������**********************************************************************/
	//deque��clear��Ҫ����ȫ����Ԫ�أ����Ǳ���һ��������

	template<class T, class Alloc>
	void deque<T, Alloc>::clear()
	{
		//���ͷβ�ڵ�����Ļ�����(��Ϊ��д��������һ��������)
		for (map_pointer nodeTemp = start_.node + 1; nodeTemp < finish_.node; nodeTemp++)
		{
			//�Ի������е�Ԫ�ؽ�������
			destroy(*nodeTemp, *nodeTemp + (distance_type)deque_buf_size(sizeof(value_type)));
			node_allocator::deallocate(*nodeTemp, (distance_type)deque_buf_size(sizeof(value_type)));
		}
		//ͷβ���л����������Ǿ�Ҫ����һ��������
		if (start_.node != finish_.node)
		{
			destroy(start_.cur, start_.last);
			destroy(finish_.first, finish_.cur);
			node_allocator::deallocate(*finish_.node, (distance_type)deque_buf_size(sizeof(value_type)));
		}
		else
			//ֻ��һ����������ʱ��Ҫע���ˣ���start.cur��ffinish_.cur����
			destroy(start_.cur, finish_.cur);
		//start_.cur��������start��״̬�����Բ��ý������·���cur
		finish_ = start_;
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::insert(iterator position, const value_type& val)
	{
		if (position.cur == start_.cur)
		{
			push_front(val);
			return start_;
		}
		else if (position.cur == finish_.cur)
		{
			push_back(val);
			iterator temp = finish_;
			temp--;
			return temp;
		}
		else
			return insert_aux(position, val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
	{
		typename typedef is_Integer<size_type>::class_type is_Int;
		insert_aux(position, n, val, is_Int());
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void deque<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
	{
		typename typedef is_Integer<InputIterator>::class_type is_Int;
		insert_aux(position, first, last, is_Int());
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator position)
	{
		//next��������ʵ����Ϊ�˷��ص�����������Ϊ��copyԪ�أ���Ϊ����ֻ��Ҫ�ƶ�������Ԫ�أ���Ͳ�֪���Ǳߵĵ�������ʧЧ
		iterator next = position;
		next++;

		//ǰ�벿�ֵ�Ԫ��
		distance_type index = position - start_;

		if(index < size() >> 1)
		{
			//��ʾǰ���Ԫ�ؽ��٣������ƶ�ǰ���Ԫ��
			copy_backward(start_, position, next);
			pop_front();
		}
		else
		{
			//��벿�ֵĵ�Ƚ��٣���������ƶ���벿�ֵĵ�
			copy(next, finish_, position);
			pop_back();
		}
		//������Ҫע��һ�£���Ϊ���ǲ�֪�����ƶ���ǰ���Ԫ�ػ��Ǻ����Ԫ�أ����Ե�������ʧЧ����յ��������Ǵ�start���ĵ�����
		return start_ + index;
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator first, iterator last)
	{
		size_type n = last - first;
		if (first == start_ && last == finish_)
		{
			clear();
			return finish_;
		}
		else
		{
			//�����Ƴ�[first, last)֮��ǰ���Ԫ��
			size_type elements_before = first - start_;
			if (elements_before < (finish_ - start_ - n) >> 1)
			{
				//ǰ��Ԫ�رȽ���
				copy_backward(start_, first, last);
				iterator new_start_ = start_ + n;
				destroy(start_, new_start_);				//����Ԫ�أ�

				//�ͷŻ�����
				for (map_pointer temp = start_.node; temp < new_start_.node; temp++)
					deallocate_node(*temp);

				start_ = new_start_;
			}
			else
			{
				//��벿�ֵ�Ԫ�رȽ���
				copy(last, finish_, first);
				iterator new_finish_ = finish_ - n;
				destroy(new_finish_, finish_);				//����Ԫ�أ�
				
				for(map_pointer temp = finish_.node; temp > new_finish_.node; temp--)
					deallocate_node(*temp);

				finish_ = new_finish_;
				
			}
		}
		return start_ + n;
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::resize(size_type n, const value_type& val)
	{
		if (n < size())
			return;
		//���ò��������
		size_type new_n = n - size();
		insert(finish_, new_n, val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::swap(const deque& deq)
	{
		smart_stl::swap(map, deq.map);
		smart_stl::swap(map_size, deq.map_size);
		smart_stl::swap(start_, deq.start_);
		smart_stl::swap(finish_, deq.finish_);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::push_back(const  value_type& val)
	{
		//���ȼ��finish_��Ӧ�Ļ��������Ƿ������һ��Ԫ�أ���������һ��Ԫ�صĻ���ô����Ҫ�������û���������һ��map_node
		if(finish_.cur != finish_.last - 1)
		{
			//��Ϊ��uninitialized memory������Ҫ����construct����Ϊ��ָ�룬���Բ���new placement�ķ���
			construct(finish_.cur, val);
			++finish_.cur;
		}
		else
			push_back_aux(val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::push_front(const value_type& val)
	{
		//����Ϊʲô��push_front��ʱ��ֻ�п�����������һ������Ԫ�أ�����push_back��ʱ��Ҫ������������Ԫ�أ���Ϊfinish�е�cur�ǿյģ�һֱҪ�����ǿյ�
		//ֻҪ��һ������Ԫ�ؾͿ��Է���
		if(start_.cur != start_.first)
		{
			construct(start_.cur - 1, val);
			--start_.cur;
		}
		else
			//û�б��ÿռ��ˣ���Ҫ����ʹ�ÿռ�
			push_front_aux(val);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::pop_back()
	{
		if (finish_.cur != finish_.first)
		{
			//�����������л�������һ��Ԫ��
			--(finish_.cur);
			destroy(finish_.cur);
		}
		else
			pop_back_aux();
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::pop_front()
	{
		//������������������Ԫ��
		if (start_.cur != start_.last - 1)
		{
			destroy(start_.cur);
			(start_.cur)++;
		}
		else
			pop_front_aux();
	}
	/*********************************************************************************************************************************/


	template<class T, class Alloc>
	bool operator == (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs)
	{
		typename deque<T, Alloc>::iterator tempLhs = lhs.start_;
		typename deque<T, Alloc>::iterator tempRhs = rhs.start_;

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
		//�����±���ʱ�򣬶�start_��finish_�Ѿ���������
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

	template<class T, class Alloc>
	void deque<T, Alloc>::push_back_aux(const value_type& val)
	{
		//�鿴�Ƿ��б�Ҫ��������map�пص�����·���map��
		reserve_map_back();
		//�����µĻ�����
		*(finish_.node + 1) = allocate_node();
		try
		{
			//�쳣����
			construct(finish_.cur, val);
			finish_.set_node(finish_.node + 1);
			finish_.cur = finish_.first;
		}
		catch(...)
		{
			node_allocator::deallocate(*(finish_.node + 1));
			//��ȫ������Ҫ�׳��쳣
			throw;
		}
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reserve_map_back(size_type nodes_to_add = 1)
	{
		if ( (finish_.node - map +1) + nodes_to_add > map_size)
			reallocate_map(nodes_to_add, false);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reallocate_map(size_type nodes_to_add, bool add_at_front)
	{
		//����int**�ᵼ�±���ʱ����
		//ȥ����reallocate_map�������ã����������Ǹı�map�����ǲ��ı仺������ֻ�Ǹı�map�������еĸ�֣���
		size_type old_node_size = finish_.node - start_.node + 1;
		size_type new_node_size = old_node_size + nodes_to_add;
		//�ж��Ƿ����㹻�Ŀռ䣬���У���ô�����½��ռ�
		map_pointer new_nstart_;
		if (map_size > 2 * new_node_size)
		{
			new_nstart_ = map + (map_size - new_node_size) / 2 +
				(add_at_front ? nodes_to_add : 0);

			if (new_nstart_ < start_.node)
				for (map_pointer temp = new_nstart_, temp1 = start_.node; temp1 != finish_.node + 1; temp++, temp1++ )
					*temp = *temp1;
				//smart_stl::copy(start_.node, finish_.node + 1, new_nstart_);
			else
				//smart_stl::copy_backward(start_.node, finish_.node + 1, new_nstart_);
				for (map_pointer temp = new_nstart_ + old_node_size - 1, temp1 = finish_.node; temp1 != start_.node - 1; temp--, temp1--)
					*temp = *temp1;
		}
		else
		{
			size_type new_map_size = map_size + max(map_size, new_node_size) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);

			new_nstart_ = new_map + (new_map_size - new_node_size) / 2 + 
				(add_at_front ? nodes_to_add : 0);
			//��ԭmap�����ݿ�������
			//copy(start_.node, finish_.node + 1, new_nstart_);
			for (map_pointer temp = new_nstart_, temp1 = start_.node; temp1 != finish_.node + 1; temp++, temp1++ )
				*temp = *temp1;
			//����ԭ����map�п���
			//ע������ֱ���ͷ����ݾͿ����ˣ���Ϊ��ŵ���16�������������ڶ������Բ�������
			map_allocator::deallocate(map, map_size);

			map = new_map;
			map_size = new_map_size;
		}

		//�����趨start_��finish_��node����Ϊnode�Ѿ����·����ˣ�����start_��finish_��node��Ӧ�ĵط��Ѿ������仯�ˣ�����Ҫ���·���
		start_.set_node(new_nstart_);
		finish_.set_node(new_nstart_ + old_node_size - 1);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::push_front_aux(const value_type& val)
	{
		reserve_map_front();
		*(start_.node - 1) = allocate_node();

		try
		{
			start_.set_node(start_.node - 1);
			start_.cur = start_.last - 1;
			construct(start_.cur, val);
		}
		catch(...)
		{
			//commit or rollback
			start_.set_node(start_.node + 1);
			start_.cur = start_.first;
			deallocate_node(*(start_.node - 1));
			throw;
		}
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reserve_map_front(size_type nodes_to_add  = 1 )
	{
		if (start_.node - map < (distance_type)nodes_to_add)
			reallocate_map(nodes_to_add, true);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::pop_back_aux()
	{
		//finish_�л�����û��Ԫ��
		deallocate_node(finish_.first);
		finish_.set_node(finish_.node - 1);
		finish_.cur = finish_.last - 1;
		destroy(finish_.cur);
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::pop_front_aux()
	{
		destroy(start_.cur);
		start_.set_node(start_.node + 1);
		start_.cur = start_.first;
		node_allocator(*(start_.node - 1));
	}

	//������deque�м����Ԫ��
	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::insert_aux(iterator position, const value_type& val)
	{
		size_type n = position - start_;
		if(n < size() >> 1)
		{
			//push_front���ܻᵽ����positionʧЧ
			push_front(front());
			//positionǰ��Ԫ�رȽ��٣��ƶ��ⲿ��Ԫ��
			iterator front1 = start_;
			front1++;
			iterator front2 = front1;
			front2++;
			position = start_ + n;
			iterator position1 = position;
			position1++;

			copy(front2, position1, front1);
		}
		else
		{
			//position�����Ԫ�ظ��٣��ƶ���벿��Ԫ��
			//push_back���ܻᵼ��positionʧЧ
			push_back(back());
			iterator back1 = finish_;
			--back1;
			iterator back2 = back1;
			--back2;
			//��Ϊposition���ܻ�ʧЧ���������¶���
			position = start_ + n;
			copy_backward(position, back2, back1);
		}
		*position = val;
		return position;
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::insert_aux(iterator position, size_type n, const value_type& val, _true_type)
	{
		//����n��Ԫ�����õķ��������һ������ͬ���ˣ���ͬ�ĵط��������ƶ�Ԫ�ص�ʱ�򣬲�������˼����Ȼ�ǡ����ƶ����ٵ�Ԫ�أ�
		//ʱ�̹�ע������������п���ͽ����ͷ�
		if(position == start_)
		{
			iterator new_start_ = reserve_elements_at_front(n);
			uninitialized_fill(new_start_, start_, val);
			start_ = new_start_;
		}
		else if(position == finish_)
		{
			iterator new_finish_ = reserve_elements_at_back(n);
			uninitialized_fill(finish_, new_finish_, val);
			finish_ = new_finish_;
		}
		else
		{
			insert_at_the_middle(position, n, val);
		}

	}
	template<class T, class Alloc>
	template<class InputIterator>
	void deque<T, Alloc>::inser_aux(iterator position, iterator first, iterator last, _false_type)
	{

	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::reserve_elements_at_front(size_type n)
	{
		size_type vacancies= (size_type)(start_.cur - start_.first);
		if (vacancies < n)
			reserve_new_elements_at_front(n - vacancies);
		return start_ - n;
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reserve_new_elements_at_front(size_type n)
	{
		//Ϊ�˼���n��Ԫ��Ӧ��ռ�ö��ٸ�node��n + deque_buf_size(sizeof(value_type)) - 1�е�-1��Ϊ�˽�0�ų���
		size_type new_nodes = (n + deque_buf_size(sizeof(value_type)) - 1) / (deque_buf_size(sizeof(value_type)));
		reserve_map_front(new_nodes);

		distance_type i;
		for (i = 0; i < (distance_type)n; i++)
			*(start_.node - i -1) = allocate_node();
	}

	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::reserve_elements_at_back(size_type n)
	{
		size_type vacancies= (size_type)(finish_.cur - finish_.last);
		if (vacancies < n)
			reserve_new_elements_at_back(n - vacancies);
		return finish_ + n;
	}

	template<class T, class Alloc>
	void deque<T, Alloc>::reserve_new_elements_at_back(size_type n)
	{
		//Ϊ�˼���n��Ԫ��Ӧ��ռ�ö��ٸ�node��n + deque_buf_size(sizeof(value_type)) - 1�е�-1��Ϊ�˽�0�ų���
		size_type new_nodes = (n + deque_buf_size(sizeof(value_type)) - 1) / (deque_buf_size(sizeof(value_type)));
		reserve_map_back(new_nodes);

		distance_type i;
		for (i = 0; i < (distance_type)n; i++)
			*(start_.node - i -1) = allocate_node();
	}

	template<class T, class Alloc>
	void  deque<T, Alloc>::insert_at_the_middle(iterator position, size_type n, const value_type& val)
	{
		//insert_at_the_middle���Ҵ�����Ӱ����Ҫ�Ƕ�uninitializedϵ�е�������ʶ��������uninitialized_copy��copy����Ϊ����
		//������ô�ܽ�ģ�����uninitialized memory�����Ƕ���Ҫʹ��uninitializedϵ�У����Ե����Ƕ�uninitialized memory������ʱ��
		//����Ӧ�ñ���������˼·���ڲ��ò�ʹ��uninitialized_copy��ʱ����copyԪ�صĹ�ģ������
		size_type elems_before = position - start_;
		size_type length = this->size();
		value_type val_copy = val;
		if (elems_before <= length >> 1)
		{
			iterator new_start_ = reserve_elements_at_front(n);
			iterator old_start_ = start_;
			if (elems_before <= n)
			{
				//�����µ�position������
				iterator position = new_start_ + elems_before;
				iterator temp = new_start_;
				iterator temp2 = start_;
				for(; temp != position; temp++, temp2++)
					construct(&(*temp), *temp2);
				//���position��start_���uninitialized memory����
				uninitialized_fill(position, start_, val);
				fill_n(start_, elems_before, val);
				start_ = new_start_;
			}
			else
			{
				iterator start_n_ = start_ + n;
				uninitialized_copy(start_, start_n_, new_start_);
				copy(start_n_, position, old_start_);
				fill(position - (distance_type)n, position, val);
			}
		}
		else		//��벿�ֵ�Ԫ���ƶ����٣����Լ�Ԫ�ؾͼ��ں���
		{
			size_type  elems_after = distance_type(finish_ - position);
			iterator new_finish_ = reserve_elements_at_back(n);
			iterator old_finish_ = finish_;
			if(n <= elems_after)
			{
				iterator finish_n_ = finish_ - n;
				uninitialized_copy(finish_n_, old_finish_, old_finish_);
				//��position��finish_n_֮���Ԫ�ظ�ֵ��finish_�����˴�Ӧ�ò���copy_backward����
				copy_backward(position, finish_n_, finish_);
				fill(position, position + n, val);
				finish_ = new_finish_;
			}
			else
			{
				//��Ҫ���ӵ�Ԫ��>elems_after
				iterator finish_n_ = new_finish_ - n;
				iterator temp = position;
				iterator temp1 = finish_n_;
				for (; temp1 != new_finish_; temp++, temp1++)
					construct(&(*temp1), *temp);

				uninitialized_fill(old_finish_, finish_n_, val);
				fill(position, old_finish_, val);
				finish_ = new_finish_;
			}
		}
	}

}


#endif