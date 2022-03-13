#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_
#include <vector>
#include <iostream>
#include "base.h"

class CatalogManager
{
public:
	CatalogManager() {};
	~CatalogManager() {};
	bool hasTable(std::string Tname);//if there is  a table named Tname return true, else return false
	void createTable(std::string Tname, Attribute attr, short primary, Index index, int blockNumber);//create a table
	//下面的接口可以被别的模块调用
	//得到表的属性信息，将表的属性信息存在传入的第二个参数里。
	void getAttribute(std::string Tname, Attribute& attr);//return the table's attributes
	//得到表的属性信息和索引信息,将表的信息存在传入的第二个参数里（注意仅包括表的模式信息）
	void getTable(std::string Tname, Table& T);//return the table named Tname
	//得到属性在表中的序号，如果不存在返回-1.
	int getAttrIndex(std::string AttrName, Attribute attr);//get the attribute index
	void UpdateTable(std::string Tname, Table inTable);
	void UpdateTableBlockNum(Table& T);
};



#endif
