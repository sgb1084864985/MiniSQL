#ifndef _MINISQL_PAGE_H_
#define _MINISQL_PAGE_H_
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <assert.h>
#include <map>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// PageSize is an integral multiple of KB, define as 4KB.
#define PAGE_SIZE 4096
// cache capacity, 10 page as defined
#define CACHE_CAPACITY 10
//typedef int Pointer;
//typedef int int;
//typedef int FileHandle;
typedef int BlockOffset;
//enum Comparison { Lt, Le, Gt, Ge, Eq, Ne };
enum PageType 
{
	Undefined,
	RecordPage,
	IndexPage,
};

class InsertPos {
public:
	int BlockNum;//在内存中的第几个区块
	int Position; //在区块中的位置
};

//一个page对象表示内存缓冲区的一个块，对特定的表进行数据存储
class Page {
public:
	string tableName;//处理表的名字
	PageType pageType;//区块的类型
	int pageIndex;//区块在缓冲区中的索引
	BlockOffset ofs;
	char pageData[PAGE_SIZE];//具体储存信息
	bool isValid;//用于作为lazy_deletion的标记

	Page()
	{
		tableName = "";
		pageType = Undefined;
		pageIndex = -1;
		ofs = 0;
		memset(pageData, '#', PAGE_SIZE);
		isValid = 0;
	}

	~Page()
	{

	}

	// 比较两个Page，运算符重载
	bool operator==(const Page& page) {
		return (tableName == page.tableName) && (pageType == page.pageType) && (ofs == page.ofs);
	}

	string getvalues(int startpos, int endpos) 
	{
		string tmpt = "";
		if (startpos >= 0 && startpos <= endpos && endpos <= PAGE_SIZE)
			for (int i = startpos; i <= endpos; i++)//6.25修复bug
				tmpt += pageData[i];
		return tmpt;
	}
	char getvalues(int pos) 
	{
		if (pos >= 0 && pos <= PAGE_SIZE)
			return pageData[pos];
		return '\0';
	}
};

#endif
