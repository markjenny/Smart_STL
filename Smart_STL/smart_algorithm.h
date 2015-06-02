#ifndef _SMART_ALGORUTHM_H_
#define _SMART_ALGORUTHM_H_
namespace smart_stl
{
	/*****
	功能：vector复制构造函数
	参数：被复制的vector对象v
	返回值：无
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