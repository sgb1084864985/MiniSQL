#include "RecordManager.h"
#include "IndexManager.h"
#include "CatalogManager.h"
#include <cmath>
RecordManager::~RecordManager()
{

}

bool RecordManager::isSatisfied(Table& tableinfor, Tuper& row, vector<int> mask, vector<Where> w)
{
	bool temp = true;

	for (int i = 0; i < mask.size(); i++) 
	{
		if (w[i].d == NULL) 
		{ //不存在Where条件
			continue;
		}
		//row[]的返回类型为data指针，row[mask[i]]->flag判断数据类型，row[mask[i]]->x表示其value与对应的w[i].d的value作比较
		else if (row[mask[i]]->flag == -1) 
		{ //int
			switch (w[i].flag) 
			{
				case eq: if (!(((DataInt*)row[mask[i]])->x == ((DataInt*)w[i].d)->x)) return false; break;
				case leq: if (!(((DataInt*)row[mask[i]])->x <= ((DataInt*)w[i].d)->x)) return false; break;
				case l: if (!(((DataInt*)row[mask[i]])->x < ((DataInt*)w[i].d)->x)) return false; break;
				case geq: if (!(((DataInt*)row[mask[i]])->x >= ((DataInt*)w[i].d)->x)) return false; break;
				case g: if (!(((DataInt*)row[mask[i]])->x > ((DataInt*)w[i].d)->x)) return false; break;
				case neq: if (!(((DataInt*)row[mask[i]])->x != ((DataInt*)w[i].d)->x)) return false; break;
				default:;
			}
		}
		else if (row[mask[i]]->flag == 0) 
		{ //Float
			switch (w[i].flag) 
			{
				case eq: if (!(abs(((DataFloat*)row[mask[i]])->x - ((DataFloat*)w[i].d)->x) < 0.0001)) return false; break;
				case leq: if (!(((DataFloat*)row[mask[i]])->x <= ((DataFloat*)w[i].d)->x)) return false; break;
				case l: if (!(((DataFloat*)row[mask[i]])->x < ((DataFloat*)w[i].d)->x)) return false; break;
				case geq: if (!(((DataFloat*)row[mask[i]])->x >= ((DataFloat*)w[i].d)->x)) return false; break;
				case g: if (!(((DataFloat*)row[mask[i]])->x > ((DataFloat*)w[i].d)->x)) return false; break;
				case neq: if (!(((DataFloat*)row[mask[i]])->x != ((DataFloat*)w[i].d)->x)) return false; break;
				default:;
			}
		}
		else if (row[mask[i]]->flag > 0) 
		{ //string
			switch (w[i].flag) 
			{
				case eq: if (!(((DataChar*)row[mask[i]])->x == ((DataChar*)w[i].d)->x)) return false; break;
				case leq: if (!(((DataChar*)row[mask[i]])->x <= ((DataChar*)w[i].d)->x)) return false; break;
				case l: if (!(((DataChar*)row[mask[i]])->x < ((DataChar*)w[i].d)->x)) return false; break;
				case geq: if (!(((DataChar*)row[mask[i]])->x >= ((DataChar*)w[i].d)->x)) return false; break;
				case g: if (!(((DataChar*)row[mask[i]])->x > ((DataChar*)w[i].d)->x)) return false; break;
				case neq: if (!(((DataChar*)row[mask[i]])->x != ((DataChar*)w[i].d)->x)) return false; break;
				default:;
			}
		}
		else 
		{ //just for debug
			cout << "Error in RecordManager in function is satisified!" << endl;
			system("pause");
		}
	}
	return temp;
}

char* RecordManager::Tuper_Char(Table& tableIn, Tuper& tempTuper)
{
	char* ptrRes;
	int pos = 0;//当前的插入位置
	ptrRes = new char[(tableIn.dataSize() + 1) * sizeof(char)];
	for (int i = 0; i < tableIn.getAttribute().num; i++) 
	{
		if (tableIn.getAttribute().flag[i] == -1) 
		{ //int
			int value = ((DataInt*)tempTuper[i])->x;
			memcpy(ptrRes + pos, &value, sizeof(int));//把相应的值拷贝到对应的数据区块
			pos += sizeof(int);
		}
		else if (tableIn.getAttribute().flag[i] == 0) 
		{
			float value = ((DataFloat*)tempTuper[i])->x;
			memcpy(ptrRes + pos, &value, sizeof(float));
			char bug[100];//debug
			memcpy(bug, &value, sizeof(float));//debug
			pos += sizeof(float);
		}
		else 
		{ //string
			string value(((DataChar*)tempTuper[i])->x);
			int strLen = tableIn.getAttribute().flag[i] + 1;
			memcpy(ptrRes + pos, value.c_str(), strLen);//多加1，拷贝最后的'\0';
			pos += strLen;
		}
	}
	ptrRes[tableIn.dataSize()] = '\0';
	return ptrRes;
}
Tuper RecordManager::String_Tuper(Table& tableIn, string stringRow)
{
	Tuper temp_tuper;
	if (stringRow.c_str()[0] == '#') return temp_tuper;//该行是空的
	int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
	for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
	{
		if (tableIn.getAttribute().flag[attr_index] == -1) {//是一个整数
			int value;
			memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(int));
			c_pos += sizeof(int);
			temp_tuper.addData(new DataInt(value));
		}
		else if (tableIn.getAttribute().flag[attr_index] == 0) {//float
			float value;
			memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(float));
			c_pos += sizeof(float);
			temp_tuper.addData(new DataFloat(value));
		}
		else {
			char value[100];
			int strLen = tableIn.getAttribute().flag[attr_index] + 1;
			memcpy(value, &(stringRow.c_str()[c_pos]), strLen);
			c_pos += strLen;
			temp_tuper.addData(new DataChar(string(value)));
		}
	}//以上内容先从文件中生成一行tuper
	return temp_tuper;
}
Tuper* RecordManager::Char_Tuper(Table& tableIn, char* stringRow)//实现和Tuper_Char类似
{
	Tuper* temp_tuper;
	temp_tuper = new Tuper;
	if (stringRow[0] == '#') 
		return temp_tuper;//该行是空的
	int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
	for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) {
		if (tableIn.getAttribute().flag[attr_index] == -1) 
		{//是一个整数
			int value;
			memcpy(&value, &(stringRow[c_pos]), sizeof(int));
			c_pos += sizeof(int);
			temp_tuper->addData(new DataInt(value));
		}
		else if (tableIn.getAttribute().flag[attr_index] == 0) 
		{//float
			float value;
			memcpy(&value, &(stringRow[c_pos]), sizeof(float));
			c_pos += sizeof(float);
			temp_tuper->addData(new DataFloat(value));
		}
		else 
		{
			char value[100];
			int strLen = tableIn.getAttribute().flag[attr_index] + 1;
			memcpy(value, &(stringRow[c_pos]), strLen);
			c_pos += strLen;
			temp_tuper->addData(new DataChar(string(value)));
		}
	}//以上内容先从文件中生成一行tuper
	return temp_tuper;
}
int RecordManager::FindWithIndex(Table& tableIn, Tuper& row, int mask)
{
	//IndexManager indexMA;
	for (int i = 0; i < tableIn.index.num; i++) 
	{
		if (tableIn.index.location[i] == mask) 
		{ //找到索引
			Data* ptrData;
			ptrData = row[mask];
			int pos = 1;//indexMA.Find(tableIn.getName() + to_string(mask) + ".index", ptrData);
			return pos;
		}
	}
	return -1;
}

bool RecordManager::DropTable(Table& tableIn)
{
	string filename = tableIn.getName();
	if (remove(filename.c_str()) != 0) {
		//异常("Can't delete the file!\n");
	}
	else 
		buf_ptr->setInvalid(filename);
	return true;
}
bool RecordManager::CreateTable(Table& tableIn)
{

	string filename = tableIn.getName();
	fstream fout(filename.c_str(), ios::out);
	fout.close();
	tableIn.blockNum = 1;
	//CatalogManager Ca;
	//Ca.changeblock(tableIn.getName(), 1);
	return true;
}

bool RecordManager::UNIQUE(Table& tableIn, Where w, int loca) 
{
	int length = tableIn.dataSize() + 1; //一个元组的信息在文档中的长度
	const int recordNum = PAGE_SIZE / length; //一个block中存储的记录条数
	string stringRow;
	string filename = tableIn.getName();
	int attroff = 1;
	for (int i = 0; i < loca - 1; i++) 
	{
		if (tableIn.attr.flag[i] == -1) 
		{
			attroff += sizeof(int);
		}
		else if (tableIn.attr.flag[i] == 0) 
		{
			attroff += sizeof(float);
		}
		else 
		{
			attroff += sizeof(char) * tableIn.attr.flag[i];
		}
	}
	int inflag = tableIn.attr.flag[loca];
	for (int blockOffset = 0; blockOffset < tableIn.blockNum; blockOffset++) 
	{//读取整个文件中的所有内容
		Page tempPage;
		tempPage.tableName = tableIn.Tname;
		tempPage.ofs = tableIn.blockNum;
		tempPage.pageType = RecordPage;
		Page& readIn = tempPage;
		int pageIndex = buf_ptr->findPageInCache(readIn);

		buf_ptr->readPage(readIn);
		//int bufferNum = buf_ptr->getIfIsInBuffer(filename, blockOffset);
		//if (bufferNum == -1) 
		//{ //该块不再内存中，读取之
		//	bufferNum = buf_ptr->getEmptyBuffer();
		//	buf_ptr->readBlock(filename, blockOffset, bufferNum);
		//}
		for (int offset = 0; offset < recordNum; offset++) {
			int position = offset * length + attroff;
			if (inflag == -1) {
				int value;
				memcpy(&value, &(buf_ptr->cachePages[pageIndex].pageData[position + 4]), sizeof(int));
				if (value == ((DataInt*)(w.d))->x)
					return false;
			}
			else if (inflag == 0) {
				float value;
				memcpy(&value, &(buf_ptr->cachePages[pageIndex].pageData[position + 4]), sizeof(float));
				if (value == ((DataFloat*)(w.d))->x)
					return false;
			}
			else {
				char value[100];
				memcpy(value, &(buf_ptr->cachePages[pageIndex].pageData[position + 4]), tableIn.attr.flag[loca] + 1);
				if (string(value) == ((DataChar*)(w.d))->x)
					return false;
			}

		}
	}
	return true;

}

Table RecordManager::SelectProject(Table& table, vector<int>attrSelect)//where后条件为空的情况
{
	//将选择出来的元组进行投影，使得结果只有给定的属性
	Attribute update;
	Tuper* temp = NULL;
	update.num = attrSelect.size();//获取特定属性的个数
	for (int i = 0; i < attrSelect.size(); i++) 
	{
		update.flag[i] = table.getAttribute().flag[attrSelect[i]];//获取特定属性的类型
		update.name[i] = table.getAttribute().name[attrSelect[i]];//获取特定属性的名称
		update.isUnique[i] = table.getAttribute().isUnique[attrSelect[i]];//获取特定属性是否唯一信息
	}
	Table tableOut(update, table.getName(), table.blockNum);//准备输出新的表
	for (int i = 0; i < table.data.size(); i++)
	{//遍历Tuper的个数
		temp = new Tuper;
		for (int j = 0; j < attrSelect.size(); j++)
		{
			int k = attrSelect[j];
			Data* add = NULL;
			if (table.data[i]->operator [](k)->flag == -1) //如果属性类型是int
			{
				add = new DataInt((*((DataInt*)table.data[i]->operator [](k))).x);
			}
			else if (table.data[i]->operator [](k)->flag == 0) //如果属性类型是float
			{
				add = new DataFloat((*((DataFloat*)table.data[i]->operator [](k))).x);
			}
			else if (table.data[i]->operator [](k)->flag > 0) //如果属性类型是char
			{
				add = new DataChar((*((DataChar*)table.data[i]->operator [](k))).x);
			}

			temp->addData(add);//bug

		}
		tableOut.addData(temp);
		//return tableOut;
	}
	return tableOut;
}
Table RecordManager::Select(Table& tableIn, vector<int>attrSelect, vector<int>mask, vector<Where>& w)
	{
		if (mask.size() == 0) //如果where中没有特殊条件的限定
		{
			return Select(tableIn, attrSelect);
		}
		string stringRow;
		
		string filename = tableIn.getName();
		string indexfilename;
		int length = tableIn.dataSize() + 1;
		const int recordNum = PAGE_SIZE / length;

		
		for (int blockOffset = 0; blockOffset < tableIn.blockNum; blockOffset++) 
		{
			Page tempPage;
			tempPage.tableName = tableIn.Tname;
			tempPage.ofs = blockOffset;
			tempPage.pageType = RecordPage;
			Page& readIn = tempPage;

			buf_ptr->readPage(readIn);
			for (int offset = 0; offset < recordNum; offset++) {
				int position = offset * length;
				stringRow = readIn.getvalues(position, position + length);
				if (stringRow.c_str()[0] == '#') continue;//该行是空的
				int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
				Tuper* temp_tuper = new Tuper;
				for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
				{
					if (tableIn.getAttribute().flag[attr_index] == -1) 
					{//是一个整数
						int value;
						memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(int));
						c_pos += sizeof(int);
						temp_tuper->addData(new DataInt(value));
					}
					else if (tableIn.getAttribute().flag[attr_index] == 0) 
					{//float
						float value;
						memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(float));
						c_pos += sizeof(float);
						temp_tuper->addData(new DataFloat(value));
					}
					else 
					{
						char value[100];
						int strLen = tableIn.getAttribute().flag[attr_index] + 1;
						memcpy(value, &(stringRow.c_str()[c_pos]), strLen);
						c_pos += strLen;
						temp_tuper->addData(new DataChar(string(value)));
					}
				}//以上内容先从文件中生成一行tuper，以下判断是否满足要求							
				if (isSatisfied(tableIn, *temp_tuper, mask, w)) {
					tableIn.addData(temp_tuper); //可能会存在问题;solved!
				}
				else delete temp_tuper;
			}
		}
		return SelectProject(tableIn, attrSelect);
	}
Table RecordManager::Select(Table& tableIn, vector<int>attrSelect)
	{
		string filename = tableIn.getName();
		int length = tableIn.dataSize() + 1; //一个元组的信息在文档中的长度
		string stringRow;
		Tuper* temp_tuper;
		
		const int recordNum = PAGE_SIZE / length; //一个block中存储的记录条数
		for (int blockOffset = 0; blockOffset < tableIn.blockNum; blockOffset++) 
		{
			Page tempPage;
			tempPage.tableName = tableIn.Tname;
			tempPage.ofs = blockOffset;
			tempPage.pageType = RecordPage;
			Page& readIn = tempPage;
			buf_ptr->readPage(readIn);
			for (int offset = 0; offset < recordNum; offset++) {
				int position = offset * length;
				stringRow = readIn.getvalues(position, position + length);
				if (stringRow.c_str()[0] == '#') continue;//该行是空的
				int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
				temp_tuper = new Tuper;
				for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
				{
					if (tableIn.getAttribute().flag[attr_index] == -1) 
					{//是一个整数
						int value;
						memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(int));
						c_pos += sizeof(int);
						temp_tuper->addData(new DataInt(value));
					}
					else if (tableIn.getAttribute().flag[attr_index] == 0) 
					{//float
						float value;
						memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(float));
						c_pos += sizeof(float);
						temp_tuper->addData(new DataFloat(value));
					}
					else {
						char value[300];
						int strLen = tableIn.getAttribute().flag[attr_index] + 1;
						memcpy(value, &(stringRow.c_str()[c_pos]), strLen);
						c_pos += strLen;
						temp_tuper->addData(new DataChar(string(value)));
					}
				}
				tableIn.addData(temp_tuper); //可能会存在问题;solved!
			}
		}
		return SelectProject(tableIn, attrSelect);
	}

InsertPos RecordManager::Insert(Table& tableIn, Tuper& tempTuper)
	{
	
	for (int i = 0; i < tableIn.attr.num; i++)//判断Unique约束
	{
		if (tableIn.attr.isUnique[i]) 
		{
			Table ret = tableIn;
			vector<int> attr, mask;
			vector<Where> w;
			attr.push_back(i);
			mask.push_back(i);
			Where temp;
			temp.AttributeIndex = i;
			temp.AttributeName = tableIn.attr.name[i];
			temp.d = tempTuper[i];
			temp.flag = eq;
			w.push_back(temp);
			//ret = Select(tableIn, attr, mask, w);
			IndexManager IM(tableIn, i, false);
			IM.select(*(tempTuper[i]), ret);
			
			if (ret.data.size() != 0)
			{
				throw TableException("ERROR:Insert Failed! The attribute " + tableIn.attr.name[i] + " is UNIQUE!");
			}
		}
	}

		Page P;
		P.tableName = tableIn.getName();

		char* charTuper;
		charTuper = Tuper_Char(tableIn, tempTuper);//把一个元组转换成字符串
		InsertPos iPos = buf_ptr->getInsertPosition(tableIn);//获取插入位置
		P.ofs = buf_ptr->cachePages[iPos.BlockNum].ofs;
		P.pageType = RecordPage;
		buf_ptr->readPage(P);
		
		P.pageData[iPos.Position] = 1;// "#" ==> "1"		
		memcpy(&(P.pageData[iPos.Position + 1]), charTuper, tableIn.dataSize());
		buf_ptr->writePage(P);

		int length = tableIn.dataSize() + 1;
		iPos.Position = iPos.Position / length;//将Position的值改为为块中的第几个记录。（base0）
		//iPos.BlockNum未修改 在API中修改 在这里是第几个缓存区
		delete[] charTuper;
		return iPos;
	}
void RecordManager::InsertWithIndex(Table& tableIn, Tuper& tempTuper)
{
	//check Redundancy using index
	for (int i = 0; i < tableIn.attr.num; i++) 
	{
		if (tableIn.attr.isUnique[i] == 1) 
		{
			int addr = FindWithIndex(tableIn, tempTuper, i);
			if (addr >= 0) 
			{ //already in the table 
				//异常throw QueryException("Unique Value Redundancy occurs, thus insertion failed");
				return;
			}
		}
	}

	for (int i = 0; i < tableIn.attr.num; i++) 
	{
		if (tableIn.attr.isUnique[i]) 
		{
			vector<Where> w;
			vector<int> mask;
			Where* uni_w = new Where;
			uni_w->flag = eq;
			switch (tempTuper[i]->flag) {
			case -1:uni_w->d = new DataInt(((DataInt*)tempTuper[i])->x); break;
			case 0:uni_w->d = new DataFloat(((DataFloat*)tempTuper[i])->x); break;
			default:uni_w->d = new DataChar(((DataChar*)tempTuper[i])->x); break;
			}
			w.push_back(*uni_w);
			mask.push_back(i);
			Table temp_table = Select(tableIn, mask, mask, w);


			//if (temp_table.T.size() != 0) {
			//	//异常throw QueryException("Unique Value Redundancy occurs, thus insertion failed");
			//}
			//code by hrg

		/*	if (!UNIQUE(tableIn, w[0], i)) {
				throw QueryException("Unique Value Redundancy occurs, thus insertion failed");
			}*/

			//code by hrg

			delete uni_w->d;
			delete uni_w;
		}
	}
	tableIn.addData(&tempTuper);
	char* charTuper;
	charTuper = Tuper_Char(tableIn, tempTuper);//把一个元组转换成字符串
	//判断是否Unique
	InsertPos iPos = buf_ptr->getInsertPosition(tableIn);//获取插入位置

	buf_ptr->cachePages[iPos.BlockNum].pageData[iPos.Position] = 1;
	memcpy(&(buf_ptr->cachePages[iPos.BlockNum].pageData[iPos.Position + 1]), charTuper, tableIn.dataSize());
	int length = tableIn.dataSize() + 1; //一个元组的信息在文档中的长度
	//insert tuper into index file
	//IndexManager indexMA;
	//int blockCapacity = PAGE_SIZE / length;
	//for (int i = 0; i < tableIn.index.num; i++) 
	//{
	//	int tuperAddr = buf_ptr->cachePages[iPos.BlockNum].ofs * blockCapacity + iPos.Position / length; //the tuper's addr in the data file
	//	for (int j = 0; j < tableIn.index.num; j++) 
	//	{
	//		//indexMA.Insert(tableIn.getName() + to_string(tableIn.index.location[j]) + ".index", tempTuper[tableIn.index.location[i]], tuperAddr);
	//	}
	//}
	//buf_ptr->writeBlock(iPos.BlockNum);
	delete[] charTuper;
}

int RecordManager::Delete(Table& tableIn, vector<int>mask, vector<Where> w)
{
	string filename = tableIn.getName();
	string stringRow;

	int count = 0;
	int length = tableIn.dataSize() + 1;
	const int recordNum = PAGE_SIZE / length;
	for (int blockOffset = 0; blockOffset < tableIn.blockNum; blockOffset++) 
	{
		Page tempPage;
		tempPage.tableName = tableIn.Tname;
		tempPage.ofs = blockOffset;
		tempPage.pageType = RecordPage;
		Page& readIn = tempPage;
		//int pageIndex = buf_ptr->findPageInCache(readIn);//读文件到内存

		buf_ptr->readPage(readIn);
		for (int offset = 0; offset < recordNum; offset++) 
		{
			int position = offset * length;
			stringRow = readIn.getvalues(position, position + length);
			if (stringRow.c_str()[0] == '#') continue;//该行是空的
			int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
			Tuper* temp_tuper = new Tuper;
			for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
			{
				if (tableIn.getAttribute().flag[attr_index] == -1) 
				{//是一个整数
					int value;
					memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(int));
					c_pos += sizeof(int);
					temp_tuper->addData(new DataInt(value));
				}
				else if (tableIn.getAttribute().flag[attr_index] == 0) 
				{//float
					float value;
					memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(float));
					c_pos += sizeof(float);
					temp_tuper->addData(new DataFloat(value));
				}
				else 
				{
					char value[100];
					int strLen = tableIn.getAttribute().flag[attr_index] + 1;
					memcpy(value, &(stringRow.c_str()[c_pos]), strLen);
					c_pos += strLen;
					temp_tuper->addData(new DataChar(string(value)));
				}
			}//以上内容先从文件中生成一行tuper，一下判断是否满足要求

			if (isSatisfied(tableIn, *temp_tuper, mask, w)) 
			{
				readIn.pageData[position] = '#'; //DELETED==EMYTP
				//删除更新索引
				if (tableIn.index.num != 0)
				{
					for (int i = 0; i < tableIn.attr.num; i++)
					{
						if (tableIn.index.location[i] > 0)
						{
							IndexManager IM(tableIn, i, false);
							IM.remove(*(temp_tuper->data[i]));
						}
					}
				}
				//写回buffer
				buf_ptr->writePage(readIn);
				count++;
			}
		}
	}
	return count;
}

//add 
InsertPos RecordManager::getInsertPos(Table& tableIn, Tuper& temp)//得到已经插入的元组的位置
{
	Page P;
	char* tuper_char;//该元组对应的字符串
	tuper_char = Tuper_Char(tableIn, temp);
	int length = tableIn.dataSize() + 1;//每个元组的长度
	int recordNum = PAGE_SIZE / length;//每个块中存的元组数
	InsertPos ret;
	ret.BlockNum = ret.Position = -1;//invalid
	bool flag = true;//用来判断待查寻元组和文件中元组是否一样

	//第一层循环单位为块 第二层循环单位为元组
	for (int i = 0; i < tableIn.blockNum; i++)//i 是文件中第几个块
	{
		P.ofs = i;
		P.tableName = tableIn.Tname;
		P.pageType = RecordPage;
		buf_ptr->readPage(P);
		for (int j = 0; j < recordNum; j++)
		{
			flag = true;
			int position = j * length;
			string stringRow = P.getvalues(position, position + length);
			if (stringRow.c_str()[0] == '#') continue;//该行是空的
			int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
			char* value = new char[length];
			memcpy(value, &(stringRow.c_str()[c_pos]), sizeof(char)*length);

			//Tuper* temp_tuper = new Tuper;
			//for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++)
			//{
			//	if (tableIn.getAttribute().flag[attr_index] == -1)
			//	{//是一个整数
			//		int value;
			//		memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(int));
			//		c_pos += sizeof(int);

			//		int x = ((DataInt*)temp[attr_index])->x;
			//		if (x != value)
			//		{
			//			flag = false;
			//			break;
			//		}
			//	}
			//	else if (tableIn.getAttribute().flag[attr_index] == 0)
			//	{//float
			//		//float value;
			//		//memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(float));
			//		//c_pos += sizeof(float);
			//		
			//		float x = ((DataFloat*)temp[attr_index])->x;
			//		//if (x != value) flag = false;
			//		char CNM[4], RNM[4];
			//		memcpy(CNM, &(stringRow.c_str()[c_pos]), sizeof(float));
			//		memcpy(RNM, &x, sizeof(float));
			//		if (CNM[0] != RNM[0] || CNM[1] != RNM[1] || CNM[2] != RNM[2] || CNM[3] != RNM[3])
			//		{
			//			flag = false;
			//			break;
			//		}
			//		c_pos += sizeof(float);

			//	}
			//	else
			//	{
			//		char value[100];
			//		int strLen = tableIn.getAttribute().flag[attr_index] + 1;
			//		memcpy(value, &(stringRow.c_str()[c_pos]), strLen);
			//		c_pos += strLen;
			//		
			//		string x = ((DataChar*)temp[attr_index])->x;
			//		string value_str = value;
			//		if (x != value)
			//		{
			//			flag = false;
			//			break;
			//		}
			//	}
			//}
			flag = isEqual(tableIn, stringRow, c_pos, temp);

			if (flag)
			{
				ret.BlockNum = i;
				ret.Position = j;
				return ret;
			}

		}
	}
	return ret;
}

bool RecordManager::isEqual(Table& tableIn, string stringRow, int& c_pos, Tuper& temp)
{
	bool flag = true;
	Tuper* temp_tuper = new Tuper;
	for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++)
	{
		if (tableIn.getAttribute().flag[attr_index] == -1)// Int
		{
			int value;
			memcpy(&value, &(stringRow.c_str()[c_pos]), sizeof(int));
			c_pos += sizeof(int);

			int x = ((DataInt*)temp[attr_index])->x;
			if (x != value)
			{
				flag = false;
				return flag;
			}
		}
		else if (tableIn.getAttribute().flag[attr_index] == 0)// Float
		{
			float x = ((DataFloat*)temp[attr_index])->x;
			char CNM[4], RNM[4];//CNM 是文件里读的
			memcpy(CNM, &(stringRow.c_str()[c_pos]), sizeof(float));
			memcpy(RNM, &x, sizeof(float));
			if (CNM[0] != RNM[0] || CNM[1] != RNM[1] || CNM[2] != RNM[2] || CNM[3] != RNM[3])
			{
				flag = false;
				return flag;
			}
			c_pos += sizeof(float);

		}
		else//String
		{
			char value[100];
			int strLen = tableIn.getAttribute().flag[attr_index] + 1;
			memcpy(value, &(stringRow.c_str()[c_pos]), strLen);
			c_pos += strLen;

			string x = ((DataChar*)temp[attr_index])->x;
			string value_str = value;
			if (x != value)
			{
				flag = false;
				return flag;
			}
		}
	}
}