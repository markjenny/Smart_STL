#ifndef _SMART_VECTOR_H_
#define _SMART_VECTOR_H_
#include <iostream>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_construct.h"
#include "smart_uninitialized.h"
#include "smart_algorithm.h"
#include <cstddef> //for ptrdiff_t  and size_t,because you need to "define" the ptrdiff_t
/*****��ʱû�а���reverse_iterator��ع���******
author:Mark
date:4/15/2015
************************************************/

namespace smart_stl
{
	//for vector
	/*��Ϊ�ڶ���ģ���ʱ��vector��simple_alloc�����������ռ�
	smart_stl�У����Բ����ü���smart_stl::�����ռ��ʶ��*/
	/*************vector����Ҫ�ĺ�������****************
	���ȣ���Ҫ��¼�ռ����õĻ�����start��finish��end_of_storage����Щ���Ƕ���ɵ���������
	����������
	begin()��end()��size()��capacity()��empty()��oprator[]()��front()��back()��push_back()��
	pop_back()��erase()�������������������������resize()��clear()
	*****************************************************/
	template<class T, class Alloc = simple_alloc<T>>
	class vector
	{
	public:
		//Ϊvector������Ƕ���ͣ������������
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;
		//the iterator is only for vector
		typedef T* iterator;
		//const����Ƕ����
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef const T* const_iterator;

		//the vector allocator,why is the allocator defined protected,perhaps other container take the vector as the sequence
	protected:
		//��ʱ����ģ������У��Ѿ�ȷ����T,�����ں����data_allocator�п���ֱ��ʹ��
		typedef Alloc data_allocator;

	private:
		iterator start_;
		iterator finish_;
		iterator end_of_storage_;

	public:
		/************************************�漰�����캯�������ƹ��캯������������*************************************/
		
		/*****
		���ܣ�vector���캯��
		��������
		����ֵ����
		******/
		vector() : start_(0), finish_(0), end_of_storage_(0){}

		/*****
		���ܣ�vector���캯��
		��������Ҫ�����Ԫ�ظ���n
		����ֵ����
		******/
		//��ֹ��ʽ���ù��캯�����ýӿھ���ͳһ�ԣ���ʹ�ýӿڸ���������ȷʹ�á���ע��explicit���������η�
		explicit vector(const size_type n);

		/*****
		���ܣ�vector���캯��
		��������Ҫ�����Ԫ�ظ���n����Ҫ�����Ԫ��ֵval
		����ֵ����
		******/
		vector(size_type n, const value_type& val);

		/*****
		���ܣ�vector���캯��
		�����������Ƶ�vector�ĵ�����first��last
		����ֵ����
		******/
		//����ͨ�����е�����������vector����
		template<class InputIterator>
		vector(InputIterator first, InputIterator last);

		/*****
		���ܣ�vector���ƹ��캯��
		�����������Ƶ�vector����v
		����ֵ����
		******/
		vector(const vector& v);

		/*****
		���ܣ�vector��assignment operator
		�����������и�ֵ������vector����
		����ֵ����
		******/
		vector& operator=(const vector& v);

		/*****
		���ܣ�vector��������
		��������
		����ֵ����
		******/
		~vector();

		/**************************************************************************************************************************/


		/****************************************************��Ƚ����*************************************************************/
		/*****
		���ܣ��ж�����vector�����Ƿ���ͬ
		�������������Ƚϵ�vector����v
		����ֵ���Ƿ���ͬ��bool�ͽ��
		******/
		bool operator==(const vector& v) const;

		/*****
		���ܣ��ж�����vector�����Ƿ�ͬ
		�������������Ƚϵ�vector����v
		����ֵ���Ƿ�ͬ��bool�ͽ��
		******/
		bool operator!=(const vector& v) const;

		/*****
		���ܣ��ж�����vector�����Ƿ���ͬ�ġ���Ԫ������
		�������������Ƚϵ�����vector����v1��v2
		����ֵ���Ƿ���ͬ��bool�ͽ��
		******/
		friend bool operator==(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2);


		/*****
		���ܣ��ж�����vector�����Ƿ�ͬ�ġ���Ԫ������
		�������������Ƚϵ�����vector����v1��v2
		����ֵ���Ƿ���ͬ��bool�ͽ��
		******/
		friend bool operator!=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2);
		/**************************************************************************************************************************/

		/*****************************************************����������********************************************************/
		iterator begin(){return start_;}
		//const_iterator begin()const {return start_;}
		const_iterator cbegin() const {return start_;}
		iterator end() {return finish_;}
		const_iterator end() const {return finish_;}
		//������ͬ���������������ݶ���һ���ģ�������ԭ��
		const_iterator cend() const{return finish_;}
		/***************************************************************************************************************************/

		/***********************************************************���������*****************************************************/
		distance_type size() const {return finish_ - start_;}
		distance_type capacity() const {return end_of_storage_ - start_;}
		bool empty() const {return start_ == finish_;}
		void resize(size_type new_size);
		void resize(size_type new_size, const T& val);
		/***************************************************************************************************************************/

		/************************************************************�����Ԫ���й�************************************************/
		//������Ϊreference oprator[]����begin()����const_reference operator[]����cbegin()��Ϊ�˸��õ���⺯�����߼�
		reference operator[](distance_type i)  {return *(begin() + i);}
		//�ڷ���Ԫ�ظ���ĳ��������
		const_reference operator[](distance_type i) const {return *(cbegin() + i);}
		reference front() {return (*begin());}
		reference back() {return *(end() - 1);}
		/***************************************************************************************************************************/

		/**********************************************************���޸��������**************************************************/
		//�������������еĶ��󣬲�ʹ��������sizeΪ0�����ǲ����������еĿռ�
		void clear();
		//������������������Ҫ������һ������������Ϊ���ô���
		void swap(vector& v);
		/*****
		���ܣ�push_back����
		�����������Ԫ��val
		����ֵ����
		******/
		void push_back(const value_type& val);
		void pop_back();
		//�������������positionλ��ǰ���в��룬�����ظ�λ��ǰ������
		iterator insert(iterator positon, const value_type& val);
		//�ڶ��ֵ������ǽ�positionλ��ǰ����n��ֵΪval�ĵ�����������n�������������n������������������n��������
		void insert(iterator positon, const size_type& n, const value_type& val);
		//eraseҲ�������ֺ�����һ����ֻɾ���ض�λ��position�ϵĵ��������ڶ�����ɾ��
		//erase��ʱ�򷵻ص���positionǰһ��λ�õĵ�����
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		/***************************************************************************************************************************/

	private:
		//fill_initialize����vector�Ĺ��캯��
		void fill_initialize(size_type n, const value_type& val);
		//�������ÿռ䲢�����ݽ������
		iterator allocate_and_fill(size_type n, const value_type& val);
		//����Ҫʹ�õ�����Ͳ���ĸ�������insert_aux,vector_aux
		/*****************
		�˴�����д
		******************/
		//private���͵���غ���
		void deallocate();

		//���ֵ���vector(InputIterator first, InputIterator last)����ָ�뻹��������Ȼ����в�ͬ�Ĵ���
		template<class InputIterator>
		void vector_aux(InputIterator first, InputIterator last, _false_type);

		template<class Integer>
		void vector_aux(Integer n, const value_type& val, _true_type);

		template<class InputIterator>
		void allocate_and_copy(InputIterator first, InputIterator last);

		//push_back����insert_aux��sgi_stl�����ж��Ƿ��б��ÿռ䣬Ȼ��insert_aux����ȥ�ж��Ƿ��б��ÿռ䣬����û�б�Ҫ��ֱ����push_back
		//ȥ����insert_aux
		void insert_aux(iterator position, const T& val);



	};
	/**********************************��vector���ʵ�֡�***************************************************/
	//����ģ����ĺ���������ʵ�ֵ�����һ�ַ�ʽ�ǽ�������ʵ�ֶ�����.h�ļ��У�һ�±���vector�ľ���ʵ��

	
	
	/**************************�漰�����캯�������ƹ��캯������������**********************************/
	/*****
	���ܣ�vector���캯��
	��������
	����ֵ����
	******/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(const size_type n)
	{
		fill_initialize(n, T());
	}

	/*****
	���ܣ�vector���캯��
	��������Ҫ�����Ԫ�ظ���n�������Ԫ��ֵΪval
	����ֵ����
	******/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(size_type n, const value_type& val)
	{
		fill_initialize(n, val);
	}

	/*****
	���ܣ�vector���캯��
	�����������Ƶ�vector�ĵ�����first��last
	����ֵ����
	******/
	template<class T, class Alloc>
	template<class InputIterator>
	vector<T, Alloc>::vector(InputIterator first, InputIterator last)
	{
		typedef typename is_Integer<InputIterator>:: class_type is_integer;
		vector_aux(first, last, is_integer());
	}

	/*****
	���ܣ�vector���ƹ��캯��
	�����������Ƶ�vector����v
	����ֵ����
	******/
	template<class T, class Alloc>
	vector<T, Alloc>::vector(const vector& v)
	{
		allocate_and_copy(v.start_, v.finish_);
	}

	/*****
	���ܣ�vector��assignment operator
	�����������и�ֵ������vector����
	����ֵ����
	******/
	template<class T, class Alloc>
	vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& v)
	{
		//��ȫ���
		if (*this != v)
			allocate_and_copy(v.start_, v.finish_);
		return *this;
	}


	/*****
	���ܣ�vector��������
	��������
	����ֵ����
	******/
	template<class T, class Alloc>
	vector<T,Alloc>::~vector()
	{
		//����������һ�������ʱ�򣬴��µ�˼·���������ģ��Ƚ������١�Ȼ�󽫸ÿ��ڴ�鷵�����ڴ��
		smart_stl::destroy(start_, finish_);
		deallocate();
	}


	/***************************************************************************************************************************/




	/****************************************************��Ƚ����*************************************************************/
	/*****
	���ܣ��ж�����vector�����Ƿ���ͬ
	�������������Ƚϵ�vector����v
	����ֵ���Ƿ���ͬ��bool�ͽ��
	******/
	template<class T, class Alloc>
	bool vector<T, Alloc>::operator==(const vector& v) const
	{
		if ( (finish_ -  start_ ) != v.size() )
			return false;
		else
		{
			iterator ptr_temp1 = start_;
			iterator ptr_temp2 = v.start_;

			for(; ptr_temp1 != finish_; ptr_temp1++, ptr_temp2++)
				if (*ptr_temp1 != *ptr_temp2)
					return false;

			return true;
		}
	}

	/*****
	���ܣ��ж�����vector�����Ƿ�ͬ
	�������������Ƚϵ�vector����v
	����ֵ���Ƿ�ͬ��bool�ͽ��
	******/
	template<class T, class Alloc>
	bool vector<T, Alloc>::operator!=(const vector& v) const
	{
		return !(*this == v);
	}

		/*****
		���ܣ��ж�����vector�����Ƿ���ͬ�ġ���Ԫ������
		�������������Ƚϵ�����vector����v1��v2
		����ֵ���Ƿ���ͬ��bool�ͽ��
		******/
		//friend���������η���������ʵ�ֵ�ʱ��Ͳ��������
		template<class T, class Alloc>
		bool operator==(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2)
		{
			return v1.operator =(v2);
		}


		/*****
		���ܣ��ж�����vector�����Ƿ�ͬ�ġ���Ԫ������
		�������������Ƚϵ�����vector����v1��v2
		����ֵ���Ƿ���ͬ��bool�ͽ��
		******/
		//friend���������η���������ʵ�ֵ�ʱ��Ͳ��������
		template<class T, class Alloc>
		bool operator!=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2)
		{
			return !(v1==v2);
		}
	/********************************************************************************************************************************/




	/***********************************************************���������*****************************************************/
	/*****
	���ܣ�resize���������¶���ߴ�
	��������Ҫ������µ�size
	����ֵ����
	******/
	template<class T, class Alloc>
	void vector<T, Alloc>::resize(size_type new_size)
	{
		resize(size_type, T());
	}

	/*****
	���ܣ�resize���������¶���ߴ�
	��������Ҫ������µ�size��
	����ֵ����
	******/
	//resize���÷�����newsize��ԭ����sizeС�Ļ�����ô��ֻ����Ǯnew_size���������newsize��size��Ļ���
	//�ͽ�newsize��size�����Щ����vector��β��
	template<class T, class Alloc>
	void vector<T, Alloc>::resize(size_type new_size, const T& val)
	{
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), val);
	}
	/********************************************************************************************************************************/



	/**********************************************************���޸��������**************************************************/
	template<class T, class Alloc>
	void vector<T, Alloc>::clear()
	{
		//������������
	}


	template<class T, class Alloc>
	void vector<T, Alloc>::swap(vector& v)
	{
		//������������
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::push_back(const value_type& val)
	{
		insert_aux(finish_, val);
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::pop_back()
	{
		//������������

	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator positon, const value_type& val)
	{
		//������������
		return start_;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::insert(iterator positon, const size_type& n, const value_type& val)
	{
		//������������
		return start_;
	}
	

	/*****
	���ܣ�ɾ���������ض�
	��������Ҫɾ����λ��position
	����ֵ����һ��Ԫ�ض�Ӧ�ĵ�����
	******/
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
	{
		return erase(position, position+1);
	}

	/*****
	���ܣ�ɾ���������ض���Χ�ڵ�Ԫ��
	��������Ҫɾ���ķ�Χ��ʼfirst����ֹlast
	����ֵ��lastλ�ô���Ԫ�صĵ�����
	******/
	//erase��ʵ���ǽ�û�б�ɾ������ǰ�ƶ�
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{
		//��Ϊvector��iterator��ԭ��ָ�룬���Կ�������+=n��-=n���÷�
		//���last��end()��ͬ��˵��Ҫɾ���Ķ��ǡ���벿�֡������ǾͲ�Ҫ�ٵ���copy�ˣ���Ϊcopy�����ͺ���ģ�壬�������ٱ��������������
		if(last != end())
			copy(last, finish_, first);
		//ֻ�����������󣬵��ǲ��ͷſռ�
		for(distance_type n = last - first; n > 0 ; n--)
		{
			--finish_;
			destroy(finish_);
		}
		return first;
	}
	/***************************************************************************************************************************/




	/********************************************private����**************************************************************************/
	template<class T, class Alloc>
	void vector<T, Alloc>::fill_initialize(size_type n, const value_type& val)
	{
		start_ = allocate_and_fill(n, val);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& val)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, val);
		return result;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::deallocate()
	{
		//�ж�start_�Ƿ�Ϊnull
		if (start_)
		{
			data_allocator::deallocate(start_, end_of_storage_ - start_);//end_of_storage_��end_����ǰ����յ�����
		}
	}


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::vector_aux(InputIterator first, InputIterator last, _false_type)
	{
		allocate_and_copy(first, last);
	}


	template<class T, class Alloc>
	template<class Integer>
	void vector<T, Alloc>::vector_aux(Integer n, const value_type& val, _true_type)
	{
		fill_initialize(n, val);
	}


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::allocate_and_copy(InputIterator first, InputIterator last)
	{
		start_ = data_allocator::allocate(last - first);
		finish_ = uninitialized_copy(first, last, start_);
		end_of_storage_ = finish_;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T& val)
	{
		//�ж��Ƿ��б��ÿռ�
		if (finish_ != end_of_storage_)
		{
			//����пռ�������������βӦ�����⿼�ǣ�stlԴ��������finish_-2��position����finish_��ʱ��ʹ������������������С��
			if(position == finish_)
			{
				*position = val;
				finish_++;
				return;
			}
			//��Ϊֻ����һ��Ԫ�أ�����ֱ����constructûʲôӰ��
			construct(finish_, *(finish_ - 1));
			++finish_;
			//û�б�Ҫʹ���м����
			//T val_copy = val;
			//����һ��Ϊʲô��Ҫ��position~finish_-2���Ƶ�finish_-1��������position~finish_-3���Ƶ�finish_-2�����ٸ���һ��Ԫ�أ�
			//������Ϊ���ǲ���֪�������ж೤�����capacityֻ��2����ôfinish_-3��Ȼ�Ǵ���ģ�����Ҫ���Ǳ߽磡
			copy_backward(position, finish_ - 2, finish_ - 1);

			*position = val;
		}
		else
		{
			const size_type old_size = size();
			//����ط�Ҳ��һ�£�STLԴ��������˵����old_sizeΪ0��ʱ�򣬽�new_size��Ϊ1����������ʹ��
			//��old_sizeС�ķǳ���Ţ��ÿ�ζ�Ҫ�����¿ռ䣬ת��Ԫ�أ��ͷžÿռ�
			const size_type new_size = old_size == 0 ? 1 : 2 * old_size;


			iterator new_start_ = data_allocator::allocate(new_size);
			iterator new_finish_ = new_start_;

			new_finish_ = uninitialized_copy(start_, position, new_start_);

			//��position��λ���������Ԫ�أ���ΪҪ��position��λ���ϲ��룬����ԭpositionλ���ϵ�Ԫ�������һλ��;
			construct(new_finish_, val);
			//����ˮλ
			new_finish_++;

			new_finish_ = uninitialized_copy(position, finish_, new_finish_);

			//����ԭ�����ϵ�Ԫ�أ����ͷſռ�
			destroy(start_, finish_);
			deallocate();

			start_ = new_start_;
			finish_ = new_finish_;
			end_of_storage_ = start_ + new_size;
		}
	}
	/**************************************************************************************************************************/


}

#endif	_SMART_VECTOR_H_
