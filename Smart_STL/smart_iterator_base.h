/********************
��ͷ�ļ��������ǵ������Ļ������ܣ�������insert_back_iterator/insert_front_iterator�ȹ����Լ����������
*********************/
#ifndef _SMART_ITERATOR_BASE_H_
#define _SMART_ITERATOR_BASE_H_
#include <cstddef>	//for ptrdiff_t

namespace smart_stl
{
	struct input_iterator_tag 
	{

	};

	struct output_iterator_tag 
	{

	};

	/*���ּ̳е����ü���Ϊ���ڱ����ʱ��Ϳ���ѡ�����غ����İ汾����ʡЧ�ʣ�����һ����ǵ�ʹ����
	�̳�ʱ�����ڲ�û���������͵ĺ���ʱ������ӵ�м̳й�ϵ����Ե��ø������͵����غ�������Ȼ��Ȼ�����
	�����ݵ��á�*/
	struct forward_iterator_tag : public input_iterator_tag 
	{

	};

	struct bidirectional_iterator_tag : public forward_iterator_tag
	{

	};

	struct random_access_iterator_tag : public bidirectional_iterator_tag
	{

	};

	template<class _Tp, class _Distance>
	struct input_iterator
	{
		typedef input_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	//sgi_stl�ж���output_iterator�ǽ����������������ó���void����Ϊ�˱���ͨ��output_iterator����ȡԪ�ص�ֵ��
	//write only��ֻ�ܸı����ݣ���ô���ǾͲ���Ҫ�������������ֻ�Ǹı�ͺ��ˣ����������֪�����͵�ǿ��ת��
	//���ǻᷢ�����ݽض���
	template<class _Tp, class _Distance>
	struct output_iterator
	{
		typedef output_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<class _Tp, class _Distance>
	struct forward_iterator
	{
		typedef forward_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<class _Tp, class _Distance>
	struct bidirectional_iterator
	{
		typedef bidirectional_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<class _Tp, class _Distance>
	struct random_access_iterator
	{
		typedef random_access_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};


	//֮���Թ����������������Ϊ���Լ�д��������ʱ����п���û��дȫ�������Ƕ��Ӧ�ͱ𣬿����޷���Smart_stl���
	//˳�����䣬���Ե�����Ƶĵ������̳���������������Ϳ��Ա�֤����Smart_STLʹ�ù淶
	template<class _Category,
					 class _Tp,
					 class _Distance = ptrdiff_t,
					 class _Pointer = _Tp*,
					 class _Reference = _Tp&>
	 struct iterator
	 {
		 typedef _Category iterator_category;
		 typedef _Tp value_type;
		 typedef ptrdiff_t difference_type;
		 typedef _Pointer pointer;
		 typedef _Reference reference;
	 };

	 //������������ȡ��
	 template<class _Iterator>
	 struct iterator_traits
	 {
		 //��Ϊ��������֪��_Iterator�е�iterator_category�����ͻ��ǳ�Ա������ͨ��typename����֪������
		 typedef typename _Iterator::iterator_category iterator_category;
		 typedef typename _Iterator::value_type value_type;
		 typedef typename _Iterator::difference_type difference_type;
		 typedef typename _Iterator::pointer pointer;
		 typedef typename _Iterator::reference reference;
	 };
	 //ƫ�ػ�����ԡ�ԭ��ָ�롱
	 template<class _Tp>
	 struct iterator_traits<_Tp*>
	 {
		 typedef random_access_iterator_tag iterator_category;
		 typedef _Tp value_type;
		 typedef ptrdiff_t difference_type;
		 typedef _Tp* pointer;
		 typedef _Tp& reference;
	 };

	 //ƫ�ػ��ġ�ԭ��ָ�롰һ�㶼��ΪT*��const T*����
	 template<class _Tp>
	 struct iterator_traits<const _Tp*>
	 {
		 typedef random_access_iterator_tag iterator_category;
		 typedef _Tp value_type;
		 typedef ptrdiff_t difference_type;
		 typedef _Tp* pointer;
		 typedef _Tp& reference;
	 };

	 /*������Ӧ����ָ��������������ȡ���������ĸ����Ա���ͣ���Ҫ
	 �������������غ����������ϣ�����������غ�������ȷѡ��
	 ��Ϊ���ڵ�������˵���������ݣ������������е�pointer��reference����
	 value_type��ص�����ֻ��������ּ���*/

	 //���ڸú�����С������ֱ������inline��ȡ���ߵ�Ч��
	 template<class Iterator>
	 inline typename iterator_traits<Iterator>::value_type*
		 value_type(const Iterator& )
	 {
		 //��C++�ı�׼����ʾǿ��ת����������static_cast
		 return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	 }
	 
	 template<class Iterator>
	 inline typename iterator_traits<Iterator>::iterator_category
		 iterator_category(const Iterator&)
	 {
		 typedef typename iterator_traits<Iterator>::iterator_category catogory;
		 return catogory();
	 }

	 template<class Iterator>
	 inline typename iterator_traits<Iterator>::difference_type*
		 difference_type(const Iterator&)
	 {
		 return static_cast<typename iterator_traits<Iterator>::difference_type*>(0)
	 }

}
#endif
