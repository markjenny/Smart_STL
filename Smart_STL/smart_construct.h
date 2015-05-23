/************************************************************************/
/* ���캯�������������Ļ�������                                                              */
/************************************************************************/
#ifndef _SMART_CONSTRUCT_H_
#define _SMART_CONSTRUCT_H_

#include <new> //�������е�placement new
#include "../smart_iterator_base.h"
#include "../smart_type_traits.h"


namespace smart_stl
{
	//Ĭ�Ϲ��캯���汾
	template <class T>
	inline void construct(T * p)
	{
		new (p) T();
	}

	//��Ϊ�ú����Ƚ϶�С�������õ���ʱ��ǳ��࣬���Բ���inline�ķ�ʽ
	template <class T1, class T2>
	inline void construct(T1 * p, T2 value)
	{
		//�����Ƕ�Ӧplacement new��Ӧ�÷���������pָ���Ϲ���T1���͵����ݣ�value��T2���͵ģ���Ҫ֮ǰ��ǿ��ת��
		new (p) T1(value);
	}


	//�������ĵ�һ���汾
	template <class T>
	inline void destroy(T* pointer)
	{
		pointer->~T();
	}

	//�������ĵڶ����汾�����������������ú����������ǽ�������value_type*��ȡ����
	template <class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last)
	{
		//value_type��Ϊ����ȡ�����ĵ�����ʵ������ʲô����ָ��
		_destroy(first, last, value_type(first));
	}

	/*�ж�Ԫ���ͱ��Ƿ���non-trivial���캯������Ϊ����Ҫ�ж�first��ָ���Ԫ�ص����ͣ�����Ϊ�˸�
	�����ʹ�ã�����Ӧ����ͨ���ж�first���ж�����ָ���Ԫ�ص����͵ĺ���*/
	//��ģ�������ˡ�����������
	//�ú����������ǽ�value_type�Ƿ����û��Դ���non-trvial-ctor������о͵��ø�alue_type�Լ�����������
	//����ʲô����������Ϊ����trivial ctor���������Ͷ���ԭʼ��������
	template <class ForwardIterator, class T>
	inline void _destroy(ForwardIterator first, ForwardIterator last, T* value_type_of_iterator)
	{
		typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
		_aux_destroy(first, last, trivial_destructor());

	}

	//���õ�������Ӧ�Ķ�����û��Ĭ�Ϲ��캯���ģ���ôҪ��ʾ����destroy����
	template<class ForwardIterator>
	inline void _aux_destroy(ForwardIterator first, ForwardIterator last, _false_type)
	{
		for (; first != last; first++)
		{
			//��������д��ԭ������Ϊ�����ڶ�����������ط���ʱ��һ�㶼�����á�*��չ�ֳ��õ�������ָ�Ķ���
			//*firstҲ�����������е�T���ͣ���ô&T�Ը����͵Ķ���ȡ��ַ����destroy�����е�"T*"�Ĳ���
			destroy(&*first);
		}
	}

	//���Ҫdestroy�Ķ�����ԭʼ�������ͣ����Ǻ���trivial-ctor�Ķ�����ôʲôҲ����
	template<class ForwardIterator>
	inline void _aux_destroy(ForwardIterator first, ForwardIterator last, _true_type)
	{
		//ʲô������������ϵͳ�Դ���trivial-ctor��ԭʼ���ݣ��ǲ��ý���������
	}

	/*��stl���е�Դ���У�����������ػ����������������type_traits��ֱ��������������͵Ļ�����ô��
	����ֱ����д��������������������ʲôҲû����������Ϊʲô�����ǵ�����ȡ��������������ʷԭ��
 	inline void destroy(char*, char*)
 	{
 		//ʲôҲ��������Ϊ��type_traits����û�а�����������
 	}
	*/
	//Ϊchar�ͺ�wchar_t���趨��������Ϊ��ʵ���У�����char��wchar_t�͵�������������Ƿǳ���ģ���������
	inline void destroy(char*, char*)
	{
		//ʲôҲ������רΪ���Ч��
	}
	inline void destroy(wchar_t*, wchar_t*)
	{

	}

}
#endif