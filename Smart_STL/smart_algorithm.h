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


	// copy--���������伫��

	// All of these auxiliary functions serve two purposes.  (1) Replace
	// calls to copy with memmove whenever possible.  (Memmove, not memcpy,
	// because the input and output ranges are permitted to overlap.)
	// (2) If we're using random access iterators, then write the loop as
	// a for loop with an explicit count.
	/*******************************
	//sgi_stl��copy������copy�������ص���������������֯�ص����������copy������������������������ʱ�������֮��ȡһ�����У���Ϊ�㲻֪��ʹ��ʱ����ʱ�仹�������ϵĿ��ǡ�
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

	/*****
	���ܣ���ʼcopy��������
	��������Ҫcopy����ʼ��ַfirst����Ҫcopy����ֹ��ַlast��copy����ʼ��ַresult
	����ֵ����
	******/
	template<class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return _copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	/*****
	���ܣ���ʼcopy��char*�汾����
	��������Ҫcopy����ʼ��ַfirst����Ҫcopy����ֹ��ַlast��copy����ʼ��ַresult
	����ֵ����
	******/
	inline char* copy(char* first, char* last, char* result)
	{
		memmove(result, first, last - first);
		//ע��ָ���size�����͵�size����һ�ߴ��
		return result + (last - first);
	}

	/*****
	���ܣ���ʼcopy��wchar_t*�汾����
	��������Ҫcopy����ʼ��ַfirst����Ҫcopy����ֹ��ַlast��copy����ʼ��ַresult
	����ֵ����
	******/
	inline wchar_t* copy(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result, first,  sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	/*****
	���ܣ�_copy_dispatch�º���
	��������Ҫcopy����ʼ��ַfirst����Ҫcopy����ֹ��ַlast��copy����ʼ��ַresult
	����ֵ����
	******/
	template<class InputIterator, class OutputIterator>
	struct _copy_dispatch
	{
		OutputIterator operator() (InputIterator first, InputIterator last, OutputIterator result)
		{
			return _copy(first, last, result, iterator_category(first));
		}
	};

	/*****
	���ܣ�_copy_dispatchƫ�ػ��º���
	��������Ҫcopy����ʼ��ַfirst����Ҫcopy����ֹ��ַlast��copy����ʼ��ַresult
	����ֵ����
	******/
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

	/*****
	���ܣ�_copy_dispatchƫ�ػ��º���
	��������Ҫcopy����ʼ��ַfirst����Ҫcopy����ֹ��ַlast��copy����ʼ��ַresult
	����ֵ����
	******/
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
	����ֵ��result
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

	/*****
	���ܣ�������RandomAccessIterator��copy����
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy������ʼλ��result
	����ֵ��result
	******/
	template<class RandomAccessIterator, class OutputIterator, class Distance>
	inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; n--, result++, first++)
			*result = *first;
		return result;
	}

	/*****
	���ܣ�ԭ��ָ��û��trival assignment operator����
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _true_type)
	{
		//ע��sizeof�ǲ����������Ǻ�������û�а�������ͷ�ļ�
		memmove(result, first, (last - first)*sizeof(T));
		return result + (last - first); 
	}

	/*****
	���ܣ�ԭ��ָ�뺬��trival assignment operator����
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao(const T* first, const T* last, T* result, _false_type)
	{
		return _copy_d(first, last, result, static_cast<ptrdiff_t*>0);
	}
	


	//������Ϊʲô��copy�㷨��û�п���first <= result <= last�п��ܻ�������ǵ����⣬ ������Ϊalgorithm�л����������
	//��������copy_backward���㷨������ͬ�ģ�ֻ�Ƿ���ı���
	/*****
	���ܣ���ʼcopy_backward��������
	��������Ҫcopy_backward����ʼ��ַfirst����Ҫcopy_backward����ֹ��ַlast��copy_backward����ʼ��ַresult
	����ֵ����
	******/
	template<class BidirectionalInterator1, class BidirectionalInterator2>
	inline BidirectionalInterator2 copy_backward(BidirectionalInterator1 first, BidirectionalInterator1 last, BidirectionalInterator2 result)
	{
		return _copy_dispatch_backward<BidirectionalInterator1, BidirectionalInterator2>()(first, last, result);
	}

	/*****
	���ܣ���ʼcopy_backward��char*�汾����
	��������Ҫcopy_backward����ʼ��ַfirst����Ҫcopy_backward����ֹ��ַlast��copy_backward����ʼ��ַresult
	����ֵ����
	******/
	inline char* copy_backward(char* first, char* last, char* result)
	{
		memmove(result - (last - first), first, last - first);
		return result - (last - first);
	}

	/*****
	���ܣ���ʼcopy_backward��wchar_t*�汾����
	��������Ҫcopy_backward����ʼ��ַfirst����Ҫcopy_backward����ֹ��ַlast��copy_backward����ʼ��ַresult
	����ֵ����
	******/
	inline wchar_t* copy_backward(wchar_t* first, wchar_t* last, wchar_t* result)
	{
		memmove(result - (last - first), first, sizeof(wchar_t) * (last - first) );
		return result - (last - first);
	}

	/*****
	���ܣ�_copy_dispatch_backward�º���
	��������Ҫcopy_backward����ʼ��ַfirst����Ҫcopy_backward����ֹ��ַlast��copy_backward����ʼ��ַresult
	����ֵ����
	******/
	template<class BidirectionalInterator1, class BidirectionalInterator2>
	struct _copy_dispatch_backward
	{
		BidirectionalInterator2 operator() (BidirectionalInterator1 first, BidirectionalInterator1 last, BidirectionalInterator2 result)
		{
			return _copy_backward(first, last, result, iterator_category(first));
		}
	};

	/*****
	���ܣ�_copy_dispatch_backwardƫ�ػ��º���
	��������Ҫcopy_backward����ʼ��ַfirst����Ҫcopy_backward����ֹ��ַlast��copy_backward����ʼ��ַresult
	����ֵ����
	******/
	template<class T>
	struct _copy_dispatch_backward<T*, T*>
	{
		//�˴������⣬���еĽṹ��Ҫ�ģ�ģ�庯���޷�ƫ�ػ���Ҫ�÷º���
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			return _copy_htao_backward(first, last, result, htao());

		}
	};

	/*****
	���ܣ�_copy_dispatch_backwardƫ�ػ��º���
	��������Ҫcopy_backward����ʼ��ַfirst����Ҫcopy_backward����ֹ��ַlast��copy_backward����ʼ��ַresult
	����ֵ����
	******/
	template<class T>
	struct _copy_dispatch_backward<const T*, T*>
	{
		T* operator()(const T* first, const T* last, T* result)
		{
			typedef typename _type_traits<T>::has_trivial_assignment_operator htao;
			_copy_htao_backward(first, last, result, htao());
		}
	};

	/*****
	���ܣ���Բ�ͬ����������(���汾ΪInputIterator)��copy_backward���ܺ���
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy_backward������ʼλ��result
	����ֵ��result
	******/
	template<class BidirectionalInterator1, class BidirectionalInterator2>
	inline BidirectionalInterator2 _copy_backward(BidirectionalInterator1 first, BidirectionalInterator1 last, BidirectionalInterator2 result, bidirectional_iterator_tag)
	{
		//��ΪInputIteratorֻ��ִ����ǰ++�Ĳ��������������жϱ߽�ֻ����ͨ��first��=last���жϣ�������ɵ��������ÿ�ζ���Ҫʹ�ñȽϲ�����������ʽ���
		for(; first != last; --last, --result)
			*result = *first;
		return result;
	}

	/*****
	���ܣ���Բ�ͬ����������(���汾ΪRandomAccessIterator)��copy_backward���ܺ���
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy_backward������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class RandomAccessIterator, class BidirectionalInterator>
	inline BidirectionalInterator _copy_backward(RandomAccessIterator first, RandomAccessIterator last, BidirectionalInterator result, random_access_iterator_tag)
	{
		/*RandomAccessIterator����+=n��-=n�Ĺ��ܣ���������ǰ��n�����������n�����������ǾͿ��Բ���ȥ�Ƚϵ�������ֱ������last-first��ֵ����Ϊ
		ѭ��������Ϊ_copy_dispatch�к��С�ԭ��ָ�롱ƫ�ػ��İ汾����ԭ��ָ��Ҳ��RandomAccessIterator������ͬʱ���Ե����������������Ϊ��
		�����Ӵ�����������ǿ��Խ�������ܻ��ֳ�һ������*/
		_copy_d_backward(first, last, result, distance_type(first));
	}

	/*****
	���ܣ�������RandomAccessIterator��copy_backward����
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy_backward������ʼλ��result
	����ֵ��result
	******/
	template<class RandomAccessIterator, class BidirectionalInterator, class Distance>
	inline BidirectionalInterator _copy_d_backward(RandomAccessIterator first, RandomAccessIterator last, BidirectionalInterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; n--, --result, --first)
			*result = *first;
		return result;
	}

	/*****
	���ܣ�ԭ��ָ����trival assignment operator����
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy_backward������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao_backward(const T* first, const T* last, T* result, _true_type)
	{
		//ע��sizeof�ǲ����������Ǻ�������û�а�������ͷ�ļ�
		memmove(result - (last - first), first, (last - first)*sizeof(T));
		return result - (last - first); 
	}

	/*****
	���ܣ�ԭ��ָ��û��trival assignment operator����
	�����������Ƶ�������ʼλ��first���������������ֹλ�ã���������last����Ҫcopy_backward������ʼλ��result
	����ֵ��result+(last-first)
	******/
	template<class T>
	inline T* _copy_htao_backward(const T* first, const T* last, T* result, _false_type)
	{
		return _copy_d_backward(first, last, result, static_cast<ptrdiff_t*>0);
	}

}
#endif _SMART_ALGORUTHM_H_