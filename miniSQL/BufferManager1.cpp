#include "BufferManager.h"

int BufferManager:: LRUCounter[CACHE_CAPACITY] = { 0 };//与缓冲区大小相同的计数器来记录缓冲块被使用的次数
Page BufferManager:: cachePages[CACHE_CAPACITY] = { Page() };//缓存使用了一个Page数组进行内存缓存,记录所有的block
bool BufferManager:: pined[CACHE_CAPACITY] = { false };//使用了一个bool数组来记录pin过的缓冲区
bool BufferManager:: isDirty[CACHE_CAPACITY] = { false };//使用了一个与缓冲区大小相同的bool数组记录缓冲区是否被写过

bool BufferManager::readPage(Page& page)
{
	//assert(page.pageIndex == PageType::Undefined);
	int pageIndex = findPageInCache(page);//找到当前所处的page
	if (pageIndex != -1) 
	{
		// 直接从快速缓冲储存区cache读
		page = cachePages[pageIndex];
		LRUCounterAddExceptCurrent(pageIndex);//更新每一个page对应的计数器的值
		return true;
	}
	//如果没有找到当前所处的page
	else {
		ForcedReadPageFromFile(page);//从磁盘文件读（强制读）
		pageIndex = getUnpinedBiggestPageFromCache();//找到目标替换项
		if (pageIndex == -1) 
			return true;
		else 
		{
			if (isDirty[pageIndex]) 
			{
				isDirty[pageIndex] = false;
				// If it's dirty page, write it again to file
				ForcedWritePageToFile(cachePages[pageIndex]);//如果是脏块，写回文件
			}
			// use the new page to replace the page
			cachePages[pageIndex] = page;//替换
			LRUCounterAddExceptCurrent(pageIndex);
			//cout << (void*)page.pageData<<endl;
			//cout << (void*)cachePages[pageIndex].pageData<<endl;
			return true;
		}
	}
}

bool BufferManager::writePage(Page& page)
{
	assert(page.pageType != Undefined);//page的类型未定义直接退出程序防止后续出bug
	int pageIndex = findPageInCache(page);//找到当前所处的page
	if (pageIndex != -1) 
	{
		cachePages[pageIndex] = page;//直接写入
		isDirty[pageIndex] = true;
		LRUCounterAddExceptCurrent(pageIndex);//更新每一个page对应的计数器的值
		return true;
	}
	else 
	{
		pageIndex = getUnpinedBiggestPageFromCache();//找到目标替换项	
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
				ForcedWritePageToFile(cachePages[pageIndex]);//如果是脏块，写回文件
			}
			cachePages[pageIndex] = page;//替换
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
	for (int i = 0; i < CACHE_CAPACITY; i++)//一个buffer含有10个块（page），遍历buffer找到当前page
		if (cachePages[i] == page) 
		{
			tempIndex = i;
			break;
		}
	return tempIndex;
}

void BufferManager::LRUCounterAddExceptCurrent(int index)//计数器函数，每当一个区块被使用时，缓存区内其他区块的计数器会进行自增，被使用的置0
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

int BufferManager::getUnpinedBiggestPageFromCache()//找到计数器值最大且未被钉住的page对应index（使用次数最少）
{
	int retIndex = -1;
	int bigSaver = -1;
	for (int i = 0; i < CACHE_CAPACITY; i++) 
	{
		if ((!pined[i]) && (LRUCounter[i] > bigSaver)) //如果当前page未被钉住并且对应计数比当前最大值大
		{
			retIndex = i;
			bigSaver = LRUCounter[i];
		}
	}
	return retIndex;
}

bool BufferManager::pinPage(Page& page)//Pin功能，本模块对外提供接口，进行模块的pin锁定。同时用pin数组进行该状态的记录
{
	int temp = false;
	for (int i = 0; i < CACHE_CAPACITY; ++i) 
	{
		if (cachePages[i] == page) 
		{
			pined[i] = true;//块被钉住
			temp = true;
			break;
		}
	}
	return temp;
}

bool BufferManager::unpinPage(Page& page)//解锁Pin功能，本模块对外提供接口，进行模块的pin取消锁定。同时用pin数组进行该状态的记录
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