#ifndef _SMART_ALGORUTHM_H_
#define _SMART_ALGORUTHM_H_
namespace smart_stl
{
	/*****
	���ܣ�vector���ƹ��캯��
	�����������Ƶ�vector����v
	����ֵ����
	******/
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& val)
	{
		for (; n > 0; first++, n-- )
			*first = n;
		return first;
	}
}
#endif _SMART_ALGORUTHM_H_