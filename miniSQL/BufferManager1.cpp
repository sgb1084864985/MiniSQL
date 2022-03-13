#include "BufferManager.h"

int BufferManager:: LRUCounter[CACHE_CAPACITY] = { 0 };//�뻺������С��ͬ�ļ���������¼����鱻ʹ�õĴ���
Page BufferManager:: cachePages[CACHE_CAPACITY] = { Page() };//����ʹ����һ��Page��������ڴ滺��,��¼���е�block
bool BufferManager:: pined[CACHE_CAPACITY] = { false };//ʹ����һ��bool��������¼pin���Ļ�����
bool BufferManager:: isDirty[CACHE_CAPACITY] = { false };//ʹ����һ���뻺������С��ͬ��bool�����¼�������Ƿ�д��

bool BufferManager::readPage(Page& page)
{
	//assert(page.pageIndex == PageType::Undefined);
	int pageIndex = findPageInCache(page);//�ҵ���ǰ������page
	if (pageIndex != -1) 
	{
		// ֱ�Ӵӿ��ٻ��崢����cache��
		page = cachePages[pageIndex];
		LRUCounterAddExceptCurrent(pageIndex);//����ÿһ��page��Ӧ�ļ�������ֵ
		return true;
	}
	//���û���ҵ���ǰ������page
	else {
		ForcedReadPageFromFile(page);//�Ӵ����ļ�����ǿ�ƶ���
		pageIndex = getUnpinedBiggestPageFromCache();//�ҵ�Ŀ���滻��
		if (pageIndex == -1) 
			return true;
		else 
		{
			if (isDirty[pageIndex]) 
			{
				isDirty[pageIndex] = false;
				// If it's dirty page, write it again to file
				ForcedWritePageToFile(cachePages[pageIndex]);//�������飬д���ļ�
			}
			// use the new page to replace the page
			cachePages[pageIndex] = page;//�滻
			LRUCounterAddExceptCurrent(pageIndex);
			//cout << (void*)page.pageData<<endl;
			//cout << (void*)cachePages[pageIndex].pageData<<endl;
			return true;
		}
	}
}

bool BufferManager::writePage(Page& page)
{
	assert(page.pageType != Undefined);//page������δ����ֱ���˳������ֹ������bug
	int pageIndex = findPageInCache(page);//�ҵ���ǰ������page
	if (pageIndex != -1) 
	{
		cachePages[pageIndex] = page;//ֱ��д��
		isDirty[pageIndex] = true;
		LRUCounterAddExceptCurrent(pageIndex);//����ÿһ��page��Ӧ�ļ�������ֵ
		return true;
	}
	else 
	{
		pageIndex = getUnpinedBiggestPageFromCache();//�ҵ�Ŀ���滻��	
		if (pageIndex == -1) 
		{
			ForcedWritePageToFile(page);// May cause bug
			return true;
		}
		else 
		{
			if (isDirty[pageIndex]) 
			{
				isDirty[pageIndex] = false;
				ForcedWritePageToFile(cachePages[pageIndex]);//�������飬д���ļ�
			}
			cachePages[pageIndex] = page;//�滻
			LRUCounterAddExceptCurrent(pageIndex);
			return true;
		}
	}
	return false;
}

bool BufferManager::ForcedReadPageFromFile(Page& page)
{
	string filename = (page.tableName + "." + "record");
	Page temp;
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		cout << "file open failed when writing" << endl;
		exit(0);
	}
	if (fp) 
	{
		fseek(fp, PAGE_SIZE * page.ofs, SEEK_SET);

		fread(temp.pageData, PAGE_SIZE, SEEK_CUR, fp);
		fclose(fp);

		/*temp.ofs = page.ofs;
		temp.pageIndex = page.pageIndex;
		temp.pageType = page.pageType;
		temp.tableName = page.tableName;
		page = temp;*/
		return true;
	}

	/*
	fstream file(filename, ios::out | ios::in);
	if (file.fail())
		return false;
	file.seekp(PAGE_SIZE * page.ofs,ios::beg);
	if (!file.eof()) {
		file.read(temp.pageData, PAGE_SIZE);
	}
	file.close();
	*/
	return true;
}

bool BufferManager::ForcedWritePageToFile(Page& page)
{
	string filename = (page.tableName + "." + "record");
	FILE* fp = fopen(filename.c_str(), "rb+");
	if (!fp) {
		cout << "file open failed when writing" << endl;
		exit(0);
	}
	if (fp) 
	{
		fseek(fp, PAGE_SIZE * page.ofs, SEEK_SET);
		fwrite(page.pageData, PAGE_SIZE, SEEK_CUR, fp);
		fclose(fp);
		//delete& page;
		return true;
	}
	return false;


	/*
	 ofstream output(filename, ios::out);
	 if (output.fail())
		 return false;
	 output.seekp(PAGE_SIZE * page.ofs, ios::beg);
	 output.write(page.pageData, PAGE_SIZE);
	 output.close();
	 return true;
	 */
}

int BufferManager::findPageInCache(Page& page)
{
	int tempIndex = -1;
	for (int i = 0; i < CACHE_CAPACITY; i++)//һ��buffer����10���飨page��������buffer�ҵ���ǰpage
		if (cachePages[i] == page) 
		{
			tempIndex = i;
			break;
		}
	return tempIndex;
}

void BufferManager::LRUCounterAddExceptCurrent(int index)//������������ÿ��һ�����鱻ʹ��ʱ������������������ļ������������������ʹ�õ���0
{
	for (int i = 0; i < CACHE_CAPACITY; i++)
		LRUCounter[i]++;
	LRUCounter[index] = 0;
}

Page& BufferManager::recordManagerGetBlankPage()
{
	Page* newPage = new Page();
	newPage->pageType = PageType::RecordPage;
	Page& newPageRef = (*newPage);
	return newPageRef;
}

int BufferManager::getUnpinedBiggestPageFromCache()//�ҵ�������ֵ�����δ����ס��page��Ӧindex��ʹ�ô������٣�
{
	int retIndex = -1;
	int bigSaver = -1;
	for (int i = 0; i < CACHE_CAPACITY; i++) 
	{
		if ((!pined[i]) && (LRUCounter[i] > bigSaver)) //�����ǰpageδ����ס���Ҷ�Ӧ�����ȵ�ǰ���ֵ��
		{
			retIndex = i;
			bigSaver = LRUCounter[i];
		}
	}
	return retIndex;
}

bool BufferManager::pinPage(Page& page)//Pin���ܣ���ģ������ṩ�ӿڣ�����ģ���pin������ͬʱ��pin������и�״̬�ļ�¼
{
	int temp = false;
	for (int i = 0; i < CACHE_CAPACITY; ++i) 
	{
		if (cachePages[i] == page) 
		{
			pined[i] = true;//�鱻��ס
			temp = true;
			break;
		}
	}
	return temp;
}

bool BufferManager::unpinPage(Page& page)//����Pin���ܣ���ģ������ṩ�ӿڣ�����ģ���pinȡ��������ͬʱ��pin������и�״̬�ļ�¼
{
	int temp = false;
	for (int i = 0; i < CACHE_CAPACITY; ++i) 
	{
		if (cachePages[i] == page) 
		{
			pined[i] = false;
			temp = true;
			break;
		}
	}
	return temp;
}