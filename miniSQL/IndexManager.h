#pragma once
#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include"base.h"
#include"BptNode.h"
#include"IndexFileHead.h"
#include"IndexException.h"

using namespace std;

//typedef int Pos[2];

class IndexManager{
public:
	//for API
	IndexManager(
		const Table& t, 
		int attr, 
		int new_index = false
	);

	void select(const Data& key, Table& t);
	void range_select(
		const Data* key1, const Data* key2,
		bool bound1, bool bound2,
		Table& t);

	void insert(const Data& key, int block_index, int offset);
	void remove(const Data& key);

	// for function FindWithIndex
	IndexManager() :index_head(),max_degree(0){

	}
	int KeyExist(const Data& key);
	void reset(const Table& t, int attr, int new_index = false);


	// for debug
	void show_index_file();
	void show_leaves();


	// temp
	IndexManager(
		const string& filename,
		int key_size, int fnew = false
	) :index_head(filename, fnew) {
		//max_degree = (PAGE_SIZE - 2 * sizeof(int)-sizeof(Pos)) / (sizeof(Pos) + key_size) + 1;
		max_degree = 4;
	}
private:
	//int clustered;
	static const int findUNIQUE = 1;
	static const int findEXIST = 2;
	static const int findDEFAULT = 0;

	int max_degree;
	IndexFileHead index_head;


	void ReachFirstLeaf(BptNode& root);
	void parent_insert(
		vector<BptNode*>& ancestor,
		vector<int>& his_pos,
		BptNode& child,
		const Data& key,
		BptNode& new_child
	);

	void remove_entry(
		vector<BptNode*>& ancestor,
		vector<int>& his_pos,
		const Data& key
	);

	void pure_insert(
		vector<BptNode*>& ancestor,
		vector<int>& his_pos,
		//BptNode& t,
		const Data& key,
		const Pos& pos,
		int index
	);

	Pos find(
		const Data& key,
		BptNode& root,
		vector<BptNode*>& ancestor,
		vector<int>& his_pos,
		int flag = findDEFAULT
	);

	//side effect: root -> leaf
	Pos find(const Data& key, BptNode& root);

};

