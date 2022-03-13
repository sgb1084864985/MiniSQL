#pragma once
//#include "IndexManager.h"
#include "BufferManager.h"
#include "IndexException.h"

// "Index" 0-79
// RootPos 80-160
// FreeSpacePos 160-240


class IndexFileHead{
private:
	Page p;
	string filename;
	int root_pos;
	int free_pos;
	int last_pos;
public:

	static const int NULL_POS = -1;
	static const int LABLE_POS = 0;
	static const int ROOT_POS = 80;
	static const int FREE_POS = 160;
	static const int LAST_POS = 240;

	BufferManager bm;

	IndexFileHead() :p(), bm(),root_pos(NULL_POS),free_pos(NULL_POS),last_pos(NULL_POS) {
		p.pageType = IndexPage;
		p.ofs = 0;
	}

	IndexFileHead(const string& filename, int New) : filename(filename), p(), bm() {
		p.tableName = filename;
		p.pageType = IndexPage;
		p.ofs = 0;

		if (New) {
			fstream file;
			file.open(filename, ios::out);
			file.close();
			formatting();
		}
		else reload();
	}

	void reset(const string& filename, int New) {
		this->filename = filename;
		p.tableName = filename;

		if (New) {
			fstream file;
			file.open(filename, ios::out);
			file.close();
			formatting();
		}
		else reload();
	}

	~IndexFileHead() {
		resave();
	}
	const string& getFilename() {
		return filename;
	}
	int getRootPos() const {
		return root_pos;
	}

	int getNextFreeSpacePos()const {
		return free_pos;
	}

	void setRootPos(int pos) {
		root_pos = pos;
	}

	void setNextFreeSpacePos(int pos) {
		free_pos = pos;
	}

	int getLastPos()const {
		return last_pos;
	}

	void setLastPos(int pos) {
		last_pos = pos;
	}

	void formatting();
	void reload();
	void resave();

	//void addFreePage(int pos);
	//void addNewPage();
	int getNewPage();
	void freePage(int pos);

};

