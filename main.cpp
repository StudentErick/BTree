#include <iostream>
#include <cstring>
#include <limits>
#include "BTree.h"
using namespace std;

void Menu() {
	cout << "========B-Trees 的设计与实现========" << endl;
	cout << "        153403040424 吕伟华" << endl;
	cout << "操作说明:" << endl;
	cout << "1.建立B-Tree" << endl;
	cout << "2.遍历输出B-Tree" << endl;
	cout << "3.删除元素" << endl;
	cout << "4.插入元素" << endl;
	cout << "5.退出" << endl;
}

int main()
{
	Menu();
	BTree<char>* bt = nullptr;
	bool flag = true;
	while (flag) {
		cout << "请输入操作编号:";
		int n;
		cin.clear();cin.sync();
		cin >> n;
		switch (n) {
		case 1:
			if (bt != nullptr) {
				cout << "树已经存在!" << endl;
				continue;
			}
			else {
				cout << "输入度数m和元素个数n:";
				int m, n;
				cin >> m >> n;
				if (m < 2) {
					cout << "度数太小!" << endl;
					continue;
				}
				bt = new BTree<char>(m);
				cout << "输入" << n << "个元素:" << endl;
				for (int i = 0;i < n;++i) {
					char c;
					cin >> c;
					bt->B_Tree_Insert(c);
				}
			}
			break;
		case 2:
			if (bt == nullptr) {
				cout << "当前没有建立B树!" << endl;
			}
			else if (bt->size() <= 0) {
				cout << "B树中无元素!" << endl;
			}
			else {
				bt->Print();
			}
			break;
		case 3:
			if (bt == nullptr) {
				cout << "当前没有建立B树!" << endl;
			}
			else if (bt->size() <= 0) {
				cout << "B树中无元素!" << endl;
			}
			else {
				cout << "请输入要删除的元素:";
				char k;
				cin >> k;
				bt->B_Tree_Delete(k);
			}
			break;
		case 4:
			if (bt != nullptr) {
				cout << "请输出插入的元素:";
				char c;
				cin >> c;
				bt->B_Tree_Insert(c);
			}
			else {
				cout << "B树未建立！" << endl;
			}
			break;
		case 5:
			delete bt;
			bt = nullptr;
			flag = false;
			break;
		default:
			cout << "输入无效!" << endl;
			cin.clear();            //clear bad input flag
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //适应gnu
			cin.sync();     //适应msvc
		}
	}
	system("pause");
	return 0;
}
