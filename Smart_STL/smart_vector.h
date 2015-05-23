#ifndef _SMART_VECTOR_H_
#define _SMART_VECTOR_H_
#include "../smart_iterator_base.h"
#include "../smart_alloc.h"
#include "../smart_construct.h"
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
		vector() : start_(0), finish_(0), end_of_storage_(0){}
		//��ֹ��ʽ���ù��캯�����ýӿھ���ͳһ�ԣ���ʹ�ýӿڸ���������ȷʹ�á�
		explicit vector(const size_type n);
		vector(size_type n, const value_type& value);
		//����ͨ�����е�����������vector����
		template<class InputIterator>
		vector(InputIterator first, InputIterator last);
		vector(const vector& v);
		vector& operator=(const vector&);
		//�����ĳ����Ҫ���ö�������ĺ������������ĺ���ʱ��ͨ��������ô�ú���û�б�Ҫ���ó�inline���͵ġ�
		//inline�����ʺ���Щ��̵ģ��ظ��Ըߵĺ���������~vector()������Ϊ���ʺ���inline����
		~vector();


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
		void resize(size_type new_size, const T& value);

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
		fill_initialize();
		//�������ÿռ䲢�����ݽ������
		allocate_and_fill(const size_type& n, const value_type& val);
		//����Ҫʹ�õ�����Ͳ���ĸ�������insert_aux,vector_aux
		/*****************
		�˴�����д
		******************/
		//private���͵���غ���
		void deallocate();



	};
}

#endif	_SMART_VECTOR_H_
