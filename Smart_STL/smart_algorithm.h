#ifndef _SMART_ALGORUTHM_H_
#define _SMART_ALGORUTHM_H_
#include <string.h>
#include <cstddef>			//for ptrdiff_t
#include "smart_iterator_base.h"


namespace smart_stl
{
	/*****
	���ܣ�fill_n�㷨
	��������Ҫfill����ʼ��ַfirst����Ҫfill�ĸ���n��fill��Ԫ��ֵval
	����ֵ����
	******/
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& val)
	{
		for (; n > 0; first++, n-- )
			*first = val;
		return first;
	}

	/*****
	���ܣ�copy�㷨
	��������Ҫfill����ʼ��ַfirst����Ҫfill�ĸ���n��fill��Ԫ��ֵval
	����ֵ����
	******/
	/*******************************
	������Ҫ˵һ�£�ԭ��ָ�벢���Ƿ�Ҫָ���������͵�ָ�룬������int��float��double�ȣ�ͬʱҲ�������Լ������һ����class����νԭ��ָ��
	���������ָ���������͵�ָ�룬��������Ϊ����ָ�뵫��ȴ����ָ���
                                |-------_copy_dispatch-----|--------_copy_dispatch���ͣ���ʱ�Ѿ�˵��first��last����ԭ��ָ���ˣ���ô���ǵ��������͵��ˣ���ô���ǾͿ���
																						�������Ƿ���InputIterator����RandomIterator�ˣ�����ע���������RandomIterator����ôʣ�µľͶ���InputIterator����Ϊ
																						���������������Ͷ��Ǽ̳�InputIterator��ʡȥ��ת������˼·Ҳ��������
								|				                           |--------ƫ�ػ� ԭ��ָ��_copy_dispatch(T*,T*) : ���ʱ��Ϳ��Կ���has_trivial_assignment_operator�����
								|                                          |--------ƫ�ػ� ԭ��ָ��_copy_dispatch(const T* ,const T*) �� ���ʱ����Կ���has_trivial_assignment_operator�����
	copy���ֲܷ�---- |-------copy(char*,char*)
                                |
                                |
                                |-------copy(wchar_t*, wchar_t*)
	********************************/
	//������Ҫ˵һ����ǣ��ںܶ�С�ĺ������Բ���inline����������������ת���ܵĺ�������Ϊ��ת�������������������ջ����Ȳ��Ҳ��ϵ�ȥ���ļĴ�����CS��IPָ��
	//���ָ��һֱ���任
	template<class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return _copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	inline char* copy(char* first, char* last, char* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* copy(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	template<class InputIterator, class OutputIterator>
	struct _copy_dispatch
	{
		OutputIterator operator() (InputIterator first, InputIterator last, OutputIterator result)
		{
			return _copy(first, last, result, iterator_category(first));
		}
	};

	template<class T>
	struct _copy_dispatch<T*, T*>
	{
		//�˴������⣬���еĽṹ��Ҫ�ģ�ģ�庯���޷�ƫ�ػ���Ҫ�÷º���
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			return _copy_htao(first, last, result, htao());

		}
	};

	template<class T>
	struct _copy_dispatch<const T*, T*>
	{
		T* operator()(const T* first, const T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			_copy_htao(first, last, result, htao());
		}
	};

	/*****
	���ܣ���Բ�ͬ����������(���汾ΪInputIterator)��copy���ܺ���
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class InputIterator, class OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//��ΪInputIteratorֻ��ִ����ǰ++�Ĳ��������������жϱ߽�ֻ����ͨ��first��=last���жϣ�������ɵ��������ÿ�ζ���Ҫʹ�ñȽϲ�����������ʽ���
		for(; first != last; first++, result++)
			*result = *first;
		return result;
	}

	/*****
	���ܣ���Բ�ͬ����������(���汾ΪRandomAccessIterator)��copy���ܺ���
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class RandomAccessIterator, class OutputIterator>
	inline OutputIterator _copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
	{
		/*RandomAccessIterator����+=n��-=n�Ĺ��ܣ���������ǰ��n�����������n�����������ǾͿ��Բ���ȥ�Ƚϵ�������ֱ������last-first��ֵ����Ϊ
		ѭ��������Ϊ_copy_dispatch�к��С�ԭ��ָ�롱ƫ�ػ��İ汾����ԭ��ָ��Ҳ��RandomAccessIterator������ͬʱ���Ե����������������Ϊ��
		�����Ӵ�����������ǿ��Խ�������ܻ��ֳ�һ������*/
		_copy_d(first, last, result, distance_type(first));
	}

	template<class RandomAccessIterator, class OutputIterator, class Distance>
	inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; n--, result++, first++)
			*result = *first;
		return result;
	}

	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _true_type)
	{
		//ע��sizeof�ǲ����������Ǻ�������û�а�������ͷ�ļ�
		memmove(result, first, (last - first)*sizeof(T));
		return result + (last - first); 
	}

	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _false_type)
	{
		return _copy_d(first, last, result, static_cast<ptrdiff_t*>0);
	}
	

}
#endif _SMART_ALGORUTHM_H_