# Smart_STL
a smart stl,no thread-safe

Smart_STL目前进度（2015/5/23）


1.拥有二级适配器，二级适配器采用内存池；


申请空间如果多于128bytes，则使用一级适配器；


2.对于vector的构成正在进行，目前写到vector的诸多构造函数；
