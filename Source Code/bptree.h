#ifndef _INDEX_H_
#define _INDEX_H_
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "BufferManager.h"

#define BLOCKSIZE 4096
#define MESSAGE 24
#define POINTERLENGTH 20
//block内结构，前20位记录当前节点信息，后键值信息分为四块：第一块为键值，第二块为兄弟键值位置
//第三块为叶子节点独有，记录文件对应位置；对应的内节点记录下一块地址(比自己小的)，第四块为当前编号位置,第五块专门记比自己大的地址
//节点第一部分空出特地用来记录第一个键值位置
//编号均从0开始

using namespace std;

enum nodetype{ Internal, Leaf };
extern BufferManager bf;
class index{
public:
	int Number;
	int keylength[3];
	
private:
	int maxchild;
	int order;
	int type;//0 int; 1 double; 2 string
	string name;
public:
	index(string filename);
	~index(){};
	void initialize(Data* key, int Addr, int ktype);
	int find(Data* key);
	void insert(Data* key, int Addr);
	int* split(char* currentBlock, Data* mid, Data* key, int Addr, int leftpos, int rightpos);
	void Internal_insert(char* currentBlock, Data* mid, int leftpos, int rightpos);
	void SplitLeaf(char* block1, char*block2, char* currentBlock, Data* key, int Addr);
	void SplitInternal(char* block1, char*block2, char* currentBlock, Data* mid, int leftpos, int rightpos);
	void Delete(Data* key);
	int* Range(Data*key1, Data*key2);
};

#endif