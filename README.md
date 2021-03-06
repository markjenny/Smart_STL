对于建造轮子期初于蓝色巨巨的建议，因为刚好那段没有事情就把《源码剖析》看了一遍，其中的一些数据结构和算法我感觉很有效，所以就拿来练手一下。现在这个smart_stl暂时没有线程安全，我准备在将所有的功能组件大致完成后，看看《C++并发编程》来改善线程安全。
**暂时不添加异常处理机线程安全**
# Smart_STL
a smart stl,no thread-safe


#Smart_STL目前进度（2015/7/18）

##smart_stl组件进度：

1.construct完成进度：100%；

2.alloc（二级适配器）完成进度：100%；

3.allocator（一级配置器）完成进度：100%；

4.smart_iterator_base完成进度：50%；

5.smart_type_traits完成进度：100%；

6.smart_uninitialized完成进度：100%；

7.smart_vector完成进度：100%；

8.smart_list完成进度：100%；

9.smart_functional完成进度：100%；

10.smart_deque完成进度：100%；

11.smart_stack完成进度：100%;

12.smart_queue完成进度：100%；

11.smart_heap完成进度：100%；

12.smart_slist完成进度：30%；

13.smart_RB_tree完成进度：30%；

13.smart_set完成进度：10%；


##smart_stl算法进度：

1.fill_n完成进度：100%；

2.copy完成进度：100%；

3.copy_backward完成进度：100%；

4.swap完成进度：100%；

5.fill完成进度：100%；

6.lexicalgraphical_compare完成进度：100%；



#smart_st测试环境为MicroSoft Visual Studio 2008 && Windows & && Debug模式


###vector相关测试：

```
#include <iostream>
#include "smart_vector.h"
using namespcace std;

int main()
{
	double start,end,cost;
	start=clock();
	smart_stl::vector<int> smart_a;
	for (int i = 0; i < 1000000; i++)
	{
		smart_a.push_back(i);
		//cout << a.back() << endl;
	}
	end=clock();
	cost= end - start;
	cout << "利用smart_stl库中的push_back一百万个数时所用时间为：" << cost/CLOCKS_PER_SEC << "s" << endl;
}
```

| 数量\时间       | smart_stl   |  sgi stl  |
| -------   | -----:  | :----:  |
| 1W     | 0.001s |   0.004s     |
| 10W    |   0.006s   |   0.038s   |
| 100W   |    0.057s    |  0.39s  |

###list相关测试：
```
int a[10] = {12, 23 ,34 ,45, 56, 67, 78, 89, 90, 100};
smart_stl::list<int> b(a, a + 10);
b.reverse();
for (smart_stl::list<int>::iterator iter = b.begin(); iter != b.end(); iter++)
{
cout << *iter << " ";
}

cout << endl;

b.sort();
for (smart_stl::list<int>::iterator iter = b.begin(); iter != b.end(); iter++)
{
cout << *iter << " ";
}
```

输出结果：

```
100 90 89 78 67 56 45 34 23 12
12 23 34 45 56 67 78 89 90 100
```


###deque相关测试 
```
smart_stl::deque<int> a(10,22);
smart_stl::deque<int>::iterator iter = a.begin(); 
smart_stl::deque<int>b;
b.push_back(1);
b.push_back(2);
b.push_back(3);
b.push_back(4);
b.push_back(5);
iter++;
a.insert(iter, 2, 50);
a.insert(iter, b.begin(), b.end());
for (smart_stl::deque<int>::iterator iter = a.begin(); iter != a.end(); iter++)
{
	int a;
	a = *iter;
	cout << *iter << " ";
}
cout << endl;
cout << "the size of a is: " << a.size() << endl;
```

输出结果：
```
22 50 50 1 2 3 4 5 22 22 22 22 22 22 22 22 22
the size of a is: 17
```

测试代码(在上段代码的基础上)
```
cout << a[4] << endl;
cout << a.at(15) << endl;
```

输出结果：
```
2
22
```
测试代码(在上段代码的基础上)
```
a.erase(temp);
for (smart_stl::deque<int>::iterator iter = a.begin(); iter != a.end(); iter++)
{
	int a;
	a = *iter;
	cout << *iter << " ";
}
cout << "the size of a is: " << a.size() << endl;
```

输出结果：
```
22 50 50 1 3 4 5 22 22 22 22 22 22 22 22 22
the size of a is: 16
```

测试代码:
```
a.erase(temp, temp + 5);
	for (smart_stl::deque<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	{
		int a;
		a = *iter;
		cout << *iter << " ";
	}
	cout << "the size of a is: " << a.size() << endl;
```

输出结果：
```
22 50 50 1 22 22 22 22 22 22 22 22
the size of a is: 12
```

###heap相关测试：
```
int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
vector<int> ivec(ia, ia+9);

make_heap(ivec.begin(), ivec.end());
for(int i = 0; i < ivec.size(); ++i)
	cout << ivec[i] << endl;
cout << endl;

```

![heap相关测试结果](http://7xjxcs.com1.z0.glb.clouddn.com/heap%E7%9B%B8%E5%85%B3%E6%B5%8B%E8%AF%95%E7%BB%93%E6%9E%9C.png "heap相关测试结果")


###slist相关测试：
```
smart_stl::slist<int> a;
a.push_front(5);
a.push_front(6);
a.push_front(7);
a.push_front(8);
for (smart_stl::slist<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	cout << *iter << endl;
```

###输出结果：
![slist相关测试结果](http://7xjxcs.com1.z0.glb.clouddn.com/slist测试结果.png "slist相关测试结果")
