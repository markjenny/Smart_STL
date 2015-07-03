/************************************************************************/
/*author��Mark
�ô������дϰ�ߣ�
����ö�����͵�Ԫ��ȫ�Ǵ�д��
����private���͵ĺ���������������ȫ�ô�д��
time: Wednesday,March 18th�� 2015
email��lijianlin1990@sina.com
the header is the sub-allocation
*/
/************************************************************************/
#ifndef _SMART_ALLOC_H_
#define _SMART_ALLOC_H_

#include <cstdlib>	//for malloc
#include <cstddef> //for size_t
//ֱ���ö���������
namespace smart_stl
{
	class alloc
	{
	public:
		//����ռ���������صĺ����������static����ԭ��ģ�ֱ�Ӱ����Ƕ����staticֻ��������࣬���������κζ���
		static void * allocate(size_t n);
		//��Ϊ��ģ��ֻ�С���ģ���ͱ������������ֻ����void��ָ��
		static void deallocate(void *p, size_t n);
		static void * reallocate(void *p, size_t old_sz, size_t new_sz);
	private:
		enum SAlign{ ALIGN = 8 }; //С������ĵ����ռ�
		enum SMax_Bytes{ MAX_BYTES = 128 }; //С�����������
		enum SFreeList{ FREE_LIST = MAX_BYTES / ALIGN }; //free_lists�ĸ���
		enum SNobjs{ NOBJS = 20 }; //ÿ��ȱʡ���ӵĽڵ���
	private:
		//��union��Ҫ����ѹ��
		typedef union objs
		{
			union objs * next;
			char client[1];
		}objs;
		//��Ϊ���еĶ���Ҫ��������ڴ��б���ȻҪ�����static����
		/*
		һ������Ϊvolatile�ı�����˵��������ܻᱻ���벻���ظı䣬�������������Ͳ���ȥ�������
		������ֵ�ˡ���ȷ��˵���ǣ��Ż������õ��������ʱ����ÿ�ζ�С�ĵ����¶�ȡ���������ֵ����
		����ʹ�ñ����ڼĴ�����ı��ݡ�
		*/
		//��Ҫ��Ϊ�˶��߳�
		static union objs * volatile free_list[FREE_LIST];

	//��¼�ڴ�ص��������
	private:
		static char *start_free; //��¼�ڴ�ص���ʼ��
		static char *end_free; //��¼�ڴ����ֹ��
		static size_t heap_size;

		//���ϵ����û�������ڴ棬ʹ֮��Ϊ8��������
		inline static size_t ROUND_UP(size_t bytes)
		{
			return (bytes + (ALIGN - 1) & ~(ALIGN - 1));
		}
		//����������ڴ������С��ȷ��ʹ�õڼ��ŵ�free_list
		inline static size_t FREELIST_INDEX(size_t bytes)
		{
			return ( (bytes + ALIGN - 1) / ALIGN - 1);//��λ�����˼������ȡ��Ȼ���1
		}

	private:
		//���½��룿����
		static void *REFILL(size_t);
		//����objs����СΪsize�Ĵ����飬�����������objs�����飬�����ʵ����͸���������
		static char* CHUNK_ALLOC(size_t size, int& nobjs);
	};


	//Ϊsmart_stl�Ŀռ�������alloc��װһ��ӿڣ�ʹ�÷���stl�ı�׼
	template<class T, class Alloc = alloc>
	class simple_alloc
	{
	public:
		//��̬��Ա�����ĳ�ʼ���ǲ����Է���ͷ�ļ��еģ�����������ظ����壬���Ǿ�̬��Ա�����ǿ��Ե�
		static T* allocate(size_t n)
		{
			//��ʵ���ÿ���n�Ƿ�Ϊ��Ҳ�ǿ��Եģ����������ܻ����Ч�ʽϵͣ����Ծ����ܼ���Ч�ʵ͵����
			return 0 == n ? (T*)0 : (T*)Alloc::allocate(n * sizeof(T));
		}

		//�������һ���Ƕ�Ӧint()��
		//���ĸ������е�n��ʾ���Ƕ��ٸ�bytes�����������û��n������Ĭ��Ҫ���������������Ҫ��bytes��
		static  T* allocate()
		{
			return (T*)Alloc::allocate(sizeof(T));
		}

		static void deallocate(T* p, size_t n)
		{
			if(0 != n)
				Alloc::deallocate(p, n * sizeof(T));
			//void���͵ĺ���һ��������ǲ���return�ģ�������������м䲿�־�
			//�����˳�������ʱ�򣬾�Ҫ�õ�return
		}

		static void deallocate(T* p)
		{
			Alloc::deallocate(p, sizeof(T));
		}
	};
}
#endif