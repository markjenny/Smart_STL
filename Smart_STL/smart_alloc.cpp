/************************************************************************/
/*author：Mark
该代码的书写习惯：
对于枚举类型的元素全是大写：
对于private类型的函数，函数的名字全用大写；
time: Wednesday,March 18th， 2015
email：lijianlin1990@sina.com
the cpp is the sub-allocation
*/
/************************************************************************/
#include "smart_alloc.h"
#include "smart_allocator.h"

namespace smart_stl
{
	//初始static member data，很明显这三个变量不应该是属于对象的所以把他们定义为static类型，
	//但是不能在类中进行定义
	char * alloc::start_free = 0;
	char * alloc::end_free = 0;
	//内存池的大小，他的作用是为后续内存池增大到什么程序做一个标准
	size_t alloc::heap_size = 0;

	//volatile是说明变量可能会被意想不到的改变，不需要编译器进行编译，每一次都是小心的读取
	alloc::objs * volatile alloc::free_list[FREE_LIST] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	//static静态成员变量虽然说不属于类的对象，但是是属于类的，所以这么定义没有问题
	//allocate的返回值为void类型，这是必须的，因为后续的空间配置器是可能要应用到各种类型的数据中
	//n为申请的总的bytes
	void * alloc::allocate(size_t n)
	{
		objs * volatile * my_free_list;
		//这里能感觉到stl的一些考虑，因为我们在内部申请的是空间，所以定义成objs类型的指针是易于理解程序的意思
		//可读性强，
		objs* result;
		//如果n大于free_list中最大的区块，则调用allocator的函数
		if (n > (size_t)MAX_BYTES)
		{
			return ( malloc_alloc::allocate(n) );
		}
		//如果没有大于128bytes，那么,就调用free_list中的区块即可
		my_free_list = free_list + FREELIST_INDEX(n);

		//因为如果有空间的话，那么我们会在这个区块链中的第一个作为返回
		result = *my_free_list;
		//如果该区块链中已经没有了可用的区块
		if (0 == result)
		{
			//将n调整成8倍数，然后去向内存池获取空间，这样多获取的区块还能直接放在FREELIST_INDEX(n)处
			void *r = REFILL(ROUND_UP(n));
			return r;
		}

		*my_free_list = result->next;
		//在C++中什么类型的指针都是可以和void*进行转换，所以我们可以直接return result，
		//而不用特意将result转换成void*型
		//return (void*)result;
		return result;
	}

	void alloc::deallocate(void *p, size_t n)
	{
		//因为在allocate和deallocate中都涉及到free_list，所以我们需要用到objs** my_free_list
		objs* volatile * my_free_list;
		//因为我们要用指针p完成两种用法
		objs* q = (objs*)p;

		if (n > MAX_BYTES)
		{
			malloc_alloc::deallocate(p, n);
			//这个时候的return是必须要使用的，因为我们要析构的文件如果是128bytes那么就交给它来做，
			//后续的就不什么都不用做了，就要用到return
			return;
		}

		//这时候说明我们要析构的块是在freelist中，因为我们在申请的时候就是按照8的倍数进行申请的，
		//那么析构的时候一定也是8的倍数，这样直接把这个区块直接返回就好
		//?问题:如果该区块给一个自己定义的数据结构，而数据结构中使用了一个指针，不用去管指针吗？
		//如果这个内存块是在128bytes以内，那么它一定是8的倍数，所以可以直接放到内存池的索引表中。
		my_free_list = free_list + FREELIST_INDEX(n);
		q->next = *my_free_list;
		*my_free_list = q;
	}

	void* alloc::reallocate(void* p, size_t old_sz, size_t new_sz)
	{
		deallocate(p, old_sz);
		p = allocate(new_sz);
		return p;
	}

	void* alloc::REFILL(size_t n)
	{
		int nobjs = NOBJS;

		objs* volatile *my_free_list;
		char* chunk;//便于我们利用指针的距离数据

		chunk = CHUNK_ALLOC(n, nobjs);

		//把result设置成objs类型可能是更加便于程序逻辑的理解
		objs* result;
		objs *cur_obj, *next_obj;

		//stl的另一个特定，将变量方在最大的可行域最前面进行定义
		int i;

		//只是分到了一个区块
		if (1 == nobjs)
		{
			//因为什么类型都是可以隐式转换成void*型
			return chunk;
		}

		//分到了多个区块，将最前面的区块给到调用函数的指针，剩下的区块放到free_list中
		my_free_list = free_list + FREELIST_INDEX(n);

		result = (objs*)chunk;
		next_obj = (objs*)(chunk + n);
		*my_free_list = next_obj;

		for (i = 1; ; i++)
		{
			cur_obj = next_obj;
			next_obj = (objs*)((char*)next_obj + n);
			if (i + 1 == nobjs)
			{
				//当执行到最后一个点时，改点是cur_objs，他没有next_obj!!
				cur_obj->next = 0;
				break;
			}
			else
			{
				cur_obj->next = next_obj;
			}
		}
		return result;
	}

	//内存池的管理
	char* alloc::CHUNK_ALLOC(size_t size, int& nobjs)
	{
		char* result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;
		//内存池中的空间足够应付所申请的内存
		if ( bytes_left > total_bytes )
		{
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		//如果没有那么多的空间，但是可以给他一个<或者以上的>size的空间
		else if ( bytes_left > size )
		{
			nobjs = bytes_left / size;
			result = start_free;
			start_free += nobjs * size;
			return result;
		}
		//如果内存池中连一块也提供不了的话
		else
		{
			//内存池容量增大的策略
			size_t bytes_to_get = 2 * total_bytes + (heap_size >> 4);
			//bytes_left大于0，说明内存池总还是有一些小额的区块，那么将这个区块放到free_list中
			if (bytes_left > 0)
			{
				//byte_left都是8的倍数
				objs* volatile * my_free_list;
				my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((objs*)start_free)->next = *my_free_list;
				*my_free_list = (objs*)start_free;
			}

			//将内存池中剩余的空间分配之后，开始配置空间
			start_free = (char*)malloc(bytes_to_get);
			//说明没有分配成功
			if (0 == start_free)
			{
				//那么我们将从比size大的free_list区块看看有没有可以分配给他的
				objs* volatile * my_free_list;
				objs* p;
				int i;
				for (i = size; i < MAX_BYTES; i+=ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (0 != p)//说明该index的链区有空闲的区块
					{
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;

						//注意这种用法，即完成了递归，又可以保证退出了循环
						return (CHUNK_ALLOC(size, nobjs));//nobjs用于修正
					}
				}
				//退出了循环，说明free_list中也是没有可以用的空间了
				end_free = 0;
				start_free = (char*)(malloc_alloc::allocate(bytes_to_get));
			}
			heap_size = bytes_to_get;
			end_free = start_free + heap_size;
			//因为这次通过malloc、malloc_alloc::allocate等函数已经将内存池填入了新的内存，所以可以再次调用该算法
			return (CHUNK_ALLOC(size, nobjs));
		}

	}
}
