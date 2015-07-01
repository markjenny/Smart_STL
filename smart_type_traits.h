/************************************************************************/
/* 类型萃取器                                                          */
/************************************************************************/
#ifndef _SMART_TYPE_TRAITS_H_
#define  _SMART_TYPE_TRAITS_H_
#define _SMART_STL_TEMPLAT_NULL template<>

namespace smart_stl
{
	//因为萃取的结果往往是利用在重载函数中，所以我们就不能用bool变量来表示，要用类型
	typedef struct _true_type 
	{
	}_true_type;
	typedef struct _false_type
	{
	}_false_type;
	
	/****************
	这个_type_traits中需要加上一个元素“this dummy member must be first”，因为有的编译器是含有类型强化
	的功能，这个是为了告诉我们这个_type_traits与编译器中的那个是不同的，必须要采用我们的这个_type_traits
	*****************/
	template<class T>
	struct _type_traits
	{
		/*将这些诸如has_trivial_default_constructor设置成trivial是因为我们不知道是否是trivial这样我们就需要设置一个
		保守值，当他即使含有trivial的一个函数，我们也按照non-trivial来处理，这样也不会出错，虽然在效率上慢了点
		但是当对象拥有的仅是non-trivial类型的函数，那么他一定不会出错*/
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


		/*针对char的类型设计出了char、unsigned char、signed char是因为有的编译器默认char是
		unsigned char类型,而有的编译器又是将char默认成signed char类型的，所以都要写出来，而signed char
		和unsigned char的主要区别就是对于unsigned char最高位是数据位，而对signed char来说最高位是
		符号位，那么在char像其他bytes数高的数据类型进行转换的时候，unsigned char扩展零即可，但是
		signed char却要扩展符号位*/
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


		//浮点型位数是32bits(32位的处理器)
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

			//4.16.2015 新增加的功能，用于去判断一个类型是否是整数，因为要应用到vector的构造函数中
			template<class T>
			struct is_Integer
			{
				typedef _false_type class_type;
			};

			//该形式的使用是“显示具体化”
		_SMART_STL_TEMPLAT_NULL
			struct is_Integer<int>
			{
				typedef _true_type class_type;
			};

			//该形式的使用是“显示具体化”
			_SMART_STL_TEMPLAT_NULL
			struct is_Integer<unsigned int>
			{
				typedef _true_type class_type;
			};

			//该形式的使用是“显示具体化”
			_SMART_STL_TEMPLAT_NULL
			struct is_Integer<long unsigned int>
			{
				typedef _true_type class_type;
			};



}
#endif