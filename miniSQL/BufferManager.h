#ifndef _MINISQL_BUFFERMANAGER_H_
#define _MINISQL_BUFFERMANAGER_H_
#define _CRT_SECURE_NO_DEPRECATE
#include "Page.h"
#include "base.h"
using namespace std;

class BufferManager {
public:
	int LRUCounter[CACHE_CAPACITY];//与缓冲区大小相同的计数器来记录缓冲块被使用的次数
	Page cachePages[CACHE_CAPACITY];//缓存使用了一个Page数组进行内存缓存,记录所有的block
	bool pined[CACHE_CAPACITY];//使用了一个bool数组来记录pin过的缓冲区
	bool isDirty[CACHE_CAPACITY];//使用了一个与缓冲区大小相同的bool数组记录缓冲区是否被写过

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

	//下面六个函数为接口给其他模块使用
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
	//实现LRU算法
	void LRUCounterAddExceptCurrent(int);
	int getUnpinedBiggestPageFromCache();

};

#endif

