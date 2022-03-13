#ifndef _RECORDMANAGER_H
#define	_RECORDMANAGER_H
#include "base.h"
#include "BufferManager.h"
#include "Page.h"
class RecordManager
{
public:
	RecordManager(BufferManager* bf) :buf_ptr(bf) {}
	~RecordManager();

	bool isSatisfied(Table& tableinfor, Tuper& row, vector<int> mask, vector<Where> w);
	Table Select(Table& tableIn, vector<int>attrSelect, vector<int>mask, vector<Where>& w);
	Table Select(Table& tableIn, vector<int>attrSelect);

	bool CreateIndex(Table& tableIn, int attr);
	int FindWithIndex(Table& tableIn, Tuper& row, int mask);
	InsertPos Insert(Table& tableIn, Tuper& singleTuper);
	Tuper* Char_Tuper(Table& tableIn, char* stringRow);
	void InsertWithIndex(Table& tableIn, Tuper& singleTuper);

	char* Tuper_Char(Table& tableIn, Tuper& singleTuper);
	int Delete(Table& tableIn, vector<int>mask, vector<Where> w);
	bool DropTable(Table& tableIn);
	bool CreateTable(Table& tableIn);

	Table SelectProject(Table& tableIn, vector<int>attrSelect);
	Tuper String_Tuper(Table& tableIn, string stringRow);
	bool UNIQUE(Table& tableinfo, Where w, int loca);

	//add
	InsertPos getInsertPos(Table& tableIn, Tuper& temp);
	bool isEqual(Table& tableIn, string stringRow, int& c_pos, Tuper& temp);
private:
	RecordManager() {} //产生rm的时候必须把bufferManager的指针赋值给它
	BufferManager* buf_ptr;
};

#endif


