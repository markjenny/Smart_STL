/************************************************************************/
/* ��׼�Ŀռ�������                                                              */
/************************************************************************/
#ifndef _SMART_ALLOCATOR_H_
#define  _SMART_ALLOCATOR_H_
#define  _THROW_BAD_ALLOC_ cerr<<"out of memory" << endl;exit(1) 
#include <new>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include "smart_alloc.h"

using namespace std;
/*��sgi�в���ʹ�����ֿռ�����������Ϊ���̫���ˣ�
ֻ�Ƕ�ȫ�ֵ�operator delete��operator new���е�һ����װ,
Ч�ʲ��Ǻܸ�*/
namespace smart_stl
{
	class allocator
	{
		public:
			static void * allocate(size_t n)
			{
				void *result = malloc(n);
				if (0 == result)
				{
					//malloc����ռ�ʧ��
					result =oom_malloc(n);
				}
				return result;
			}
			//free��ԭ��
			/*����ϵͳ�����ڴ棬ά����һ�������ڴ�����malloc�Ӹ�������ѡ��һ����ʹ�ã�ÿ���ڴ�鶼��һ��ͷ������ʾ����ڴ�Ļ�����Ϣ�����ڴ��С��
			����free ʱ�� �ܹ���סԭ��ָ����ָ���ڴ��С�����������ڴ�����Ƿ��� \0 ����ʱ����ָ���ڴ�Ĵ�С����Ҫ�ַ����ļ��㳤�ȵķ������󵼡�
			����һ��Ҫע��ľ��ǣ�ϵͳ��free �ڴ��ʱ�򣬼�ס��ֻ�� malloc ʱ��ĵ�ַ���� �����ڴ�Ĵ�С��*/
			//����ԭ�����Ϊʲô��malloc_alloc�е�deallocate������ʱ��Ҫ֪��n*sizeof(T)�Ĵ�С
			static void deallocate(void *p, size_t)
			{
				free(p);
			}
			static void * reallocate(void *p, size_t new_sz)
			{
				void *result = realloc(p, new_sz);
				if (0 == result)
				{
					oom_realloc(p, new_sz);
				}
				return result;
			}
			static void* set_malloc_handler(void(*f)())
			{
				void(*old)() = malloc_alloc_oom_handler;
				malloc_alloc_oom_handler = f;
				//old����һ��ָ�룬������old���͵�
				return old;
			}
		
		private:
			//����oom����oom means out of memory
			static void *oom_malloc(size_t n);
			static void *oom_realloc(void *p, size_t n);
			//define the function pointer
			static void (* malloc_alloc_oom_handler)();

		//�Ա�����<<��Ŀ>>Ϊ���䵥λ

	};
	//��Ϊ��Щ�������Ƚ�С��û�б�Ҫ�ٵ���cpp��д��ֱ��������д��
	void (*allocator::malloc_alloc_oom_handler)() = 0;

	void * allocator::oom_malloc(size_t n)
	{
		//oom_malloc��������Ҫ���ڵ����ͷ��ڴ�ĺ���
		void (*my_oom_handler)();
		void *result;

		//���ߵ��ͷš�malloc֪�������ڴ�
		for ( ; ; )
		{
			//sgi���ص��ǳ�Ա�����дӲ�ֱ���������ݳ�Ա���������á��м��������һ��
			my_oom_handler = malloc_alloc_oom_handler;
			if (0 == my_oom_handler)
			{
				_THROW_BAD_ALLOC_;
			}
			//���ú���ָ��ĸ�ʽ
			result = malloc(n);
			if (0 != result)
				return result;
		}
	}
	void *allocator::oom_realloc(void *p, size_t n)
	{
		void (*my_oom_handler)();
		void * result;

		for ( ; ; )//��ͣ������ռ䡢�ͷ��ڴ�ռ䡢����ռ�...ֱ������ڴ������
		{
			my_oom_handler = malloc_alloc_oom_handler;
			if (0 == my_oom_handler)
				_THROW_BAD_ALLOC_;
			(*my_oom_handler)();
			result = realloc(p, n);
			if (0 != result)
				return result;
		}
	}
	//�����ͽ���typedefһ�£��������������������ã�class allocator����һ������
	typedef allocator malloc_alloc;
}
#endif