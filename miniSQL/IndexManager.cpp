#include "IndexManager.h"
#include "RecordManager.h"

IndexManager::IndexManager(
	const Table& t,
	int attr,
	int new_index
):index_head(t.index.IndexName[attr],new_index) {
	DataInt tmp(0);
	int key_size = tmp.getSize(t.attr.flag[attr]);
	max_degree = 
		(PAGE_SIZE - 2 * sizeof(int)-sizeof(Pos))
		/ (sizeof(Pos) + key_size) 
		+ 1;
}

void IndexManager::reset(const Table& t, int attr, int new_index) {
	index_head.reset(t.index.IndexName[attr], new_index);
	DataInt tmp(0);
	int key_size = tmp.getSize(t.attr.flag[attr]);
	max_degree =
		(PAGE_SIZE - 2 * sizeof(int) - sizeof(Pos))
		/ (sizeof(Pos) + key_size)
		+ 1;
}


void IndexManager::select(const Data& key, Table& t) {
	Pos pos, rec_pos;
	Tuper* tp;
	RecordManager rm(NULL);
	int root_pos = index_head.getRootPos();
	if (root_pos == index_head.NULL_POS) return;
	BptNode& root = *new BptNode(index_head.getFilename(), root_pos, max_degree);
	pos = find(key, root);
	if (pos.pos[1] != pos.pos[0]) return;
	rec_pos = root.getPtr(pos.pos[1]);

	Page& p = *new Page;
	p.ofs = rec_pos.pos[0];
	p.pageType = RecordPage;
	p.tableName = t.getName();

	index_head.bm.readPage(p);
	tp = rm.Char_Tuper(t, (char*)(p.pageData + rec_pos.pos[1]));

	t.data.push_back(tp);

	delete& root;
	delete& p;
}

void IndexManager::range_select(
	const Data* key1, const Data* key2,
	bool bound1, bool bound2,
	Table& t) {
	int i;
	Pos pos, rec_pos;
	Tuper* tp;
	RecordManager rm(nullptr);
	int root_pos = index_head.getRootPos();
	if (root_pos == index_head.NULL_POS) return;

	BptNode& start = 
		*new BptNode(
			index_head.getFilename(), 
			root_pos,
			max_degree
		);

	Page& p = *new Page;
	Data* K;
	p.pageType = RecordPage;
	p.tableName = t.getName();
	pos.pos[1] = 0;
	pos.pos[0] = -1;
	if (key1 == nullptr) {
		K = key2->new_copy();
		ReachFirstLeaf(start);
	}
	else {
		K = key1->new_copy();
		pos = find(*key1, start);
	}
	if (pos.pos[1] == pos.pos[0] && bound1 == false) {
		pos.pos[1]++;
	}
	bool loop=true;
	do {
		start.load(index_head.bm);
		for (i = pos.pos[1]; i < start.getNum(); i++) {
			start.getKey(*K,i);
			if ((key2)&&(*key2<*K || *key2==*K && !bound2)){
				loop=false;
				break;
			}
			rec_pos = start.getPtr(i);
			p.ofs = rec_pos.pos[0];
			index_head.bm.readPage(p);
			tp = rm.Char_Tuper(t, (char*)(p.pageData + rec_pos.pos[1]));
			t.data.push_back(tp);
		}
		pos.pos[1]=0;
	} while (loop && start.being_next());
	delete K;
	delete &p;
	delete &start;
}

int IndexManager::KeyExist(const Data& key) {
	int r = index_head.getRootPos();
	if (r == index_head.NULL_POS) return 0;
	BptNode& p = *new BptNode(index_head.getFilename(), r, max_degree);
	Pos pos = find(key, p);
	delete& p;
	return pos.pos[0] == pos.pos[1];
}

Pos IndexManager::find(const Data& key, BptNode& root) {
	Pos pos;
	int i;
	while (1) {
		root.load(index_head.bm);
		pos = root.binary_search(key);
		if (root.IsLeaf())break;
		i = pos.pos[1];
		if (pos.pos[1] == pos.pos[0]) i++;
		root.BeChildOf(root, i);
	}
	pos = root.binary_search(key);
	return pos;
}

void IndexManager::ReachFirstLeaf(BptNode& root) {
	while (1) {
		root.load(index_head.bm);
		if (root.IsLeaf()) break;
		root.BeChildOf(root, 0);
	}
}

void IndexManager::show_leaves() {//int
	int rootpos = index_head.getRootPos();
	if (rootpos == index_head.NULL_POS) {
		cout << "empty" << endl;
	}
	BptNode p(index_head.getFilename(), rootpos, max_degree);

	int sum = 0;
	ReachFirstLeaf(p);
	DataInt key(0);
	do {
		p.load(index_head.bm);
		p.PrintNode(key);
		sum += p.getNum();
		cout << "==" << endl;
	} while (p.being_next());
	cout << "total keys: " << sum << endl;
}

void IndexManager::show_index_file() {
	cout << index_head.getFilename() << ".record:" << endl;
	cout << "number of block: " << index_head.getLastPos() << endl;
	BptNode t(index_head.getFilename(), 0, max_degree);
	DataInt key(0);
	int sum = 0;
	for (int i = 1; i <= index_head.getLastPos(); i++) {
		cout << "block " << i << ":"<<endl;
		try {
			t.setBlock(i);
			t.load(index_head.bm);
			t.PrintNode(key);
			sum += t.getNum();
		}
		catch (IndexException& e) {
			if (e.getErrorCode() != OUT_OF_RANGE) throw e;
		}
	}
	cout << endl;
	cout << "total " << sum << " keys" << endl;
}

Pos IndexManager::find(// judge if key exists and be unique
	const Data& key, 
	BptNode& root, 
	vector<BptNode*>& ancestor,
	vector<int>& his_pos,
	int flag
) {
	Pos pos;
	BptNode* node = &root;
	BptNode* next;
	int i;
	while (1) {
		node->load(index_head.bm);
		ancestor.push_back(node);
		pos = node->binary_search(key);

		if (node->IsLeaf()) break;
		i = pos.pos[1];
		if (pos.pos[1] == pos.pos[0]) i++;

		his_pos.push_back(i);

		next = new BptNode(*node, i);
		node = next;
	}
	if (flag == findUNIQUE && pos.pos[1] == pos.pos[0]) {
		throw IndexException(KEY_NOT_UNIQUE, "key not unique");
	}
	else if (flag == findEXIST && pos.pos[1] != pos.pos[0]) {
		throw IndexException(KEY_NOT_EXIST, "key not exist");
	}
	return pos;
}


void IndexManager::pure_insert(
	vector<BptNode*>&ancestor,
	vector<int>& his_pos,
	//BptNode& t,
	const Data& key,
	const Pos& pos,
	int index
) {
	BptNode& t = *(ancestor.back());
	ancestor.pop_back();
	if (t.getNum() < max_degree - 1) {
		t.leaf_insert(key, pos, index);
		t.save(index_head.bm);
		delete& t;
	}
	else {
		BptNode ano(index_head.getFilename(), index_head.getNewPage(), max_degree);
		Data* p_new_key = key.new_copy();
		t.leaf_split(ano, key, pos, index, *p_new_key);
		ano.save(index_head.bm);
		t.save(index_head.bm);

		//t.PrintNode(key);
		//ano.PrintNode(key);

		parent_insert(ancestor,his_pos, t, *p_new_key, ano);
		delete p_new_key;
		delete& t;
	}
}

void IndexManager::parent_insert(
	vector<BptNode*>& ancestor,
	vector<int>& his_pos,
	BptNode& child, 
	const Data& key,
	BptNode& new_child) {

	if (ancestor.empty()) {
		BptNode root(index_head.getFilename(), index_head.getNewPage(), max_degree);
		root.setState(0,1, root.NO_CHANGE);
		root.setKey(key, 0);
		root.setPos(child, 0);
		root.setPos(new_child, 1);
		root.save(index_head.bm);
		index_head.setRootPos(root.getPos());
	}
	else {
		BptNode& P = *(ancestor.back());
		ancestor.pop_back();
		int tmp_pos = his_pos.back();
		his_pos.pop_back();
		if (P.getNum() < max_degree - 1) {
			P.nonleaf_insert(key, new_child, tmp_pos);
			P.save(index_head.bm);
			delete& P;
		}
		else {
			BptNode ano(index_head.getFilename(), index_head.getNewPage(), max_degree);
			Data* pnew_key = key.new_copy();
			P.parent_split(
				ano,
				key,
				tmp_pos,
				new_child,
				*pnew_key
			);
			ano.save(index_head.bm);
			P.save(index_head.bm);
			parent_insert(ancestor,his_pos, P, *pnew_key, ano);
			delete pnew_key;
			delete& P;
		}
	}
}

void IndexManager::insert(
	const Data& key, 
	int block_index, 
	int offset
) {
	BptNode* proot,*p;
	if (index_head.getRootPos() == index_head.NULL_POS) {
		proot = new BptNode(index_head.getFilename(), index_head.getNewPage(), max_degree);
		proot->setState(true, 0);
		proot->save(index_head.bm);
		index_head.setRootPos(proot->getPos());
	}
	else proot = new BptNode(index_head.getFilename(), index_head.getRootPos(), max_degree);

	vector<BptNode*> ans;
	vector<int> his_pos;
	Pos pos = find(key, *proot, ans,his_pos,findUNIQUE);
	Pos insert_pos;
	insert_pos.pos[0] = block_index;
	insert_pos.pos[1] = offset;
	pure_insert(ans, his_pos, key, insert_pos, pos.pos[1]);
	while (!ans.empty()) {
		p = ans.back();
		ans.pop_back();
		delete p;
	}
}

void IndexManager::remove_entry(
	vector<BptNode*>& ancestor,
	vector<int>& his_pos,
	const Data& key
) {
	BptNode& t = *ancestor.back();
	int delete_pos = his_pos.back();
	his_pos.pop_back();
	ancestor.pop_back();
	t.remove_in_node(key,delete_pos);
	if (ancestor.empty() && t.getNum() == 0) {
		int lastroot = t.getPos();
		if (t.IsLeaf()) {
			index_head.setRootPos(index_head.NULL_POS);
		}
		else {
			t.BeChildOf(t, 0);
			index_head.setRootPos(t.getPos());
		}
		index_head.freePage(lastroot);
		delete& t;
		return;
	}
	if (!ancestor.empty() && t.tooless()) {
		int case1, case2;
		delete_pos = his_pos.back();
		Data&delete_key = *key.new_copy();
		Data&new_delete_key = *key.new_copy();
		BptNode& P = *ancestor.back();
		BptNode* left=nullptr;
		BptNode* right=nullptr;

		if (delete_pos == 0) case1 = 0;
		else {
			left = new BptNode(P, delete_pos - 1);
			left->load(index_head.bm);
			if (t.MergeSize(*left) >= (max_degree))
				case1 = 2;
			else case1 = 1;
		}

		if (delete_pos == P.getNum()) case2 = 0;
		else {
			right= new BptNode(P, delete_pos + 1);
			right->load(index_head.bm);
			if (t.MergeSize(*right) >= (max_degree))
				case2 = 2;
			else case2 = 1;
		}
		int flag = case2 + (case1 << 4);
		switch (flag) {
		case 0x01:
			if (right) {
				P.getKey(delete_key, delete_pos);
				his_pos.pop_back();
				his_pos.push_back(delete_pos + 1);
				t.coalesce(delete_key, *right);
				t.save(index_head.bm);
				remove_entry(ancestor, his_pos, key);
				index_head.freePage(right->getPos());
				break;
			}
			else throw IndexException(DEFAULT, "pointer null");
		case 0x10:
		case 0x11:
			P.getKey(delete_key, delete_pos-1);
			left->coalesce(delete_key, t);
			left->save(index_head.bm);
			remove_entry(ancestor, his_pos, key);
			index_head.freePage(t.getPos());

			break;
		case 0x02:
		case 0x12:
			if (right) {
				P.getKey(delete_key, delete_pos);
				t.distribute_from(*right, delete_key, new_delete_key);
				P.setKey(new_delete_key, delete_pos);
				P.save(index_head.bm);
				t.save(index_head.bm);
				right->save(index_head.bm);
				break;
			}
			else throw IndexException(DEFAULT, "pointer null");

		case 0x20:
		case 0x21:
		case 0x22:
			P.getKey(delete_key, delete_pos-1);
			left->distribute_to(t, delete_key, new_delete_key);
			P.setKey(new_delete_key, delete_pos-1);
			P.save(index_head.bm);
			t.save(index_head.bm);
			left->save(index_head.bm);
			break;
		default:
			throw IndexException(DEFAULT, "delete entry error");
		}
		delete left;
		delete right;
		delete& new_delete_key;
		delete& delete_key;
	}
	else t.save(index_head.bm);
	delete& t;
}

void IndexManager::remove(const Data& key) {
	BptNode* proot, * p;
	if (index_head.getRootPos() == index_head.NULL_POS) {
		throw IndexException(DEFAULT, "index empty");
	}
	else
		proot = new BptNode(
			index_head.getFilename(),
			index_head.getRootPos(),
			max_degree
		);

	vector<BptNode*> ans;
	vector<int> his_pos;
	Pos pos = find(key, *proot, ans, his_pos, findEXIST);
	his_pos.push_back(pos.pos[1]);
	//if (pos.pos[0] != pos.pos[1])
	//	throw IndexException(DELETE_FAILED, "key not found");
	remove_entry(ans, his_pos, key);
	while (!ans.empty()) {
		p = ans.back();
		ans.pop_back();
		delete p;
	}
}

void IndexManager::remove_entry(
	vector<BptNode*>& ancestor,
	vector<int>& his_pos,
	const Data& key
) {
	BptNode& t = *ancestor.back();
	int delete_pos = his_pos.back();
	t.remove_in_node(key,delete_pos);
	if (ancestor.empty() && t.getNum() == 0) {
		int lastroot = t.getPos();
		if (t.IsLeaf()) {
			index_head.setRootPos(index_head.NULL_POS);
		}
		else {
			t.BeChildOf(t, 0);
			index_head.setRootPos(t.getPos());
		}
		index_head.freePage(lastroot);
		return;
	}
	if (!ancestor.empty() && t.tooless()) {
		int case1, case2;
		delete_pos = his_pos.back();
		Data&delete_key = *key.new_copy();
		Data&new_delete_key = *key.new_copy();
		BptNode& P = *ancestor.back();
		BptNode* left=nullptr;
		BptNode* right=nullptr;

		if (delete_pos == 0) case1 = 0;
		else {
			left = new BptNode(P, delete_pos - 1);
			left->load(index_head.bm);
			if (t.MergeSize(*left) >= (max_degree))
				case1 = 2;
			else case1 = 1;
		}

		if (delete_pos == P.getNum()) case2 = 0;
		else {
			right= new BptNode(P, delete_pos + 1);
			right->load(index_head.bm);
			if (t.MergeSize(*right) >= (max_degree))
				case2 = 2;
			else case2 = 1;
		}
		int flag = case2 + (case1 << 4);
		switch (flag) {
		case 0x01:
			if (right) {
				t.coalesce(delete_key, *right);
				remove_entry(ancestor, his_pos, key);
				break;
			}
			else throw IndexException(DEFAULT, "pointer null");
		case 0x10:
		case 0x11:
			left->coalesce(delete_key, t);
			remove_entry(ancestor, his_pos, key);
			break;
		case 0x02:
		case 0x12:
			if (right) {
				t.distribute_from(*right, delete_key, new_delete_key);
				P.setKey(new_delete_key, delete_pos);
				break;
			}
			else throw IndexException(DEFAULT, "pointer null");
		case 0x20:
		case 0x21:
		case 0x22:
			P.getKey(delete_key, delete_pos-1);
			left->distribute_to(t, delete_key, new_delete_key);
			P.setKey(new_delete_key, delete_pos-1);
			break;
		default:
			throw IndexException(DEFAULT, "delete entry error");
		}
	}
}