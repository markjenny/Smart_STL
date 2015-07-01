/************************************************************************/
/* ������ȡ��                                                          */
/************************************************************************/
#ifndef _SMART_TYPE_TRAITS_H_
#define  _SMART_TYPE_TRAITS_H_
#define _SMART_STL_TEMPLAT_NULL template<>

namespace smart_stl
{
	//��Ϊ��ȡ�Ľ�����������������غ����У��������ǾͲ�����bool��������ʾ��Ҫ������
	typedef struct _true_type 
	{
	}_true_type;
	typedef struct _false_type
	{
	}_false_type;
	
	/****************
	���_type_traits����Ҫ����һ��Ԫ�ء�this dummy member must be first������Ϊ�еı������Ǻ�������ǿ��
	�Ĺ��ܣ������Ϊ�˸����������_type_traits��������е��Ǹ��ǲ�ͬ�ģ�����Ҫ�������ǵ����_type_traits
	*****************/
	template<class T>
	struct _type_traits
	{
		/*����Щ����has_trivial_default_constructor���ó�trivial����Ϊ���ǲ�֪���Ƿ���trivial�������Ǿ���Ҫ����һ��
		����ֵ��������ʹ����trivial��һ������������Ҳ����non-trivial����������Ҳ���������Ȼ��Ч�������˵�
		���ǵ�����ӵ�еĽ���non-trivial���͵ĺ�������ô��һ���������*/
		typedef _true_type this_dummy_member_must_be_first;
		typedef _false_type has_trivial_default_constructor;
		typedef _false_type has_trivial_copy_constructor;
		typedef _false_type has_trivial_assignment_operator;
		typedef _false_type has_trivial_destructor;
		typedef _false_type is_POD_type;
	};

	_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<char>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};


		/*���char��������Ƴ���char��unsigned char��signed char����Ϊ�еı�����Ĭ��char��
		unsigned char����,���еı��������ǽ�charĬ�ϳ�signed char���͵ģ����Զ�Ҫд��������signed char
		��unsigned char����Ҫ������Ƕ���unsigned char���λ������λ������signed char��˵���λ��
		����λ����ô��char������bytes���ߵ��������ͽ���ת����ʱ��unsigned char��չ�㼴�ɣ�����
		signed charȴҪ��չ����λ*/
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<signed char>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<unsigned char>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<short>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<unsigned short>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<int>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<unsigned int>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<long>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<unsigned long>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};


		//������λ����32bits(32λ�Ĵ�����)
		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<float>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};

		_SMART_STL_TEMPLAT_NULL 
		struct _type_traits<double>
		{
			typedef _true_type has_trivial_default_constructor;
			typedef _true_type has_trivial_copy_constructor;
			typedef _true_type has_trivial_assignment_operator;
			typedef _true_type has_trivial_destructor;
			typedef _true_type is_POD_type;
		};

		_SMART_STL_TEMPLAT_NULL
			struct _type_traits<wchar_t>
			{ 
				typedef _true_type has_trivial_default_constructor;
				typedef _true_type has_trivial_copy_constructor;
				typedef _true_type has_trivial_assignment_operator;
				typedef _true_type has_trivial_destructor;
				typedef _true_type is_POD_type;
			};

					template<class T>
			struct _type_traits<T*>
			{
				typedef _false_type has_trivial_default_constructor;
				typedef _false_type has_trivial_copy_constructor;
				typedef _false_type has_trivial_assignment_operator;
				typedef _false_type has_trivial_destructor;
				typedef _false_type is_POD_type;
			};

			//4.16.2015 �����ӵĹ��ܣ�����ȥ�ж�һ�������Ƿ�����������ΪҪӦ�õ�vector�Ĺ��캯����
			template<class T>
			struct is_Integer
			{
				typedef _false_type class_type;
			};

			//����ʽ��ʹ���ǡ���ʾ���廯��
		_SMART_STL_TEMPLAT_NULL
			struct is_Integer<int>
			{
				typedef _true_type class_type;
			};

			//����ʽ��ʹ���ǡ���ʾ���廯��
			_SMART_STL_TEMPLAT_NULL
			struct is_Integer<unsigned int>
			{
				typedef _true_type class_type;
			};

			//����ʽ��ʹ���ǡ���ʾ���廯��
			_SMART_STL_TEMPLAT_NULL
			struct is_Integer<long unsigned int>
			{
				typedef _true_type class_type;
			};



}
#endif