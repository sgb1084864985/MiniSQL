#pragma once
//#include "IndexManager.h"
#include "BufferManager.h"
#include "IndexException.h"
#include "base.h"
#include <iostream>
//#include "IndexFileHead.h"
// Buffer : static variables, construct function without initialization
// Data   : some new virtual functions
// table  : kind of index;degree of index
struct Pos {
	int pos[2];
};
class BptNode{
// num parent leaf m*key (m+1)*p
private:
	static const int POS_NUM = 0;
	static const int POS_LEAF = sizeof(int);
	static const int POS_OFFSET = 2 * sizeof(int);
	#define  KEY_OFFSET (POS_OFFSET + degree * sizeof(Pos))
	static const int NOT_FOUND=-1;

#define POS_PTR(index) (p.pageData + POS_OFFSET + (index) * sizeof(Pos))
#define KEY_PTR(key,index) (p.pageData + KEY_OFFSET + (index)*(key).size())
	int leaf;
	int num;
	//int parent;
	int degree;

public:
	static const int NO_CHANGE = -2;
	//BptNode();
	//BptNode(const BptNode& t);// be parent of t
	void PrintNode(const Data& key_like);
	void setBlock(int pos) { p.ofs = pos; }
	Page p;

	BptNode(const string& filename, int blockoft, int degree) :p(), degree(degree) {
		p.tableName = filename;
		p.pageType = IndexPage;
		p.ofs = blockoft;
	}
	void BeChildOf(const BptNode& t, int i) {
		Pos oft;

		memcpy(&oft, t.p.pageData + POS_OFFSET + i * sizeof(Pos), sizeof(Pos));
		p.tableName = t.p.tableName;
		p.pageType = IndexPage;
		p.ofs = oft.pos[0];
	}
	int being_next();
	int getPos()const { return p.ofs; }

	int IsLeaf()const { return leaf; }

	void setOffset(int offset) {
		p.ofs = offset;
	}

	int getNum()const {
		return num;
	}

	int MergeSize(const BptNode& other)const {
		if (leaf && other.leaf) {
			return num + other.num;
		}
		else if (!leaf && !other.leaf) {
			return num + other.num + 1;
		}
		else throw IndexException(DEFAULT, "match error");
	}

	int tooless()const {
		if (leaf) return num < degree / 2;
		else return num < (degree-1) / 2;
	}

	void getKey(Data& key, int index) const{
		key.MemtoData(KEY_PTR(key, index));
	}

	void setKey(const Data& key, int index) {
		key.DatatoMem(KEY_PTR(key, index));
	}

	void setPos(const Pos& pos, int index) {
		memcpy(POS_PTR(index), &pos, sizeof(Pos));
	}

	Pos getPtr(int index) {
		Pos pos;
		memcpy( &pos, POS_PTR(index), sizeof(Pos));
		return pos;
	}

	void setPos(const BptNode& t, int index) {
		setPos(t.p.ofs, index);
	}

	void setPos(int pos, int index) {
		Pos tmp;
		tmp.pos[0] = pos;
		setPos(tmp, index);
	}

	void setState(int leaf, int num, int degree = NO_CHANGE) {
		if (leaf != NO_CHANGE) this->leaf = leaf;
		if (num != NO_CHANGE) this->num = num;
		if (degree != NO_CHANGE) this->degree = degree;
	}

	BptNode(const BptNode& t, int i) :p(), degree(t.degree) { BeChildOf(t, i); }

	//void insert(const Data& key, const Pos& insert_pos, const IndexFileHead& h);
	void leaf_insert(const Data& key, const Pos& p,int index);
	void nonleaf_insert(const Data& key, const BptNode& t,int index);
	void leaf_split(BptNode& ano,const Data&key,const Pos& pos,int index_key,Data& ret_key);
	
	void parent_split(
		BptNode& ano,
		const Data& key,
		int index_key,
		BptNode& new_child,
		Data& ret_key
	);

	// delete only in leaf
	void remove_in_node(const Data& key,int index);
	void nonleaf_distribute_to(BptNode& other,const Data& midkey,Data& retkey);
	void nonleaf_distribute_from(BptNode& other, const Data& midkey, Data& retkey);
	void leaf_distribute_to(BptNode& other, Data& retkey);
	void leaf_distribute_from(BptNode& other, Data& retkey);

	void distribute_to(BptNode& other, const Data& midkey, Data& retkey) {
		if (leaf) leaf_distribute_to(other, retkey);
		else nonleaf_distribute_to(other, midkey, retkey);
	}

	void distribute_from(BptNode& other, const Data& midkey, Data& retkey) {
		if (leaf) leaf_distribute_from(other, retkey);
		else nonleaf_distribute_from(other, midkey, retkey);
	}
	//void coalesce(BptNode& other);

	void coalesce(const Data&mid_key,BptNode& other);
	void load(BufferManager&bm);
	void save(BufferManager&bm);

	Pos binary_search(const Data& key,int right=false);
};
