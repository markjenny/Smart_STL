#ifndef _SMART_VECTOR_H_
#define _SMART_VECTOR_H_
#include <iostream>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
#include "smart_construct.h"
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
		typedef ptrdiff_t difference_type;
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
		/*****�漰�����캯�������ƹ��캯������������**
		*************************************************/
		vector() : start_(0), finish_(0), end_of_storage_(0){std::cout << "������û�в����Ĺ��캯��" << std::endl;}
		//��ֹ��ʽ���ù��캯�����ýӿھ���ͳһ�ԣ���ʹ�ýӿڸ���������ȷʹ�á���ע��explicit���������η�
		explicit vector(const size_type n);
		vector(size_type n, const value_type& val);
		//����ͨ�����е�����������vector����
		template<class InputIterator>
		vector(InputIterator first, InputIterator last);
		vector(const vector& v);
		vector& operator=(const vector&);
		//�����ĳ����Ҫ���ö�������ĺ������������ĺ���ʱ��ͨ��������ô�ú���û�б�Ҫ���ó�inline���͵ġ�
		//inline�����ʺ���Щ��̵ģ��ظ��Ըߵĺ���������~vector()������Ϊ���ʺ���inline����
		~vector();
// 		{
// 			destroy(start_, finish_);
// 			deallocate();
// 		}


		/***��Ƚ����************
		************************/
		bool operator==(const vector& ) const;
		bool operator!=(const vector&) const;

		/*****����������*********
		*****************************/
		iterator begin(){return start_;}
		const_iterator begin()const {return start_;}
		const_iterator cbegin() const {return start_;}
		iterator end() {return finish_;}
		const_iterator end() const {return finish_;}
		//������ͬ���������������ݶ���һ���ģ�������ԭ��
		const_iterator cend() const{return finish_;}

		/******���������**************
		**************************/
		difference_type size() const {return finish_ - start_;}
		difference_type capacity() const {return end_of_storage_ - start_;}
		bool empty() const {return start_ == finish_;}
		void resize(size_type new_size);
		void resize(size_type new_size, const T& val);

		/*******�����Ԫ���й�***********
		******************************/
		//������Ϊreference oprator[]����begin()����const_reference operator[]����cbegin()��Ϊ�˸��õ���⺯�����߼�
		reference operator[](difference_type i)  {return *(begin() + i);}
		//�ڷ���Ԫ�ظ���ĳ��������
		const_reference operator[](difference_type i) const {return *(cbegin() + i);}
		reference front() {return (*begin());}
		reference back() {return (*end());}

		/*******���޸��������***********
		******************************/
		//�������������еĶ��󣬲�ʹ��������sizeΪ0�����ǲ����������еĿռ�
		void clear();
		//������������������Ҫ������һ������������Ϊ���ô���
		void swap(vector& v);
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

	private:
		//fill_initialize����vector�Ĺ��캯��
		void fill_initialize(size_type n, const value_type& val);
		//�������ÿռ䲢�����ݽ������
		void allocate_and_fill(size_type n, const value_type& val);
		//����Ҫʹ�õ�����Ͳ���ĸ�������insert_aux,vector_aux
		/*****************
		�˴�����д
		******************/
		//private���͵���غ���
		void deallocate();





	};
	/**********************************vector���ʵ��***************************************************/
	//����ģ����ĺ���������ʵ�ֵ�����һ�ַ�ʽ�ǽ�������ʵ�ֶ�����.h�ļ��У�һ�±���vector�ľ���ʵ��


	template<class T, class Alloc>
	vector<T, Alloc>::vector(const size_type n)
	{
		fill_initialize(n, T());
	}

	template<class T, class Alloc>
	vector<T, Alloc>::vector(size_type n, const value_type& val)
	{
		fill_initialize(n, val);
	}

	template<class T, class Alloc>
	template<class InputIterator>
	vector<T, Alloc>::vector(InputIterator first, InputIterator last)
	{

	}


	template<class T, class Alloc>
	vector<T,Alloc>::~vector()
	{
		std::cout << "������������" << std::endl;
	}



	/*��Ҫ˵��һ�㣬����ģ���ຯ�������ⶨ���ʱ����Ҫ�����ض���ͷ�����磺
	template<class T, class Alloc>
	vector<T, Alloc>::
	�����ں����Ĳ����У�ȷʵ����ֱ����value_type������T������Ϊ�����Ѿ������������define��value_type��ʲô��˼�������������ֱ����
	*/
	template<class T, class Alloc>
	void vector<T, Alloc>::fill_initialize(size_type n, const value_type& val)
	{
		start_ = allocate_and_fill(n, val);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}

	template<class T, class Alloc>
	iterator vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& val)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, val);
		result = result - n;
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
}

#endif	_SMART_VECTOR_H_
