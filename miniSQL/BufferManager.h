#ifndef _MINISQL_BUFFERMANAGER_H_
#define _MINISQL_BUFFERMANAGER_H_
#define _CRT_SECURE_NO_DEPRECATE
#include "Page.h"
#include "base.h"
using namespace std;

class BufferManager {
public:
	int LRUCounter[CACHE_CAPACITY];//�뻺������С��ͬ�ļ���������¼����鱻ʹ�õĴ���
	Page cachePages[CACHE_CAPACITY];//����ʹ����һ��Page��������ڴ滺��,��¼���е�block
	bool pined[CACHE_CAPACITY];//ʹ����һ��bool��������¼pin���Ļ�����
	bool isDirty[CACHE_CAPACITY];//ʹ����һ���뻺������С��ͬ��bool�����¼�������Ƿ�д��

public:
	BufferManager() 
	{
		memset(LRUCounter, 0, sizeof(int)*CACHE_CAPACITY);
		memset(pined, false, sizeof(bool)*CACHE_CAPACITY);
		memset(isDirty, false, sizeof(bool)*CACHE_CAPACITY);
	}

	~BufferManager() 
	{
		for (int i = 0; i < CACHE_CAPACITY; i++)
			if(isDirty[i]) ForcedWritePageToFile(cachePages[i]);
	}

	//������������Ϊ�ӿڸ�����ģ��ʹ��
	bool readPage(Page&);
	bool writePage(Page&);
	Page& recordManagerGetBlankPage();
	bool pinPage(Page&);
	bool unpinPage(Page&);
	int findPageInCache(Page&);
	void setInvalid(string tempname);
	InsertPos getInsertPosition(Table& temp);
	int AddBlockInFile(Table& temp);

private:
	bool ForcedReadPageFromFile(Page&);
	bool ForcedWritePageToFile(Page&);
	//ʵ��LRU�㷨
	void LRUCounterAddExceptCurrent(int);
	int getUnpinedBiggestPageFromCache();

};

#endif

