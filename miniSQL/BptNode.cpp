#include "BptNode.h"

void BptNode::leaf_insert(const Data& key, const Pos& pos, int index) {
	if (num >= degree - 1) 
		throw IndexException(INSERT_ERROR, "no enough space for leaf node");
	if (p.pageType !=IndexPage) 
		throw IndexException(BLOCK_ALLOC_ERROR, "page not loaded yet");
	if (num) {
		if(num>index)
			memmove(
				KEY_PTR(key, index + 1),
				KEY_PTR(key, index), 
				key.size() * (num - index)
			);

		memmove(
			POS_PTR(index + 1), 
			POS_PTR(index), 
			sizeof(Pos) * (num + 1 - index)
		);
	}
	else {
		Pos tpos;
		tpos.pos[0] = NOT_FOUND;
		tpos.pos[1] = NOT_FOUND;
		memcpy(POS_PTR(index + 1), &tpos, sizeof(Pos));
	}

	key.DatatoMem(KEY_PTR(key, index));
	memcpy(POS_PTR(index), &pos, sizeof(Pos));
	num++;
}

void BptNode::remove_in_node(const Data& key, int index) {
	if (index < 0)
		throw IndexException(DELETE_FAILED, "neglect index");
	if (!leaf) index--;
	if (num < index+1)
		throw IndexException(DELETE_FAILED, "too less keys to delete");
	if (leaf!=1&&leaf!=0)
		throw IndexException(DELETE_FAILED, "node not legal");
	if (num > index + 1) {
		memmove(
			KEY_PTR(key, index),
			KEY_PTR(key, index + 1),
			key.size() * (num - index - 1)
		);
		memmove(
			POS_PTR(index+!leaf),
			POS_PTR(index+1+!leaf),
			sizeof(Pos) * (num - index -1)
		);
	}
	if (leaf)
		memmove(POS_PTR(num-1), POS_PTR(num), sizeof(Pos));
	num--;
}

void BptNode::coalesce(const Data& mid_key, BptNode& other) {
	int end_pos = num;
	if(!leaf) mid_key.DatatoMem(KEY_PTR(mid_key, end_pos++));
	memcpy(
		KEY_PTR(mid_key,end_pos),
		other.p.pageData + KEY_OFFSET,
		other.num * mid_key.size()
	);
	memcpy(
		POS_PTR(num + !leaf),
		other.p.pageData + POS_OFFSET,
		(other.num+1)*sizeof(Pos)
	);
	num += other.num + !leaf;
}

void BptNode::leaf_split(BptNode& ano, const Data& key,const Pos&pos,int index_key,Data& ret_key) {
	char* temp_key = new char[(num+1) * key.size()];
	char* temp_pos = new char[(num+2) * sizeof(Pos)];

	Pos tmp;
	tmp.pos[0] = ano.p.ofs;
	/* keys to temp memory*/
	memcpy(temp_key, KEY_PTR(key, 0), key.size()*index_key);
	key.DatatoMem(temp_key + key.size() * index_key);

	if(num>index_key)
		memcpy(
			temp_key+key.size()*(index_key+1), 
			KEY_PTR(key, index_key), 
			key.size() * (num - index_key)
		);

	/* keys moving back*/

	memcpy(KEY_PTR(key, 0), temp_key, key.size() * (num / 2+1));
	memcpy(ano.p.pageData + KEY_OFFSET,
		temp_key + (num / 2+1) * key.size(),
		key.size() * (num  - num / 2));

	/* positions to temp memory*/

	memcpy(temp_pos, POS_PTR(0), sizeof(Pos) * index_key);
	memcpy(temp_pos + sizeof(Pos) * (index_key),
		&pos, sizeof(Pos));
	if(num>index_key)
		memcpy(temp_pos + sizeof(Pos) * (index_key + 1), 
			POS_PTR(index_key), sizeof(Pos) * (num - index_key));

	/* positions moving back*/

	memcpy(POS_PTR(0), temp_pos, sizeof(Pos) * (num / 2+1));
	memcpy(ano.p.pageData + POS_OFFSET, temp_pos + (num / 2+1) * sizeof(Pos), sizeof(Pos) * (num - num / 2));

	/* assign Pn*/

	memcpy(ano.p.pageData + POS_OFFSET + sizeof(Pos) * (num - num / 2), 
		POS_PTR(num), sizeof(Pos));
	memcpy(POS_PTR(num / 2 + 1), &tmp, sizeof(Pos));

	ano.num = num - num / 2;
	ano.leaf = true;
	num = num / 2 + 1;

	ret_key.MemtoData(ano.p.pageData + KEY_OFFSET);
	delete[] temp_key;
	delete[] temp_pos;
}

void BptNode::nonleaf_insert(
	const Data& key, 
	const BptNode& t, 
	int index
){
	if (num >= degree - 1) throw IndexException(INSERT_ERROR, "no enough space for leaf node");
	if (p.pageType != IndexPage) throw(BLOCK_ALLOC_ERROR, "Page not loaded yet");

	Pos pos;
	pos.pos[0] = t.p.ofs;
	if (num > index) {
		memmove(
			KEY_PTR(key, index + 1),
			KEY_PTR(key, index),
			key.size() * (num - index)
		);

		memmove(
			POS_PTR(index + 2),
			POS_PTR(index + 1),
			sizeof(Pos) * (num - index)
		);
	}

	key.DatatoMem(KEY_PTR(key, index));
	memcpy(POS_PTR(index+1), &pos, sizeof(Pos));

	num++;
}

void BptNode::parent_split(
	BptNode& ano,
	const Data& key,
	int index_key,
	BptNode& new_child,
	Data& ret_key
){
	char* temp_key = new char[(num + 1) * key.size()];
	char* temp_pos = new char[(num + 2) * sizeof(Pos)];

	//Pos pos = binary_search(key);
	//if (pos.pos[0] == pos.pos[1])pos.pos[1]++;
	Pos insert_pos;
	insert_pos.pos[0] = new_child.p.ofs;
	//int index_key = pos.pos[1];

	/* copy keys to temp memory */

	memcpy(temp_key, KEY_PTR(key, 0), key.size() * index_key);
	key.DatatoMem(temp_key + key.size() * index_key);
	if(num>index_key)
		memcpy(temp_key + key.size() * (index_key + 1),
			KEY_PTR(key, index_key),
			key.size() * (num - index_key));

	/* copy keys to self and new node `ano` */
	memcpy(
		KEY_PTR(key, 0),
		temp_key,
		key.size() * (num / 2+1)
	);
	
	memcpy(
		ano.p.pageData + KEY_OFFSET,
		temp_key + (num / 2 + 2) * key.size(),
		key.size() * (num - 1 - num / 2)
	);

	/* copy positions to temp memory */

	memcpy(temp_pos, POS_PTR(0), sizeof(Pos) * (index_key+1));
	memcpy(temp_pos + sizeof(Pos) * (index_key+1),
		&insert_pos, sizeof(Pos));
	if(num>index_key)
		memcpy(temp_pos + sizeof(Pos) * (index_key + 2),
			POS_PTR(index_key+1), sizeof(Pos) * (num - index_key));

	/* copy positions to self and new node `ano` */

	memcpy(POS_PTR(0), temp_pos, sizeof(Pos) * (num / 2 + 2));
	memcpy(ano.p.pageData + POS_OFFSET, 
		temp_pos + (num / 2+2) * sizeof(Pos), 
		sizeof(Pos) * (num - num / 2));


	ret_key.MemtoData(temp_key + (num / 2 + 1) * key.size());

	ano.leaf = false;
	ano.num = num - 1 - num / 2;
	num = num / 2 + 1;

	delete[] temp_key;
	delete[] temp_pos;
}


void BptNode::load(BufferManager&bm) {
	bm.readPage(p);
	char* buf = p.pageData;
	memcpy(&num, buf+POS_NUM, sizeof(num));
	if (num < 0||num>degree) throw IndexException(OUT_OF_RANGE, "num < 0!");
	memcpy(&leaf, buf + POS_LEAF, sizeof(leaf));
	if (leaf!=0&&leaf!=1) throw IndexException(OUT_OF_RANGE, "leaf not bool!");
}

int BptNode::being_next() {
	if (leaf!=true) throw IndexException(DEFAULT, "not a leaf node");
	Pos pos;
	memcpy(&pos, POS_PTR(num), sizeof(Pos));
	if (pos.pos[0] <= 0) return false;
	p.ofs = pos.pos[0];
	return true;
}

void BptNode::save(BufferManager&bm) {
	char* buf = p.pageData;
	memcpy( buf + POS_NUM, &num, sizeof(num));
	memcpy( buf + POS_LEAF, &leaf, sizeof(leaf));
	bm.writePage(p);
}

void BptNode::nonleaf_distribute_to(
	BptNode& other,
	const Data& midkey,
	Data& retkey
) {
	memmove(
		other.p.pageData + KEY_OFFSET + midkey.size(),
		other.p.pageData + KEY_OFFSET,
		midkey.size() * other.num
	);

	memmove(
		other.p.pageData + POS_OFFSET + sizeof(Pos),
		other.p.pageData + POS_OFFSET,
		sizeof(Pos) * (other.num+1)
	);

	midkey.DatatoMem(other.p.pageData + KEY_OFFSET);
	memcpy(
		other.p.pageData + POS_OFFSET,
		POS_PTR(num),
		sizeof(Pos)
	);
	retkey.MemtoData(KEY_PTR(retkey, num - 1));
	num--;
	other.num++;
}

void BptNode::leaf_distribute_to(
	BptNode& other, 
	Data& retkey
) {
	memmove(
		other.p.pageData + KEY_OFFSET + retkey.size(),
		other.p.pageData + KEY_OFFSET,
		retkey.size() * other.num
	);

	memmove(
		other.p.pageData + POS_OFFSET + sizeof(Pos),
		other.p.pageData + POS_OFFSET,
		sizeof(Pos) * (other.num + 1)
	);

	memcpy(
		other.p.pageData + KEY_OFFSET,
		KEY_PTR(retkey, num - 1),
		retkey.size()
	);

	memcpy(
		other.p.pageData + POS_OFFSET,
		POS_PTR(num-1),
		sizeof(Pos)
	);

	memcpy(
		POS_PTR(num - 1),
		POS_PTR(num),
		sizeof(Pos)
	);

	retkey.MemtoData(KEY_PTR(retkey, num - 1));
	num--;
	other.num++;
}

void BptNode::leaf_distribute_from(
	BptNode& other,
	Data& retkey
) {
	memcpy(
		POS_PTR(num + 1),
		POS_PTR(num),
		sizeof(Pos)
	);
	memcpy(
		KEY_PTR(retkey, num),
		other.p.pageData + KEY_OFFSET,
		retkey.size()
	);

	memcpy(
		POS_PTR(num),
		other.p.pageData + POS_OFFSET,
		sizeof(Pos)
	);

	retkey.MemtoData(other.p.pageData + KEY_OFFSET);

	memmove(
		other.p.pageData + KEY_OFFSET,
		other.p.pageData + KEY_OFFSET + retkey.size(),
		retkey.size() * (other.num-1)
	);

	memmove(
		other.p.pageData + POS_OFFSET,
		other.p.pageData + POS_OFFSET + sizeof(Pos),
		sizeof(Pos) * (other.num)
	);
	num++;
	other.num--;
}

void BptNode::nonleaf_distribute_from(
	BptNode& other,
	const Data& midkey,
	Data& retkey
) {
	midkey.DatatoMem(KEY_PTR(midkey,num));
	memcpy(
		POS_PTR(num+1),
		other.p.pageData + POS_OFFSET,
		sizeof(Pos)
	);

	retkey.MemtoData(other.p.pageData + KEY_OFFSET);

	memmove(
		other.p.pageData + KEY_OFFSET,
		other.p.pageData + KEY_OFFSET + midkey.size(),
		midkey.size() * (other.num - 1)
	);

	memmove(
		other.p.pageData + POS_OFFSET,
		other.p.pageData + POS_OFFSET + sizeof(Pos),
		sizeof(Pos) * (other.num)
	);
	num++;
	other.num--;
}

Pos BptNode::binary_search(const Data& key,int right) {
	int pos_offset = 3 * sizeof(int);
	int key_offset = pos_offset + degree * key.size();
	int i = 0;
	int j = num-1;
	char* ptr = p.pageData + key_offset;
	int m;
	int flag = 0;
	Pos rslt;

	if (p.pageType != IndexPage) throw(BLOCK_ALLOC_ERROR, "Page not loaded yet");
	rslt.pos[1] = 0;
	while (i <= j) {
		m = (i + j) / 2;
		if (key == KEY_PTR(key,m)) {
			if (right)i = m + 1;
			else j = m - 1;
			flag = 1;
		}
		else if (key < KEY_PTR(key,m)) {
			j = m - 1;
		}
		else i = m + 1;
	}
	rslt.pos[1] = i;
	if (flag)rslt.pos[0] = i;
	else rslt.pos[0] = j;
	return rslt;
}

void BptNode::PrintNode(const Data& key_like) {
	cout << "oft:" << p.ofs << endl;
	cout << "isleaf: " << leaf << endl;
	cout << "num: " << num << endl;
	int i = 0;
	DataInt k(0);
	Pos tpos;
	for (i = 0; i < num; i++) {
		memcpy(&tpos, POS_PTR(i), sizeof(Pos));
		k.MemtoData(KEY_PTR(key_like, i));
		cout << "<" << tpos.pos[0] << "," << tpos.pos[1] << "> ";
		cout << "[" << k.x << "]" << endl;
	}
	memcpy(&tpos, POS_PTR(i), sizeof(Pos));
	cout << "<" << tpos.pos[0] << "," << tpos.pos[1] << "> "<<endl;
}