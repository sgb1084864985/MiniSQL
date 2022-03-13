#ifndef _API_H_
#define _API_H_
#include "base.h"
#include "CatalogManager.h"
#include "Interpreter.h"
#include "BufferManager.h"
#include "IndexManager.h"
#include "RecordManager.h"
extern BufferManager bf;

class API
{
public:
	API(BufferManager& buffer):BM(&buffer) {};
	void INSERT(std::string TName, Tuper& inTuper);
	Table SELECT(Table& inTable, std::vector<Where> w, std::vector<int> selectAttr);
	void CREATETABLE(Table& inTable);
	void CREATEINDEX(Table& inTable, int Attr_index);
	//hasWhere is true if the delete statement has where condition, false if no where condition
	int DELETE(Table& inTable, std::vector<Where> w, bool hasWhere);
	bool DropTable(Table& inTable);
	void DropIndex(Table& inTable, int attr);

private:
	//RecordManager RM(&BM);//record manager
	BufferManager* BM;
	//bool Comp(Where& w1, Where& w2);
};


#endif