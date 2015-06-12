/************************************************************************/
/*author：Mark
该代码的书写习惯：
对于枚举类型的元素全是大写：
对于private类型的函数，函数的名字全用大写；
time: Wednesday,March 18th， 2015
email：lijianlin1990@sina.com
the header is the sub-allocation
*/
/************************************************************************/
#ifndef _SMART_ALLOC_H_
#define _SMART_ALLOC_H_

#include <cstdlib>	//for malloc
#include <cstddef> //for size_t
//直接用二级配置器
namespace smart_stl
{
	class alloc
	{
	public:
		//将与空间适配器相关的函数都定义成static是有原因的，直接把它们定义成static只属于这个类，而不属于任何对象
		static void * allocate(size_t n);
		//因为该模版只有“无模版型别参数”，所以只能用void型指针
		static void deallocate(void *p, size_t n);
		static void * reallocate(void *p, size_t old_sz, size_t new_sz);
	private:
		enum SAlign{ ALIGN = 8 }; //小型区块的调整空间
		enum SMax_Bytes{ MAX_BYTES = 128 }; //小型区块的上限
		enum SFreeList{ FREE_LIST = MAX_BYTES / ALIGN }; //free_lists的个数
		enum SNobjs{ NOBJS = 20 }; //每次缺省增加的节点数
	private:
		//用union主要是来压缩
		typedef union objs
		{
			union objs * next;
			char client[1];
		}objs;
		//因为所有的对象要共享这个内存列表，当然要定义成static类型
		/*
		一个定义为volatile的变量是说这变量可能会被意想不到地改变，这样，编译器就不会去假设这个
		变量的值了。精确地说就是，优化器在用到这个变量时必须每次都小心地重新读取这个变量的值，而
		不是使用保存在寄存器里的备份。
		*/
		//主要是为了多线程
		static union objs * volatile free_list[FREE_LIST];

	//记录内存池的相关数据
	private:
		static char *start_free; //记录内存池的起始处
		static char *end_free; //记录内存的终止处
		static size_t heap_size;

		//向上调整用户申请的内存，使之成为8的整数倍
		inline static size_t ROUND_UP(size_t bytes)
		{
			return (bytes + (ALIGN - 1) & ~(ALIGN - 1));
		}
		//根据所需的内存区块大小来确定使用第几号的free_list
		inline static size_t FREELIST_INDEX(size_t bytes)
		{
			return ( (bytes + ALIGN - 1) / ALIGN - 1);//这段话的意思是向上取整然后减1
		}

	private:
		//重新介入？？？
		static void *REFILL(size_t);
		//分配objs个大小为size的大区块，如果不能满足objs个区块，可以适当降低个数的需求
		static char* CHUNK_ALLOC(size_t size, int& nobjs);
	};


	//为smart_stl的空间配置器alloc包装一层接口，使得符合stl的标准
	template<class T, class Alloc = alloc>
	class simple_alloc
	{
	public:
		//静态成员函数的初始化是不可以放在头文件中的，这样会造成重复定义，但是静态成员函数是可以的
		static T* allocate(size_t n)
		{
			//其实不用考虑n是否为零也是可以的，但那样可能会造成效率较低，所以尽可能减少效率低的情况
			return 0 == n ? (T*)0 : (T*)Alloc::allocate(n * sizeof(T));
		}

		//这种情况一般是对应int()等
		//这四个函数中的n表示的是多少个bytes，允许参数中没有n，但是默认要配置这个类型所需要的bytes数
		static  T* allocate()
		{
			return (T*)Alloc::allocate(sizeof(T));
		}

		static void deallocate(T* p, size_t n)
		{
			if(0 != n)
				Alloc::deallocate(p, n * sizeof(T));
			//void类型的函数一般情况是是不用return的，但是如果函数中间部分就
			//必须退出函数的时候，就要用到return
		}

		static void deallocate(T* p)
		{
			Alloc::deallocate(p, sizeof(T));
		}
	};
}
#endif