#ifndef _MINISQL_BUFFERMANAGER_H_
#define _MINISQL_BUFFERMANAGER_H_
#define _CRT_SECURE_NO_DEPRECATE
#include "Page.h"
using namespace std;

class BufferManager {
private://static
	static int LRUCounter[CACHE_CAPACITY];//�뻺������С��ͬ�ļ���������¼����鱻ʹ�õĴ���
	static Page cachePages[CACHE_CAPACITY];//����ʹ����һ��Page��������ڴ滺��,��¼���е�block
	static bool pined[CACHE_CAPACITY];//ʹ����һ��bool��������¼pin���Ļ�����
	static bool isDirty[CACHE_CAPACITY];//ʹ����һ���뻺������С��ͬ��bool�����¼�������Ƿ�д��
	//static int a;
public:
	BufferManager() {
		//memset(LRUCounter, 0, CACHE_CAPACITY);
		//memset(pined, false, CACHE_CAPACITY);
		//memset(isDirty, false, CACHE_CAPACITY);
	}

	~BufferManager() {
		//for (int i = 0; i < CACHE_CAPACITY; i++)
		//	ForcedWritePageToFile(cachePages[i]);
	}

	//������������Ϊ�ӿڸ�����ģ��ʹ��
	bool readPage(Page&);
	bool writePage(Page&);
	Page& recordManagerGetBlankPage();
	bool pinPage(Page&);
	bool unpinPage(Page&);
	int findPageInCache(Page&);

private:
	bool ForcedReadPageFromFile(Page&);
	bool ForcedWritePageToFile(Page&);
	//ʵ��LRU�㷨
	void LRUCounterAddExceptCurrent(int);
	int getUnpinedBiggestPageFromCache();

};

#endif

