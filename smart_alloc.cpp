/************************************************************************/
/*author��Mark
�ô������дϰ�ߣ�
����ö�����͵�Ԫ��ȫ�Ǵ�д��
����private���͵ĺ���������������ȫ�ô�д��
time: Wednesday,March 18th�� 2015
email��lijianlin1990@sina.com
the cpp is the sub-allocation
*/
/************************************************************************/
#include "smart_alloc.h"
#include "smart_allocator.h"

namespace smart_stl
{
	//��ʼstatic member data��������������������Ӧ�������ڶ�������԰����Ƕ���Ϊstatic���ͣ�
	//���ǲ��������н��ж���
	char * alloc::start_free = 0;
	char * alloc::end_free = 0;
	//�ڴ�صĴ�С������������Ϊ�����ڴ������ʲô������һ����׼
	size_t alloc::heap_size = 0;

	//volatile��˵���������ܻᱻ���벻���ĸı䣬����Ҫ���������б��룬ÿһ�ζ���С�ĵĶ�ȡ
	alloc::objs * volatile alloc::free_list[FREE_LIST] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	//static��̬��Ա������Ȼ˵��������Ķ��󣬵�����������ģ�������ô����û������
	//allocate�ķ���ֵΪvoid���ͣ����Ǳ���ģ���Ϊ�����Ŀռ��������ǿ���ҪӦ�õ��������͵�������
	//nΪ������ܵ�bytes
	void * alloc::allocate(size_t n)
	{
		objs * volatile * my_free_list;
		//�����ܸо���stl��һЩ���ǣ���Ϊ�������ڲ�������ǿռ䣬���Զ����objs���͵�ָ�����������������˼
		//�ɶ���ǿ��
		objs* result;
		//���n����free_list���������飬�����allocator�ĺ���
		if (n > (size_t)MAX_BYTES)
		{
			return ( malloc_alloc::allocate(n) );
		}
		//���û�д���128bytes����ô,�͵���free_list�е����鼴��
		my_free_list = free_list + FREELIST_INDEX(n);

		//��Ϊ����пռ�Ļ�����ô���ǻ�������������еĵ�һ����Ϊ����
		result = *my_free_list;
		//��������������Ѿ�û���˿��õ�����
		if (0 == result)
		{
			//��n������8������Ȼ��ȥ���ڴ�ػ�ȡ�ռ䣬�������ȡ�����黹��ֱ�ӷ���FREELIST_INDEX(n)��
			void *r = REFILL(ROUND_UP(n));
			return r;
		}

		*my_free_list = result->next;
		//��C++��ʲô���͵�ָ�붼�ǿ��Ժ�void*����ת�����������ǿ���ֱ��return result��
		//���������⽫resultת����void*��
		//return (void*)result;
		return result;
	}

	void alloc::deallocate(void *p, size_t n)
	{
		//��Ϊ��allocate��deallocate�ж��漰��free_list������������Ҫ�õ�objs** my_free_list
		objs* volatile * my_free_list;
		//��Ϊ����Ҫ��ָ��p��������÷�
		objs* q = (objs*)p;

		if (n > MAX_BYTES)
		{
			malloc_alloc::deallocate(p, n);
			//���ʱ���return�Ǳ���Ҫʹ�õģ���Ϊ����Ҫ�������ļ������128bytes��ô�ͽ�����������
			//�����ľͲ�ʲô���������ˣ���Ҫ�õ�return
			return;
		}

		//��ʱ��˵������Ҫ�����Ŀ�����freelist�У���Ϊ�����������ʱ����ǰ���8�ı�����������ģ�
		//��ô������ʱ��һ��Ҳ��8�ı���������ֱ�Ӱ��������ֱ�ӷ��ؾͺ�
		//?����:����������һ���Լ���������ݽṹ�������ݽṹ��ʹ����һ��ָ�룬����ȥ��ָ����
		//�������ڴ������128bytes���ڣ���ô��һ����8�ı��������Կ���ֱ�ӷŵ��ڴ�ص��������С�
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
		char* chunk;//������������ָ��ľ�������

		chunk = CHUNK_ALLOC(n, nobjs);

		//��result���ó�objs���Ϳ����Ǹ��ӱ��ڳ����߼������
		objs* result;
		objs *cur_obj, *next_obj;

		//stl����һ���ض����������������Ŀ�������ǰ����ж���
		int i;

		//ֻ�Ƿֵ���һ������
		if (1 == nobjs)
		{
			//��Ϊʲô���Ͷ��ǿ�����ʽת����void*��
			return chunk;
		}

		//�ֵ��˶�����飬����ǰ�������������ú�����ָ�룬ʣ�µ�����ŵ�free_list��
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
				//��ִ�е����һ����ʱ���ĵ���cur_objs����û��next_obj!!
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

	//�ڴ�صĹ���
	char* alloc::CHUNK_ALLOC(size_t size, int& nobjs)
	{
		char* result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;
		//�ڴ���еĿռ��㹻Ӧ����������ڴ�
		if ( bytes_left > total_bytes )
		{
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		//���û����ô��Ŀռ䣬���ǿ��Ը���һ��<�������ϵ�>size�Ŀռ�
		else if ( bytes_left > size )
		{
			nobjs = bytes_left / size;
			result = start_free;
			start_free += nobjs * size;
			return result;
		}
		//����ڴ������һ��Ҳ�ṩ���˵Ļ�
		else
		{
			//�ڴ����������Ĳ���
			size_t bytes_to_get = 2 * total_bytes + (heap_size >> 4);
			//bytes_left����0��˵���ڴ���ܻ�����һЩС������飬��ô���������ŵ�free_list��
			if (bytes_left > 0)
			{
				//byte_left����8�ı���
				objs* volatile * my_free_list;
				my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((objs*)start_free)->next = *my_free_list;
				*my_free_list = (objs*)start_free;
			}

			//���ڴ����ʣ��Ŀռ����֮�󣬿�ʼ���ÿռ�
			start_free = (char*)malloc(bytes_to_get);
			//˵��û�з���ɹ�
			if (0 == start_free)
			{
				//��ô���ǽ��ӱ�size���free_list���鿴����û�п��Է��������
				objs* volatile * my_free_list;
				objs* p;
				int i;
				for (i = size; i < MAX_BYTES; i+=ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (0 != p)//˵����index�������п��е�����
					{
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;

						//ע�������÷���������˵ݹ飬�ֿ��Ա�֤�˳���ѭ��
						return (CHUNK_ALLOC(size, nobjs));//nobjs��������
					}
				}
				//�˳���ѭ����˵��free_list��Ҳ��û�п����õĿռ���
				end_free = 0;
				start_free = (char*)(malloc_alloc::allocate(bytes_to_get));
			}
			heap_size = bytes_to_get;
			end_free = start_free + heap_size;
			//��Ϊ���ͨ��malloc��malloc_alloc::allocate�Ⱥ����Ѿ����ڴ���������µ��ڴ棬���Կ����ٴε��ø��㷨
			return (CHUNK_ALLOC(size, nobjs));
		}

	}
}
