/************************************************************************/
/* 标准的空间适配器                                                              */
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
/*在sgi中并不使用这种空间适配器，因为这个太简单了，
只是对全局的operator delete、operator new进行的一个封装,
效率不是很高*/
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
					//malloc分配空间失败
					result =oom_malloc(n);
				}
				return result;
			}
			//free的原理：
			/*操作系统管理内存，维护了一个空闲内存链表，malloc从个链表中选出一个来使用，每个内存块都有一个头部来表示这个内存的基本信息，如内存大小，
			所以free 时候 能够记住原来指针所指的内存大小，而不是用内存块中是否有 \0 来临时计算指向内存的大小，不要字符串的计算长度的方法所误导。
			还有一点要注意的就是，系统在free 内存的时候，记住的只是 malloc 时候的地址，和 分配内存的大小。*/
			//上述原因就是为什么，malloc_alloc中的deallocate函数中时不要知道n*sizeof(T)的大小
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
				//old即是一个指针，而且是old类型的
				return old;
			}
		
		private:
			//处理oom问题oom means out of memory
			static void *oom_malloc(size_t n);
			static void *oom_realloc(void *p, size_t n);
			//define the function pointer
			static void (* malloc_alloc_oom_handler)();

		//以变量的<<数目>>为分配单位

	};
	//因为这些函数都比较小，没有必要再单开cpp来写，直接在下面写起
	void (*allocator::malloc_alloc_oom_handler)() = 0;

	void * allocator::oom_malloc(size_t n)
	{
		//oom_malloc的作用主要是在调用释放内存的函数
		void (*my_oom_handler)();
		void *result;

		//无线的释放、malloc知道产生内存
		for ( ; ; )
		{
			//sgi的特点是成员函数中从不直接利用数据成员，都是利用《中间变量》导一下
			my_oom_handler = malloc_alloc_oom_handler;
			if (0 == my_oom_handler)
			{
				_THROW_BAD_ALLOC_;
			}
			//调用函数指针的格式
			result = malloc(n);
			if (0 != result)
				return result;
		}
	}
	void *allocator::oom_realloc(void *p, size_t n)
	{
		void (*my_oom_handler)();
		void * result;

		for ( ; ; )//不停地申请空间、释放内存空间、申请空间...直到完成内存的申请
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
	//对类型进行typedef一下，便于清晰看清该类的作用，class allocator这是一个类型
	typedef allocator malloc_alloc;
}
#endif