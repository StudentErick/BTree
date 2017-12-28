#ifndef BTREE_H
#define BTREE_H

#include<cstdlib>
#include<queue>

namespace BT {
	template<typename T>
	struct Node {
		int n;
		struct Node<T>** child;   //孩子的地址
		T* key;    //关键字
		bool leaf;  //是否为叶子的标记
		Node(int t = 0) { n = t;child = nullptr; key = nullptr;leaf = true; }
		~Node() { n = 0;child = nullptr; key = nullptr; }
	};
}

template<typename T>
class BTree {
public:
    BTree(int m = 2) { t = m;root = new BT::Node<T>;node_num = 0; }
	~BTree() { PostOrder_Delete(root); }
	BT::Node<T>* B_Tree_Search(const T& elem) { return search(root, elem); }  //查找元素
    void B_Tree_Insert(const T& k) { Insert(root, k); }//插入元素k
	void B_Tree_Delete(const T &k);					    //删除元素k
	void Print();										//结构化输出
    int size()const { return node_num; }                        //返回元素的个数
private:
	//释放整个占用的空间，给析构函数调用
	void PostOrder_Delete(BT::Node<T>*& cur);  //后根次序删除

	//查找部分
	BT::Node<T>* search(BT::Node<T>* r, const T& elem);

	//插入部分
    void B_Tree_Split_Child(BT::Node<T>* x, int i);//x的i孩子裂变
    void Insert(BT::Node<T>* &r, const T& k);//这里一点要传入指针的引用，否则会无法完成实际树的增长
	void B_Tree_Insert_NonFull(BT::Node<T>* x, const T& k); //插入非空元素

	//删除部分
	void Merge_Node(BT::Node<T>*& x, int i, BT::Node<T>*& y, BT::Node<T>*& z);  //和并x的第i个子结点y和第i+1个子结点z，此时y和z的key个数都是t-1
	T Search_Predecessor(BT::Node<T>* y); //查找前驱
	T Search_Successor(BT::Node<T>* z);   //查找后继
	void Shift_To_Left_Child(BT::Node<T>*& x, int i, BT::Node<T>*& y, BT::Node<T>*& z);
	void Shift_To_Right_Child(BT::Node<T>*& x, int i, BT::Node<T>*& y, BT::Node<T>*& z);
	void B_Tree_Delete_NoNone(BT::Node<T>* x, const T &k);//删除内部结点x的k

	//基础数据
	BT::Node<T>* root;	//根结点
	int t;              //度数，默认是2
    int node_num;       //结点个数
};

template<typename T>
BT::Node<T>* BTree<T>::search(BT::Node<T>* x, const T& k) {
	int i = 0;
	while (i<x->n&&k>x->key[i]) {
		++i;
	}
	if (i < x->n&&k == x->key[i]) {   //找到了，返回地址
		return x;
	}
	else if (x->leaf) {        //未找到，返回空
		return nullptr;
	}
	else {
		return search(x->child[i], k);   //尾递归的向下查找
	}
}

template<typename T>
void BTree<T>::B_Tree_Split_Child(BT::Node<T>* x, int i) { //i是数组的下标
	BT::Node<T>* y = x->child[i];
	BT::Node<T>*L, *R;   //L R是新裂变的两个片段
	L = new BT::Node<T>;
	R = new BT::Node<T>;
	L->n = R->n = t - 1;
	L->key = new T[t - 1];
	R->key = new T[t - 1];
	L->leaf = R->leaf = x->child[i]->leaf;  //别忘了更改孩子的属性
	//复制键值
	for (int j = 0;j <= t - 2;++j) {
		L->key[j] = y->key[j];
		R->key[j] = y->key[j + t];
	}
	if (!y->leaf) {
		L->child = new BT::Node<T>*[t];
		R->child = new BT::Node<T>*[t];
		for (int j = 0;j < t;++j) {
			L->child[j] = y->child[j];
			R->child[j] = y->child[j + t];
		}
	}
	//处理x
	x->key = (T*)realloc(x->key, (x->n + 1) * sizeof(T));
	x->child = (BT::Node<T>**)realloc(x->child, (x->n + 2) * sizeof(BT::Node<T>*));
	for (int j = x->n - 1;j >= i;--j) { //键值后移一位
		x->key[j + 1] = x->key[j];
	}
	x->key[i] = y->key[t - 1];
	for (int j = x->n;j >= i + 1;--j) {
		x->child[j + 1] = x->child[j];
	}
	x->child[i] = L;
	x->child[i + 1] = R;
	x->n++;
	//销毁原来的空间
	delete[] y->child;
	delete[] y->key;
	delete y;
}

template<typename T>
void BTree<T>::Insert(BT::Node<T>* &r, const T& k) {
	if (B_Tree_Search(k)) {
		return;
	}
	else if (r != nullptr && r->n == 2 * t - 1) {
		BT::Node<T>* s = new BT::Node<T>;
		s->leaf = false;
		s->child = new BT::Node<T>*[1];
		s->child[0] = r;
		r = s;
		s->n = 0;
		B_Tree_Split_Child(s, 0);
		B_Tree_Insert_NonFull(s, k);
	}
	else {
		B_Tree_Insert_NonFull(r, k);
	}
    node_num++;  //结点个数增加
}

template<typename T>
void BTree<T>::B_Tree_Insert_NonFull(BT::Node<T>* x, const T& k) {
	int i = x->n - 1;
	if (x->leaf) {  //叶子结点直接插入
		x->key = (T*)realloc(x->key, (x->n + 1) * sizeof(T));
		while (i >= 0 && k < x->key[i]) {
			x->key[i + 1] = x->key[i];
			--i;
		}
		x->key[i + 1] = k;
		++x->n;
	}
	else {
		while (i >= 0 && k < x->key[i]) {
			--i;
		}
		++i;
		if (x->child[i]->n == 2 * t - 1) {
			B_Tree_Split_Child(x, i);
			if (k > x->key[i]) {
				++i;
			}
		}
		B_Tree_Insert_NonFull(x->child[i], k);  //尾递归
	}

}

template<typename T>
void BTree<T>::Merge_Node(BT::Node<T>*& x, int i, BT::Node<T>*& y, BT::Node<T>*& z) {
	y->key = (T*)realloc(y->key, (2 * t - 1) * sizeof(T));
	for (int j = 0;j < t - 1;++j) {
		y->key[t + j] = z->key[j];
	}
	//不是叶子结点的情况下，还需要复制孩子
	if (!y->leaf) {
		y->child = (BT::Node<T>**)realloc(y->child, 2 * t * sizeof(BT::Node<T>**));
		for (int j = 0;j < t;++j) {
			y->child[t + j] = z->child[j];
		}
	}
	y->key[t - 1] = x->key[i];  //移动x的结点补充到y
	y->n = 2 * t - 1;
	//删除x的下移关键字
	for (int j = i;j < x->n - 1;++j) {
		x->key[j] = x->key[j + 1];
	}
	x->key = (T*)realloc(x->key, (x->n - 1) * sizeof(T));
	//删除原来指向z的指针
	for (int j = i + 1;j <= x->n - 1;++j) {
		x->child[j] = x->child[j + 1];
	}
	x->child = (BT::Node<T>**)realloc(x->child, (x->n) * sizeof(BT::Node<T>**));
	x->child[i] = y;
	--x->n;
	//释放掉结点z
	delete[] z->key;
	delete[] z->child;
	delete z;
	z = nullptr;
}

template<typename T>
T BTree<T>::Search_Predecessor(BT::Node<T>* y) {
	BT::Node<T>* x = y;//->child[y->n];
	while (!x->leaf) {
		x = x->child[x->n];
	}
	return x->key[x->n - 1];
}

template<typename T>
T BTree<T>::Search_Successor(BT::Node<T>* z) {
	BT::Node<T>* x = z;//->child[0];
	while (!x->leaf) {
		x = x->child[0];
	}
	return x->key[0];
}

template<typename T>
void BTree<T>::Shift_To_Left_Child(BT::Node<T>*& x, int i, BT::Node<T>*& y, BT::Node<T>*& z) {
	if (y == nullptr) {  //防止出现空指针
		y = new BT::Node<T>;
	}
	++y->n;
	//复制键值
	y->key = (T*)realloc(y->key, y->n * sizeof(T));
	y->key[y->n - 1] = x->key[i];
	x->key[i] = z->key[0];
	for (int j = 0;j < z->n - 1;++j) {
		z->key[j] = z->key[j + 1];
	}
	//非叶子结点的情况下复制孩子
	if (!z->leaf) {
		y->child = (BT::Node<T>**)realloc(y->child, (y->n + 1) * sizeof(BT::Node<T>*));
		y->child[y->n] = z->child[0];
		for (int j = 0;j < z->n;++j) {
			z->child[j] = z->child[j + 1];
		}
		z->child = (BT::Node<T>**)realloc(z->child, z->n * sizeof(BT::Node<T>*));
	}
	--z->n;
	z->key = (T*)realloc(z->key, z->n * sizeof(T));
}

template<typename T>
void BTree<T>::Shift_To_Right_Child(BT::Node<T>*& x, int i, BT::Node<T>*& y, BT::Node<T>*& z) {
	if (z == nullptr) { //防止出现空指针
		z = new BT::Node<T>;
	}
	++z->n;
	//复制键值
	z->key = (T*)realloc(z->key, z->n * sizeof(T));
	for (int j = z->n - 1;j > 0;--j) {
		z->key[j] = z->key[j - 1];
	}
	z->key[0] = x->key[i];
	x->key[i] = y->key[y->n - 1];
	//非叶子结点的情况下复制孩子
	if (!z->leaf) {
		z->child = (BT::Node<T>**)realloc(z->child, (z->n + 1) * sizeof(BT::Node<T>*));
		for (int j = 0;j < z->n;++j) {
			z->child[j + 1] = z->child[j];
		}
		z->child[0] = y->child[y->n];
		y->child = (BT::Node<T>**)realloc(y->child, y->n * sizeof(BT::Node<T>*));
	}
	--y->n;
	y->key = (T*)realloc(y->key, y->n * sizeof(T));
}

template<typename T>
void BTree<T>::B_Tree_Delete_NoNone(BT::Node<T>* x, const T &k) {
	//下面提到的各种情况全部参照算法导论
	if (x->leaf) {   //情况1
		int i = 0;
		while (i<x->n&&k>x->key[i]) {
			++i;
		}
		for (int j = i + 1;j < x->n;++j) {
			x->key[j - 1] = x->key[j];
		}
		--x->n;
		x->key = (T*)realloc(x->key, x->n * sizeof(T));
	}
	else {
		int i = 0;
		while (i<x->n&&k>x->key[i]) {
			++i;
		}
		BT::Node<T> *y = x->child[i], *z = nullptr;
		if (i < x->n) {
			z = x->child[i + 1];
		}
		if (k == x->key[i]) { //情况2
			if (y->n > t - 1) { //情况2a
				T k1 = Search_Predecessor(y);
				B_Tree_Delete_NoNone(y, k1);
				x->key[i] = k1;
			}
			else if (z->n > t - 1) { //情况2b
				T k1 = Search_Successor(z);
				B_Tree_Delete_NoNone(z, k1);
				x->key[i] = k1;
			}
			else {  //情况2c
				Merge_Node(x, i, y, z);
				B_Tree_Delete_NoNone(y, k);
			}
		}
		else { //情况3
			BT::Node<T>* p = nullptr;
			if (i > 0) {
				p = x->child[i - 1];
			}
			if (y->n == t - 1) {
				if (i > 0 && p->n > t - 1) {  //情况3a
					Shift_To_Right_Child(x, i - 1, p, y);
				}
                else if (i<x->n&&z->n>t - 1) { //情况3a
					Shift_To_Left_Child(x, i, y, z);
				}
				else if (i > 0) { //情况3b
					Merge_Node(x, i - 1, p, y);  //向左侧结点合并
					y = p;
				}
				else {          //情况3b
					Merge_Node(x, i, y, z);      //向右侧合并
				}
			}
			B_Tree_Delete_NoNone(y, k);
		}
	}
}
 
template<typename T>
void BTree<T>::B_Tree_Delete(const T &k) {
	BT::Node<T>* r = root;
	if (root->n == 1 && root->child == nullptr) {  //删除最后一个元素
		delete[] root->key;
		delete root;
		root = nullptr;
	}
	else if (root->n == 1){  //压缩根节点
		BT::Node<T> *y = root->child[0], *z = root->child[1];
		if (y->n == z->n&&y->n == t - 1) {
			Merge_Node(root, 0, y, z);
			root = y;
			delete[] r->child;
			delete[] r->key;
			delete r;
			B_Tree_Delete_NoNone(y, k);
		}
		else {
			B_Tree_Delete_NoNone(root, k);
		}
	}
	else {
		B_Tree_Delete_NoNone(r, k);
	}
    --node_num;  //结点个数减少一个
}

template<typename T>
void BTree<T>::Print() {
    BT::Node<T> *last = root;//last表示当前层的最右结点
	BT::Node<T>* p = root;
    std::queue<BT::Node<T>*>Q;
	if (p) {
		Q.push(p);
	}
	while (!Q.empty()) {
		BT::Node<T>* tmp = Q.front();
		Q.pop();
		if (!tmp->leaf) {
            std::cout << "|";
		}
        std::cout << "(";
		for (int i = 0;i < tmp->n;++i) {
            std::cout << tmp->key[i];
			if (tmp->child&&i != tmp->n - 1) {
                std::cout << "|";
			}
		}
        std::cout << ")";
		if (!tmp->leaf) {   //不是叶子结点才有孩子
            std::cout << "|";
			for (int i = 0;i < tmp->n + 1;++i) {
				Q.push(tmp->child[i]);
			}
		}
		if (!last->leaf&&tmp == last) {
            std::cout << std::endl;
			last = last->child[last->n];
		}
		
	}
    std::cout << std::endl;
}

template<typename T>
void BTree<T>::PostOrder_Delete(BT::Node<T>*& cur) {
    if (cur->leaf) {  //直接删除叶子结点
		delete[] cur->key;
		delete cur;
		cur = nullptr;
	}
    else { //递归地删除每个孩子
		for (int i = 0;i < cur->n + 1;++i) {
			PostOrder_Delete(cur->child[i]);
		}
		delete[] cur->key;
		delete cur;
		cur = nullptr;
	}
}

#endif
