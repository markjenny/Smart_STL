#include <iostream>
#include<time.h>
#include <stdio.h>
#include <Windows.h>
#include "smart_vector.h"
#include "smart_list.h"
#include "smart_deque.h"
#include <vector>
#include <list>
using namespace std;

int main()
{

	// 	smart_stl::list<int> a;
	// 	a.push_back(1);
	// 	a.push_back(13);
	// 	a.push_back(2);
	// 	a.push_back(7);
	// 	a.push_back(6);
	// 	a.push_back(8);
	// 	a.push_back(19);
	// 
	// 	smart_stl::list<int> b;
	// 	b.push_back(32);
	// 	b.push_back(42);
	// 	b.push_back(52);
	// 	b.push_back(53);
	// 	b.push_back(54);
	// 	b.push_back(55);
	// 	b.push_back(56);
	// 
	// 	if (a == b)
	// 	{
	// 		cout <<"a和b相同" << endl;
	// 	}
	// 	else
	// 	{
	// 		cout << "a和b不同" << endl;
	// 	}
	// 
	// 	
	// 	a.pop_back();
	// 	a.push_back(100);
	// 	a.pop_front();
	// 	a.push_front(1000);
	// 	for (smart_stl::list<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	// 	{
	// 		cout << *iter << endl;
	// 	}
	// 	cout << endl;
	// 
	// 	if (!a.empty())
	// 	{
	// 		cout <<"list a 的长度为：" << a.size() << endl;
	// 	}
	// 	cout << endl;
	// 
	// 	cout << "list a 的首元素是：" << *(a.begin())  << endl;
	// 	cout << "list a 的空元素是：" << *(a.end())  << endl;
	// 	cout << endl;
	// 
	// 	a.insert(a.begin(), b.begin(), b.end());
	// 	for (smart_stl::list<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	// 	{
	// 		cout << *iter << endl;
	// 	}
	// 	cout << endl;
	// 
	// 
	// 	a.swap(b);
	// 	for (smart_stl::list<int>::iterator iter = b.begin(); iter != b.end(); iter++)
	// 	{
	// 		cout << *iter << endl;
	// 	}
	// 	cout << endl;

	smart_stl::deque<int> a(10,22);
	smart_stl::deque<int>::iterator iter = a.begin(); 
	iter++;
	//for (int i = 0; i < 10; i++)
	a.insert(iter,10, 50);
	/*a.insert(iter,50);*/
	for (smart_stl::deque<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	{
		int a;
		a = *iter;
		cout << *iter << endl;
	}

	cout << "the size of a is: " << a.size() << endl;

	smart_stl::deque<int> b = a;
	if (a == b)
		cout << "deque a is the same with b" << endl;
	else
		cout << "a is different with b" << endl;

	// 	smart_stl::deque<int>::iterator iter = a.begin();
	// 	smart_stl::deque<int>::iterator iter2 = a.end();
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	iter++;
	// 	if (iter == a.end())
	// 	{
	// 		cout << "iter与a.end相等" << endl;
	// 	}
	// 	else
	// 		cout << "iter与a.end不等" << endl;



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 	 		double start,end,cost;
	// 	 		start=clock();
	// 	 		/*Sleep(1);*/
	// 	 	 	smart_stl::vector<int> smart_a;
	// 	 	 	for (int i = 0; i < 10000; i++)
	// 	 	 	{
	// 	 	 		smart_a.push_back(i);
	// 	 	 		//cout << a.back() << endl;
	// 	 	 	}
	// 	 		end=clock();
	// 	 		cost= end - start;
	// 	 		cout << "利用smart_stl库中的push_back一百万个数时所用时间为：" << 1000 * cost/CLOCKS_PER_SEC << "s" << endl;
	// 	 
	// 	 	double start1,end1,cost1;
	// 	 	start1=clock();
	// 	 	//int aa = start;
	// 	 	/*Sleep(1);*/
	// 	 	vector<int> a;
	// 	 	for (int i = 0; i < 10000; i++)
	// 	 	{
	// 	 		a.push_back(i);
	// 	 		//cout << a.back() << endl;
	// 	 	}
	// 	 	end1=clock();
	// 	 	//int bb = end;
	// 	 	cost1= end1 - start1;
	// 	 	cout << "利用sgi_stl库中的push_back一百万个数时所用时间为：" << 1000 * cost1/CLOCKS_PER_SEC << "s" << endl;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 	 		double start,end,cost;
	// 	 		start=clock();
	// 	 		/*Sleep(1);*/
	// 			smart_stl::vector<int> a;
	// 			a.push_back(1);
	// 			a.push_back(2);
	// 			a.push_back(3);
	// 			a.push_back(4);
	// 			a.push_back(5);
	// 
	// 			smart_stl::vector<int> b;
	// 			b.push_back(11);
	// 			b.push_back(12);
	// 			b.push_back(13);
	// 			b.push_back(14);
	// 			b.push_back(15);
	// 
	// 			a.insert(a.begin()+2, b.begin(), b.end());
	// 
	// 			for (smart_stl::vector<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	// 			{
	// 				cout << *iter << endl;
	// 			}
	// 	 		end=clock();
	// 	 		cost= end - start;
	// 	 		cout << "利用smart_stl库中的push_back一百万个数时所用时间为：" << cost/CLOCKS_PER_SEC << "s" << endl;
	// 	 
	// 	 	double start1,end1,cost1;
	// 	 	start1=clock();
	// 		smart_stl::vector<int> a1;
	// 		a1.push_back(1);
	// 		a1.push_back(2);
	// 		a1.push_back(3);
	// 		a1.push_back(4);
	// 		a1.push_back(5);
	// 
	// 		smart_stl::vector<int> b1;
	// 		b1.push_back(11);
	// 		b1.push_back(12);
	// 		b1.push_back(13);
	// 		b1.push_back(14);
	// 		b1.push_back(15);
	// 
	// 		a1.insert(a1.begin()+2, b1.begin(), b1.end());
	// 
	// 		for (smart_stl::vector<int>::iterator iter = a1.begin(); iter != a1.end(); iter++)
	// 		{
	// 			cout << *iter << endl;
	// 		}
	// 	 	end1=clock();
	// 	 	//int bb = end;
	// 	 	cost1= end1 - start1;
	// 	 	cout << "利用sgi_stl库中的push_back一百万个数时所用时间为：" << cost1/CLOCKS_PER_SEC << "s" << endl;




	//a.erase(a.begin());
	// 	for (smart_stl::vector<int>::iterator iter = a.begin(); iter != a.end(); iter++)
	// 	{
	// 		cout << *iter << endl;
	// 	}

	//cout << "smart_stl::vector插入一百万个数所用时间为：" << cost <<endl;
	//bool equal_this;
	/*	if(a == b)*/
	// 		equal_this = true;
	// 	std::cout << equal_this << std::endl;
	// 	std::cout << *a.begin() << std::endl;
	// 	std::cout << *b.begin() << std::endl;
	// 	cout << "a的长度是：" << a.size() << endl;
	return 0;
}
