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
	int BlockNum;//���ڴ��еĵڼ�������
	int Position; //�������е�λ��
};

//һ��page�����ʾ�ڴ滺������һ���飬���ض��ı�������ݴ洢
class Page {
public:
	string tableName;//����������
	PageType pageType;//���������
	int pageIndex;//�����ڻ������е�����
	BlockOffset ofs;
	char pageData[PAGE_SIZE];//���崢����Ϣ
	bool isValid;//������Ϊlazy_deletion�ı��

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

	// �Ƚ�����Page�����������
	bool operator==(const Page& page) {
		return (tableName == page.tableName) && (pageType == page.pageType) && (ofs == page.ofs);
	}

	string getvalues(int startpos, int endpos) 
	{
		string tmpt = "";
		if (startpos >= 0 && startpos <= endpos && endpos <= PAGE_SIZE)
			for (int i = startpos; i <= endpos; i++)//6.25�޸�bug
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
