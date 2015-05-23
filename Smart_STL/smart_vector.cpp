#include "../smart_vector.h"

namespace smart_stl
{
	template<class T, class Alloc>
	vector<T, Alloc>::vector(const size_type n)
	{
		fill_initialize(n, T());
	}
	template<class T, class Alloc>
	vector<T, Alloc>::~vector()
	{
		destroy(start_, finish_);
		deallocate();
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::deallocate()
	{
		//�ж�start_�Ƿ�Ϊnull
		if (start_)
		{
			data_allocator::deallocate(start_, end_of_storage_ - start_);//end_of_storage_��end_����ǰ����յ�����
		}
	}
}