#ifndef _SMART_UNINITIALIZED_
#define _SMART_UNINITIALIZED_
#include "smart_iterator_base.h"
#include "smart_construct.h"
#include "smart_algorithm.h"
//��ͷ�ļ�������Ӧ����container��copy��fill��fill_n�ĵײ�
//�ú�����Ҫʹ�õ�����iterator����ȡ��غ���value_type��_type_traits��true_type��false_type��is_POD_type�Ⱥ���
namespace smart_stl
{
	/************************************
	����uninitialized_copy��uninitialized_fill��uninitialized_fill_n��˵��������Ҫ�ǹ۲챻fill�����Ǳ�copy�������Ƿ���POD��
	�����POD����ô����һ����ӵ��default ctor/dtor/copy/assignment����������ʹ����������ʮ�ַ���ģ����硰*first = val;��
	������Ϳ������assignment�����������POD������Ҫ�������Լ��Ĺ��캯�������
	*************************************/

	//����uninitialized_fill_n����
	//ΪʲôҪ��first����ɶ�д���͵ĵ�������������Ϊ����Ҫ��first���ж�д��eg��&*first
	//��Ҫע����ǣ�uninitialized_fill_n����firstΪ��㣬��������n��Ԫ�أ���������ָ�����һ���������Ԫ�ص���һ��λ��
	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
		return _uninitialized_fill_n(first, n, x, value_type(first));
	}

	template<class ForwardIterator, class Size, class T, class T1>
	inline ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
	{
		typedef typename _type_traits<T1>::is_POD_type is_POD;
		return _uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type)
	{
		//���õ���smart_algorithm�е��㷨
		return fill_n(first, n, x);
	}
	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type)
	{
		ForwardIterator cur = first;
		//SIZE i = 0;��仰������ϵĻ��ᵼ�¶���4bytes���ڴ棬����û�б�Ҫ����Ϊ����ֻ��һ��n��������ѭ����
		//����ֱ��ʹ��n
		��ֱ��0��ѭ������
		for(; n > 0; n--, cur++)
		{
			//Ϊʲô��ֱ�����õ�������&����������Ϊ�ڵ����������ʱ�����ǽ�*��&�����������������ˣ�
			//���Բ���ֱ����&�����������յ������жԡ�*������˼�룺��*��������ȡ���������е�����Ԫ�أ���ô����&ʱ����
			//������Ԫ�صĵ�ַ�������Ϳ����ڸõ�ַ���й���
			construct(&*cur, x);
		}
		return cur;
	}


	//����uninitialized_fill����
	template<class ForwardIterator, class T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
	{
		_uninitialized_fill(first, last, x, value_type(first));
	}

	template<class ForwardIterator, class T, class T1>
	inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
	{
		typedef typename _type_traits<T1>::is_POD_type is_POD;
		_uninitialized_fill_aux(first, last, x, is_POD());
	}

	template<class ForwardIterator, class T>
	inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _true_type)
	{
		fill(first, last, x);
	}

	template<class ForwardIterator, class T>
	inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _false_type)
	{
		ForwardIterator cur = first;
		for(; cur != last; cur++)
		{
			construct(&*cur, x);
		}
	}

	//����uninitialized_copy����,ForwardIterator,����������������Ͻ��ж�д����
	//��ΪҪcopyһ�������ڵ����ݣ�����������ݾ���ֻ���ģ���һ��Ҫ��֤��д�ĺ���˼·������
	template<class InputIterator, class ForwardIterator>
	//�ú���Ҫ����result+last-first
	//ֻ��ͬһlevel�ĺ������ܿ������أ�����ᵼ���߼�������
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
	{
		return _uninitialized_copy(first, last, result, value_type(result));
	}

	template<class InputIterator, class ForwardIterator, class T>
	inline ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
	{
		typedef typename _type_traits<T>::is_POD_type is_POD;
		return _uninitialized_copy_aux(first, last, result, is_POD());
	}

	template<class InputIterator, class ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _true_type)
	{
		return copy(first, last, result);
	}

	template<class InputIterator, class ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _false_type)
	{
		ForwardIterator cur_Forward = result;
		//InputIterator cur_Input = first;
		for (; first!= last; cur_Forward++, first++)
		{
			construct(&*cur_Forward, *first);
		}
		return cur_Forward;
	}

	//Ϊchar*,wchar_t*����_uninitialized_copyƫ�ػ�
	inline char* _uninitialized_copy(char* first, char* last, char *result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* _uninitialized_copy(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	
}


#endif