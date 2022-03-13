#include "BufferManager.h"

bool BufferManager::readPage(Page& page)
{
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
			return false;
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
			return true;
		}
	}
}

bool BufferManager::writePage(Page& page)
{
	//assert(page.pageType != Undefined);//page的类型未定义直接退出程序防止后续出bug
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
		if (pageIndex == -1)//这里有bug 6/26 但基本不太会出现 先忽略
		{
			ForcedWritePageToFile(page);// May cause bug
			LRUCounterAddExceptCurrent(pageIndex);//更新每一个page对应的计数器的值
			return true;
		}
		else 
		{
			if (isDirty[pageIndex]) 
			{
				// isDirty[pageIndex] = false;
				ForcedWritePageToFile(cachePages[pageIndex]);//如果是脏块，写回文件
			}
			isDirty[pageIndex] = true;
			cachePages[pageIndex] = page;//替换
			LRUCounterAddExceptCurrent(pageIndex);
			return true;
		}
	}
	return false;
}

bool BufferManager::ForcedReadPageFromFile(Page& page)
{
	string filename = (page.tableName);
	Page temp;
	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp) 
	{
		fseek(fp, PAGE_SIZE * page.ofs, SEEK_SET);

		fread(page.pageData, PAGE_SIZE, SEEK_CUR, fp);
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
	string filename = (page.tableName);
	FILE* fp = fopen(filename.c_str(), "rb+");
	if (fp == nullptr)
	{
		throw TableException("No File!");
	}
	if (fp) 
	{
		fseek(fp, PAGE_SIZE * page.ofs, SEEK_SET);// 6/28修改 page.ofs ==> (page.ofs + 1)
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

void BufferManager::setInvalid(string tempname) 
{
	for (int i = 0; i < CACHE_CAPACITY; i++) 
	{
		if (cachePages[i].tableName == tempname) 
		{
			cachePages[i].isValid = 0;
			//cachePages[i].isWritten = 0;
		}
	}
}

InsertPos BufferManager::getInsertPosition(Table& temp) 
{
	InsertPos iPos;
	if (temp.blockNum == 0) 
	{ //new file and no block exist 
		iPos.BlockNum = AddBlockInFile(temp);
		iPos.Position = 0;
		return iPos;
	}
	string filename = temp.getName();
	int length = temp.dataSize() + 1; //多余的一位放在开头，表示是否有效

	Page tempPage;
	tempPage.tableName = temp.Tname;
	tempPage.ofs = (temp.blockNum - 1);//这里就是直接插入文件最后一个块！
	tempPage.pageType = RecordPage;
	Page& readIn = tempPage;


	readPage(readIn);
	int recordNum = PAGE_SIZE / length;
	for (int offset = 0; offset < recordNum; offset++) 
	{
		int position = offset * length;
		char isEmpty = readIn.getvalues(position);//检查第一位是否有效，判断该行是否有内容
		if (isEmpty == '#') 
		{//find an empty space
			//iPos.BlockNum = readIn.ofs;
			iPos.BlockNum = findPageInCache(readIn);
			iPos.Position = position;//这里是具体偏移量 6/28
			return iPos;
		}
	}
	//该block已经装满，新开一个block
	iPos.BlockNum = AddBlockInFile(temp);
	iPos.Position = 0;
	return iPos;
}

int BufferManager::AddBlockInFile(Table& temp) 
{
	//int PageIndex = getUnpinedBiggestPageFromCache();
	int PageIndex;
	Page t;
	t.isValid = 1;
	t.tableName = temp.getName();
	t.ofs = temp.blockNum++;
	t.pageType = RecordPage;

	readPage(t);
	PageIndex = findPageInCache(t);
	return PageIndex;
}