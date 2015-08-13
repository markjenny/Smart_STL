#ifndef _SMART_RB_TREE_H_
#define _SMART_RB_TREE_H_
#include "smart_iterator_base.h"
#include <stddef.h>
//����Ϊ����ײ����ݺ��������ѧϰΪ������Ϊ��̫����������ɾ�����̣����㷨���й��ڽڵ�ɾ����˼�������ģ����Ǿ��廹��̫������Դ����֣�
namespace smart_stl
{
	//���ڶ���ڵ���ɫ
	typedef bool rb_tree_color_type;
	const rb_tree_color_type rb_tree_red = false;		//bool����Ϊfalse�ı�ʾ��ɫ�ڵ�
	const rb_tree_color_type br_tree_black = true;		//��ɫ�ڵ���true��ʾ

	//effectivec++�еģ�����class type�޹صĴ������ģ�壬����base�������������ȫ���ŵ�base����
	struct rb_tree_node_base
	{
		typedef rb_tree_node_base* base_ptr;
		typedef rb_tree_color_type color_type;

		color_type color;
		base_ptr parent;
		base_ptr left;
		base_ptr right;

		//�������base����������������base�����������ʲôԭ���Ҳ���̫�������������Ϊ��̬��
		//Ѱ�����ֵ����Ϊ�������һ��2-3������2-3��ת���ɶ��������Ƕ���������
		static base_ptr minimum(base_ptr x)
		{
			while(x->left != NULL)
				x = x->left;
			return x;
		}

		static base_ptr maximum(base_ptr x)
		{
			while (x->right != NULL)
				x = x->right;
			return x;
		}
	};

	//������д������Ͳ����Ĳ���
	template<class Value>
	struct rb_tree_node : public rb_tree_node_base
	{
		//��rb_tree_node�н������link_type�����壿
		typedef rb_tree_node<Value>* link_type;
		Value node_value;
	};

	//���û��������������������ͻ���ڵ㽨�����ӣ�
	struct rb_tree_iterator_base		//���iterator_base��Ҫ����node_baseΪԭ��
	{
		typedef rb_tree_node_base::base_ptr base_ptr;
		//retree�еĵ������ǿ���ǰ���ߵģ����ǲ��߱���λ������ǰ�����˵�˳��ȡ���ڶ�����������˳��
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t distance_type;
		//ʣ������value_type��pointer��reference���ǲ���������صģ���������û���趨
		base_ptr node_base;		//�����ڵ���������һ�����ӣ�����list�е��÷�
		void increment();		//���ڵ�������ǰ��
		void decrement();	//���ڵ������ĺ���
	};

	//ֻҪ�����ڲ����ɾ����ʱ�򣬺������ʵ��һ��BST����ͬ��
	void rb_tree_iterator_base::increment()
	{
		//��������һ��ʱ��㶮��increment����ʵ���Ͷ������еĻ��ǲ��ģ�������һ�ǳ�����ĵط�����stl�е�rb_tree����ƣ������м�����header
		//�ṹ������ṹ��header�������Ҷ��������ֵ�����������Сֵ�����ĸ��ڵ㾹Ȼ��root����ʱ�ͻ��������������rootû���Ҷ���
		if(node_base->right != NULL)
		{
			node_base = node_base->right;		//ȡ��������Ȼ������С���Ǹ�
			while(node_base != NULL)
				node_base = node_base->left;
		}
		else
		{
			//���������û�У���ô�Լ�����һ�����������㣬��ôҪ�ұ�������ģ���Ҫ��֤����һ����������������㣬�����������++����
			base_ptr node_base_parent  = node_base->parent;
			while (node_base == node_base_parent->right)
			{
				node_base = node_base_parent;
				node_base_parent = node_base->parent;
			}

			//��ʵ���Ӧ���Ƕ�Ӧ˵�����node_base�ǵ�ǰ���е�����һ���㣬��ô��Ӧ����++��header�ĵط������Դ����������˵���������Ӧ���ǲ��Ե�
			if (node_base->right != node_base_parent)
				node_base = node_base_parent;
		}
	}

	void rb_tree_iterator_base::decrement()
	{
		//�������������������end(),Ҳ����header��Ļ���header���root��һ���������hearder�Ǻ�ɫ�ģ������ڵ����Ǻ�ɫ�ģ�
		if (node_base->color == rb_tree_red && node_base->parent->parent == node_base)
			node_base = node_base->right;
		if (node_base->left != 0)	//�ýڵ����������� ��ô����������������
		{
			node_base = node_base->left;
			while(node_base->right != NULL)
				node_base = node_base->right;
		}
		else
		{
			//���û������ӣ���˵������һ�������е���Сֵ����ô�ұ�����һ���ľ�Ӧ���Ǹ������ĸ��ڵ㣬������������Ǹ��ڵ��������
			base_ptr node_base_parent = node_base->parent;
			while (node_base == node_base_parent->left)
			{
				node_base = node_base_parent;
				node_base_parent = node_base->parent;
			}
			node_base = node_base_parent;
		}
	}
}

#endif