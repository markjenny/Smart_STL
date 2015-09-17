#ifndef _SMART_RB_TREE_H_
#define _SMART_RB_TREE_H_
#include <stddef.h>
#include <utility>
#include "smart_iterator_base.h"
#include "smart_alloc.h"
//����Ϊ����ײ����ݺ��������ѧϰΪ������Ϊ��̫����������ɾ�����̣����㷨���й��ڽڵ�ɾ����˼�������ģ����Ǿ��廹��̫������Դ����֣�
//��Ҫ˵�����ǣ�������ġ����С�����keyΪ���ģ�����Ԫ������Ԫ�ص�valueΪ׼��
namespace smart_stl
{
	//���ڶ���ڵ���ɫ
	typedef bool rb_tree_color_type;
	const rb_tree_color_type rb_tree_red = false;		//bool����Ϊfalse�ı�ʾ��ɫ�ڵ�
	const rb_tree_color_type rb_tree_black = true;		//��ɫ�ڵ���true��ʾ

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

		//���ó�static����Ϊ��ʹ��maximum��minimumΪrb_tree_node_base����
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

			//����ʵ���Ӧ���Ƕ�Ӧ˵�����node_base�ǵ�ǰ���е�����һ���㣬��ô��Ӧ����++��header�ĵط������Դ����������˵���������Ӧ���ǲ��Եġ�
			//�������������Ĵ�����⣬��Դ���������к����ʦ˵������ȷ��
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

	template<class Value, class Ref, class Ptr>
	struct rb_tree_iterator;
	
	template<class Value, class Ref, class Ptr>
	bool operator == (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs);

	template<class Value, class Ref, class Ptr>
	bool operator != (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs);

	//���ݻ��������������ɰ������
	template<class Value, class Ref, class Ptr>
	struct rb_tree_iterator : public rb_tree_iterator_base
	{
		typedef Value value_type;
		typedef Ref reference;
		typedef PTR pointer;
		//ΪʲôҪ����������Щ�����һ���û̫�㶮
		typedef rb_tree_iterator<Value, Value&, Value*> iterator;
		typedef rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
		typedef rb_tree_iterator<Value, Ref, Ptr> self;
		/////////////////////////////////////////////////////
		typedef rb_tree_node<Value>* link_type;

		//��������������·һ������typedef�������ͣ�Ȼ���캯�����������ز�������++��--��*��->�� == ����=
		rb_tree_iterator(){}
		//�����������÷���slist��ֱ��̫���ˣ�����Ϊʲô�һ���̫������鿴�Ļ����١�rb_tree_iterator������rb_tree_node�����ģ�
		//��Ȼ��Ϊ���캯���ĺ�������Ϊ�桿
		rb_tree_iterator(link_type x) {node_base = x;}
		rb_tree_iterator(const iterator&) {node_base = iterator.node_base;}

		reference operator* (){ return link_type(node_base)->node_value;}
		pointer operator->() {return &(operator*());}

		self& operator++()
		{
			increment();		//���ڵ�������ǰ��
			return *this;
		}
		self operator++(int)
		{
			self temp = *this;
			increment();
			return temp;
		}

		self& operator-- ()
		{
			decrement();	//���ڵ������ĺ���
			return *this;
		}

		self operator++ (int)
		{
			self temp = *this;
			decrement();	//���ڵ������ĺ���
			return self;
		}

		//ʹ�õ��ǰ��͵����ز���������
		friend bool operator == (const self& lhs, const self& rhs);
		friend bool operator != (const self& lhs, const self& rhs);
	};

	template<class Value, class Ref, class Ptr>
	bool operator == (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs)
	{
		return lhs.base_ptr == rhs.base_ptr;
	}

	template<class Value, class Ref, class Ptr>
	bool operator != (const rb_tree_iterator<Value, Ref, Ptr>& lhs, const rb_tree_iterator<Value, Ref, Ptr>& rhs)
	{
		return lhs.base_ptr != rhs.base_ptr;
	}

	//������ڵ����ݽṹ,KeyofValueӦ����һ���º���
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc = alloc>
	class rb_tree
	{
	public:
		//������ֻ��rb_tree��Ҫ�õ���һЩtype
		typedef void* void_pointer;
		typedef rb_tree_node_base* base_ptr;
		typedef rb_tree_node<Value> rbtree_node;
		typedef simple_alloc<rbtree_node, Alloc> rb_tree_node_allocator;
		typedef rb_tree_color_type color_type;
		//Ϊʲô�Ὣrb_tree_node<Value>*�趨Ϊ link_type�������ǽ�rb_tree_node_base�趨Ϊlink_type�����˾���iterator��iterator_base��
		//rb_tree_node��rb_tree_node_base֮����һһ��Ӧ�ģ�rb_tree_node�̳���rb_tree_node_base��iterator�̳���iterator_base
		//Ȼ��iterator_base����rb_tree_node_base���̹��죬iterator����rb_tree_node���й���

		typedef rb_tree_node<Value>* link_type;
		typedef Key key_type;

		//���Ŷ���ÿ�����ݶ���Ҫ��type��value_type��pointer��
		typedef Value value_type;
		typedef Value& reference;
		typedef Value* pointer;
		typedef const Value* const_pointer;
		typedef const Value& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t distance_type;

		//����iterator
		typedef rb_tree_iterator<value_type, reference, pointer> iterator;
		typedef rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

	private:
		//����ڵ�ȵȣ���������������ݽṹ����һ������
		link_type get_node() //������ִ��������Ǽ�����˼��������������涨��֮��Ӧ�ûḲ�ǵ�
		{
			return rb_tree_node_allocator::allocate();
		}

		void put_node(link_type p)
		{
			rb_tree_node_allocator::deallocate(p);
		}

		//�����ڵ�
		link_type create_node(const value_type& x)
		{
			link_type temp = get_node();
			construct(&(temp->node_value), x);
			return temp;
		}

		//���ƽڵ㣺��Ҫ�Ǹ��ƽڵ����ɫ��ֵ
		link_type clone_node(link_type x)
		{
			link_type temp = create_node(x->node_value);
			temp->color = x->color;
			temp->left = 0; //(��C++�У��κ����Ͷ�����0ֵ�ã�����BJ���Ǻ�ϲ����~)
			temp->right = 0;
			return temp;
		}

		//ɾ���ڵ�
		void destroy_node(link_type x)
		{
			destroy(&x->node_value);
			put_node(x);
		}

	private:
		//��������������ʾ���ź����
		size_type amountOfNode;
		link_type header;
		Compare key_compare;		//�ڵ�ıȽϹ���һ��Ӧ�ö���BST��

	private:
		//�����������˶�̬�ԣ�����root��left�ȶ��ǻ�ñ��壬��ô�������ã�����Ϊʲô��Դ��������Ҫ�ã�link_type&����ת���أ���
		//��hearder���
		link_type& root() 
		{
			return (link_type)(header->parent);		//��Ϊ���������е�parent��base���ͣ���Ҫת��һ��ָ��Ľ��ͷ�ʽ
		}
		link_type& leftmost()
		{
			return (link_type)(header->left);
		}
		link_type& rightmost()
		{
			return (link_type)(header->right);
		}

		//��ȡһ���ڵ�������Ϣ
		link_type& left(link_type& x)
		{
			return (link_type)(x->left);
		}
		link_type& right(link_type& x)
		{
			return (link_type)(x->right);
		}
		link_type& parent(link_type& x)
		{
			return (link_type)(x->parent);
		}
		reference value(link_type& x)
		{
			return x->node_value;
		}
		//��Ϊ���key�����ܸı䣬�����ں�������ʼ������const
		const Key& key(link_type& x)
		{
			return KeyofValue()(x->node_value);
		}
		color_type& color(link_type& x)
		{
			return (color_type)(x->color);
		}

		//֮ǰ��ȡһ���ڵ����Ϣ�����õ��������ָ�룬���ݶ�̬�Կ��Կ��ǻ����ָ��
		link_type& left(base_ptr& x)
		{
			return (link_type)(x->left);
		}
		link_type& right(base_ptr& x)
		{
			return (link_type)(x->right);
		}
		link_type& parent(base_ptr& x)
		{
			return (link_type)(x->parent);
		}
		color_type& color(base_ptr& x)
		{
			return (color_type)(x->color);
		}
		value_type& value(base_ptr& x)
		{
			return ((link_type)x)->node_value;
		}
		const Key& key(base_ptr& x)
		{
			return KeyofValue()(value(x));
		}
		link_type minimum(link_type x)		//���˾��ü�Ȼ��iterator��������Ͳ���Ҫ����ʲô��base_ptr�Ĺ�ϵ
		{
			//��effective C++�е�˵����link_type is a base_ptr
			return (link_type) rb_tree_node_base::minimum(x);		//��Ϊ���ܳ��ָ���
		}
		link_type maximum(link_type x)
		{
			return (link_type) rb_tree_node_base::maximum(x);
		}

		//���ϵĺ������ǿ���ͨ��֮ǰ���������Щ�������㶨��������Ҫ���������
		public:
			typedef rb_tree_iterator<value_type, reference, pointer> iterator;
		private:
			//���ܣ���sonλ���ϲ���xֵ�Ľڵ㣬son�ĸ��ڵ�Ϊparent
			iterator _insert(base_ptr son, base_ptr parent, const value_type& x);
			//û�㶮copy������
			link_type _copy(link_type x, link_type p);
			void _erase(link_type x);		//��ΪҪ���ǵ�ɾ���ڵ㣬��ô�û����������node_value����������������ʹ��liink_type���ӱ�Ҫ
			void _init()
			{
				header = get_node();
				//����header������Ҫ�ĸ�����Ϣ����Ϊ�����õ�header��ֵ���������ﲻ�ó�ʼ����
				color(header) = rb_tree_red;
				root() = 0;
				rightmost() = header;
				leftmost() = header;
			}
		public:
			//������Ҫ�����ձ����������Ҫ����������ĺ���
			//1.�빹�죬�������йأ�
			rb_tree(const Compare& comp = Compare()) : amountOfNode(0), key_compare(comp) {_init();}
			~rb_tree()
			{
				clear();
				destroy_node(header);
			}
			rb_tree& operator= (const rb_tree& x);
			//2.���߼��Ƚ��йأ�
			//3.��������size�йأ�
			bool empty() const {return amountOfNode == 0;}
			size_type size() {return amountOfNode;}
			size_type max_size(){return size_type(-1);}
			//��������ʲô��(չʾkey�ıȽϹ���)
			Compare key_comp() const {return key_compare;}
			//4.�������ĵ������йأ�
			iterator begin(){return leftmost();}
			iterator end() {return header;}

			iterator find(const Key& k);
			//5.��������Ԫ���йأ����Ƕ�����Ӧ����û�е�
			//6.��ı�������أ�
			pair<iterator, bool> insert_unique(const value_type& x);		//����
			iterator insert_equal(const value_type& x);
			void clear();
			//ָ���ʹ��������ͱ�׼���ɣ��������ƽ�⺯���У��ڵ���ɫ������rb_tree_node_base�У�����ʹ��base_ptr����
			void rb_tree_rebalance(rb_tree_node_base* new_node, rb_tree_node_base* root_node);
			rb_tree_node_base* rb_tree_rotate_left(rb_tree_node_base* new_node);
			rb_tree_node_base* rb_tree_rotate_right(rb_tree_node_base* new_node);
	};
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyofValue, Compare, Alloc>::iterator rb_tree<Key,Value, KeyofValue, Compare, Alloc>::insert_equal(const value_type&x)
	{
		link_type parent = header;
		link_type son = root();

		while (son != 0)
		{
			//BST��ʽ������������
			parent = son;
			son = key_compare(KeyofValue()(x), key(son)) ? left(son) : right(son);
		}
		return _insert(son, parent, x);
	}

	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	pair<typename rb_tree<Key, Value, KeyofValue, Compare, Alloc>::iterator, bool> 
		rb_tree<Key, Value, KeyofValue, Compare, Alloc>::insert_unique(const value_type& x)
	{
		link_type parent = header;
		link_type son = root();
		bool comp = true;
		while (son != 0)
		{
			parent = son;
			comp = key_compare(KeyofValue()(x), key(son)) ? left(son) : right(son);
			son = comp ? left(son) : right(son); 
		}

		//��ʱparentΪҶ�ӽڵ㣬sonΪ0��λ��
		iterator j = iterator(parent);		//��ΪҪ�õ�������++
		if(comp)
		{
			if (j == begin())
				return pair<iterator, bool>(_insert(son, parent, x), bool);
			else
				j--;
		}

		if(key_compare(key(j.node)), KeyofValue()(x));
			return pair<iterator, bool>(_insert(son, parent, x), true);

		return pair<iterator, bool>(j, false);
	}
	//��ͨ�Ĳ����������Ϊ֮ǰ��insert_unique���Ѿ�
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyofValue, Compare, Alloc>::iterator 
		rb_tree<Key, Value, KeyofValue, Compare, Alloc>::_insert(base_ptr son, base_ptr parent, const value_type& x)
	{
		//�о�x������û�ж����ԣ�������������д��������е�Ӱ��
		//��Ҫ��Ϊ����������߷Ż������ұ߷�
		link_type new_node = 0;
		//���е�X!=0��������һ���û�����̫������ȷŵ�����
		if(header == parent || son != 0 || key_compare(KeyofValue()(x), key(parent)))
		{
			new_node = create_node(x);
			left(parent) = new_node;
			if (header == parent)
			{
				root() = new_node;
				rightmost() = new_node;
				//����header������Ҫ�ı��һ�������ϵ������if�ж������У��Ե�����
			}
			if (parent == leftmost())
				leftmost() = new_node;
		}
		else
		{
			//���뵽���ӽڵ���
			right(parent) = new_node;
			if (parent == rightmost())
				rightmost() = new_node;
		}
		parent(new_node) = parent;
		left(new_node) = 0;
		right(new_node) = 0;

		//���������˧��һ�㣺ƽ������Ϊ2-3�������ڵ���ɫƽ��
		rb_tree_rebalance(new_node, header->parent);
		amountOfNode++;
		return iterator(new_node);
	}
	//�������Ľڵ���ɫ����һ������Ϊ�½ڵ�����λ��
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyofValue, Compare, Alloc>::rb_tree_rebalance(rb_tree_node_base* new_node, rb_tree_node_base* root_node)
	{
		//�о���û��̫���������ɫת���������Ҿ���ʹ�á��㷨4���е��㷨
		//�²���Ľڵ�һ���Ǻ�ɫ�ĵ�
		new_node->color = rb_tree_red;
		//������ĵ����Ǿֲ��ȶ�������ֻ������x�йصľͿ����ˣ�
		//ͬʱ�������ĸ��ڵ㣬�ж��Ƿ��Ǻ�ɫ�ڵ㣬���Ϊ��ɫ��˵������2-3���У�����һ��3�ڵ�
		//����while�м���ʾ���ĸ��ڵ���һ��2-3���е�2�ڵ㣬ֱ�ӽ��в��뼴��
		while (new_node != root() new_node->parent->color == rb_tree_red)
		{
			if (new_node->parent == new_node->parent->parent->left)		//���ڵ��������ĺ�ڵ�
			{
				rb_tree_node_base* uncle = new_node->parent->right;
				if(uncle != NULL && uncle->color == rb_tree_red)			//flip the color()
				{
					new_node->parent->color = rb_tree_black;
					uncle->color = rb_tree_black;
					new_node->parent->parent->color = rb_tree_red;
					new_node = new_node->parent->parent;
				}
				else																						//��uncle�ڵ����uncle�ڵ�Ϊ��ɫ
				{
					new_node = new_node->parent;
					rb_tree_rotate_left(new_node, header->parent);
				}
			}
		}
	}

	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	rb_tree_node_base* rb_tree<Key, Value, KeyofValue, Compare, Alloc>::rb_tree_rotate_left(rb_tree_node_base* new_node)
	{
		rb_tree_node_base* temp = new_node->right;
		new_node->right = temp->left;
		temp->left = new_node;
		temp->color = new_node->color;
		new_node->color = rb_tree_red;

		return temp;
	}

	//����ת��Ҫ��Ӧ���ڵ����ڵ���ɫ�ϣ�
	//1.����ڵ���Ҷ��ӵ���ɫΪ�죬��ô�ͽ�������ת��
	//2.����ڵ������ӣ������ӽڵ�����ӽڵ�ҲΪ�죬��ô�Ըõ��������(���������ô����ˣ�)
	//3.����ڵ�������ӽڵ㶼Ϊ��ɫ��flip the color
	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	rb_tree_node_base* rb_tree<Key, Value, KeyofValue, Compare, Alloc>::rb_tree_rotate_right(rb_tree_node_base* new_node)
	{
		rb_tree_node_base* temp = new_node->left;
		new_node->left = temp->right;
		temp->right = new_node;
		temp->color = new_node->color;
		new_node->color = rb_tree_red;

		return temp;
	}

	template<class Key, class Value, class KeyofValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyofValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyofValue, Compare, Alloc>::find(const Key& k)
	{
		//��find��������Ȼû�г��ֵ��ںţ�����ͨ��ʹ��key_compare����º���ȷʵ���Թ�����>=, <= ,����ͨ���任����������λ�ã�����
		//����ʵ��==��Ҫ��
		link_type y = header;		//stl�еĸ���֮���������ڵ���k����С�ļ�����Ϊy
		link_type current_node = root();

		//û��������ʱ����������˵���ˬ
		while (current_node != NULL)
			if (!key_compare(key(current_node), k))			//��ǰ���keyֵ���ڵ���k
				y = current_node, current_node = left(current_node);
			else
				current_node = right(current_node);

		//�˳�ѭ�����y��Ȼ�Ǵ��ڵ���k����Сһ��
		iterator j = iterator(current_node);
		//��Ϊ֮ǰ�ж���j��keyֵ�Ǵ��ڵ���k��keyֵ�ģ������������Ƚ�һ�£����kС��j��keyֵ����ô��û������Ⱥ��ˣ�����һ����û���ҵ���
		//˼·̫���ˣ�
		return (end() == j || key_compare(k, key(j.node_base))) ? end() : j;
	}


}

#endif