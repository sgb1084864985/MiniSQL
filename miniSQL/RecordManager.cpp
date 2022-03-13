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
		{ //������Where����
			continue;
		}
		//row[]�ķ�������Ϊdataָ�룬row[mask[i]]->flag�ж��������ͣ�row[mask[i]]->x��ʾ��value���Ӧ��w[i].d��value���Ƚ�
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
	int pos = 0;//��ǰ�Ĳ���λ��
	ptrRes = new char[(tableIn.dataSize() + 1) * sizeof(char)];
	for (int i = 0; i < tableIn.getAttribute().num; i++) 
	{
		if (tableIn.getAttribute().flag[i] == -1) 
		{ //int
			int value = ((DataInt*)tempTuper[i])->x;
			memcpy(ptrRes + pos, &value, sizeof(int));//����Ӧ��ֵ��������Ӧ����������
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
			memcpy(ptrRes + pos, value.c_str(), strLen);//���1����������'\0';
			pos += strLen;
		}
	}
	ptrRes[tableIn.dataSize()] = '\0';
	return ptrRes;
}
Tuper RecordManager::String_Tuper(Table& tableIn, string stringRow)
{
	Tuper temp_tuper;
	if (stringRow.c_str()[0] == '#') return temp_tuper;//�����ǿյ�
	int c_pos = 1;//��ǰ����������ָ���λ�ã�0��ʾ��λ�Ƿ���Ч��������ݴӵ�һλ��ʼ
	for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
	{
		if (tableIn.getAttribute().flag[attr_index] == -1) {//��һ������
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
	}//���������ȴ��ļ�������һ��tuper
	return temp_tuper;
}
Tuper* RecordManager::Char_Tuper(Table& tableIn, char* stringRow)//ʵ�ֺ�Tuper_Char����
{
	Tuper* temp_tuper;
	temp_tuper = new Tuper;
	if (stringRow[0] == '#') 
		return temp_tuper;//�����ǿյ�
	int c_pos = 1;//��ǰ����������ָ���λ�ã�0��ʾ��λ�Ƿ���Ч��������ݴӵ�һλ��ʼ
	for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) {
		if (tableIn.getAttribute().flag[attr_index] == -1) 
		{//��һ������
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
	}//���������ȴ��ļ�������һ��tuper
	return temp_tuper;
}
int RecordManager::FindWithIndex(Table& tableIn, Tuper& row, int mask)
{
	//IndexManager indexMA;
	for (int i = 0; i < tableIn.index.num; i++) 
	{
		if (tableIn.index.location[i] == mask) 
		{ //�ҵ�����
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
		//�쳣("Can't delete the file!\n");
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
	int length = tableIn.dataSize() + 1; //һ��Ԫ�����Ϣ���ĵ��еĳ���
	const int recordNum = PAGE_SIZE / length; //һ��block�д洢�ļ�¼����
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
	{//��ȡ�����ļ��е���������
		Page tempPage;
		tempPage.tableName = tableIn.Tname;
		tempPage.ofs = tableIn.blockNum;
		tempPage.pageType = RecordPage;
		Page& readIn = tempPage;
		int pageIndex = buf_ptr->findPageInCache(readIn);

		buf_ptr->readPage(readIn);
		//int bufferNum = buf_ptr->getIfIsInBuffer(filename, blockOffset);
		//if (bufferNum == -1) 
		//{ //�ÿ鲻���ڴ��У���ȡ֮
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

Table RecordManager::SelectProject(Table& table, vector<int>attrSelect)//where������Ϊ�յ����
{
	//��ѡ�������Ԫ�����ͶӰ��ʹ�ý��ֻ�и���������
	Attribute update;
	Tuper* temp = NULL;
	update.num = attrSelect.size();//��ȡ�ض����Եĸ���
	for (int i = 0; i < attrSelect.size(); i++) 
	{
		update.flag[i] = table.getAttribute().flag[attrSelect[i]];//��ȡ�ض����Ե�����
		update.name[i] = table.getAttribute().name[attrSelect[i]];//��ȡ�ض����Ե�����
		update.isUnique[i] = table.getAttribute().isUnique[attrSelect[i]];//��ȡ�ض������Ƿ�Ψһ��Ϣ
	}
	Table tableOut(update, table.getName(), table.blockNum);//׼������µı�
	for (int i = 0; i < table.data.size(); i++)
	{//����Tuper�ĸ���
		temp = new Tuper;
		for (int j = 0; j < attrSelect.size(); j++)
		{
			int k = attrSelect[j];
			Data* add = NULL;
			if (table.data[i]->operator [](k)->flag == -1) //�������������int
			{
				add = new DataInt((*((DataInt*)table.data[i]->operator [](k))).x);
			}
			else if (table.data[i]->operator [](k)->flag == 0) //�������������float
			{
				add = new DataFloat((*((DataFloat*)table.data[i]->operator [](k))).x);
			}
			else if (table.data[i]->operator [](k)->flag > 0) //�������������char
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
		if (mask.size() == 0) //���where��û�������������޶�
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
				if (stringRow.c_str()[0] == '#') continue;//�����ǿյ�
				int c_pos = 1;//��ǰ����������ָ���λ�ã�0��ʾ��λ�Ƿ���Ч��������ݴӵ�һλ��ʼ
				Tuper* temp_tuper = new Tuper;
				for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
				{
					if (tableIn.getAttribute().flag[attr_index] == -1) 
					{//��һ������
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
				}//���������ȴ��ļ�������һ��tuper�������ж��Ƿ�����Ҫ��							
				if (isSatisfied(tableIn, *temp_tuper, mask, w)) {
					tableIn.addData(temp_tuper); //���ܻ��������;solved!
				}
				else delete temp_tuper;
			}
		}
		return SelectProject(tableIn, attrSelect);
	}
Table RecordManager::Select(Table& tableIn, vector<int>attrSelect)
	{
		string filename = tableIn.getName();
		int length = tableIn.dataSize() + 1; //һ��Ԫ�����Ϣ���ĵ��еĳ���
		string stringRow;
		Tuper* temp_tuper;
		
		const int recordNum = PAGE_SIZE / length; //һ��block�д洢�ļ�¼����
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
				if (stringRow.c_str()[0] == '#') continue;//�����ǿյ�
				int c_pos = 1;//��ǰ����������ָ���λ�ã�0��ʾ��λ�Ƿ���Ч��������ݴӵ�һλ��ʼ
				temp_tuper = new Tuper;
				for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
				{
					if (tableIn.getAttribute().flag[attr_index] == -1) 
					{//��һ������
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
				tableIn.addData(temp_tuper); //���ܻ��������;solved!
			}
		}
		return SelectProject(tableIn, attrSelect);
	}

InsertPos RecordManager::Insert(Table& tableIn, Tuper& tempTuper)
	{
	
	for (int i = 0; i < tableIn.attr.num; i++)//�ж�UniqueԼ��
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
		charTuper = Tuper_Char(tableIn, tempTuper);//��һ��Ԫ��ת�����ַ���
		InsertPos iPos = buf_ptr->getInsertPosition(tableIn);//��ȡ����λ��
		P.ofs = buf_ptr->cachePages[iPos.BlockNum].ofs;
		P.pageType = RecordPage;
		buf_ptr->readPage(P);
		
		P.pageData[iPos.Position] = 1;// "#" ==> "1"		
		memcpy(&(P.pageData[iPos.Position + 1]), charTuper, tableIn.dataSize());
		buf_ptr->writePage(P);

		int length = tableIn.dataSize() + 1;
		iPos.Position = iPos.Position / length;//��Position��ֵ��ΪΪ���еĵڼ�����¼����base0��
		//iPos.BlockNumδ�޸� ��API���޸� �������ǵڼ���������
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
				//�쳣throw QueryException("Unique Value Redundancy occurs, thus insertion failed");
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
			//	//�쳣throw QueryException("Unique Value Redundancy occurs, thus insertion failed");
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
	charTuper = Tuper_Char(tableIn, tempTuper);//��һ��Ԫ��ת�����ַ���
	//�ж��Ƿ�Unique
	InsertPos iPos = buf_ptr->getInsertPosition(tableIn);//��ȡ����λ��

	buf_ptr->cachePages[iPos.BlockNum].pageData[iPos.Position] = 1;
	memcpy(&(buf_ptr->cachePages[iPos.BlockNum].pageData[iPos.Position + 1]), charTuper, tableIn.dataSize());
	int length = tableIn.dataSize() + 1; //һ��Ԫ�����Ϣ���ĵ��еĳ���
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
		//int pageIndex = buf_ptr->findPageInCache(readIn);//���ļ����ڴ�

		buf_ptr->readPage(readIn);
		for (int offset = 0; offset < recordNum; offset++) 
		{
			int position = offset * length;
			stringRow = readIn.getvalues(position, position + length);
			if (stringRow.c_str()[0] == '#') continue;//�����ǿյ�
			int c_pos = 1;//��ǰ����������ָ���λ�ã�0��ʾ��λ�Ƿ���Ч��������ݴӵ�һλ��ʼ
			Tuper* temp_tuper = new Tuper;
			for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++) 
			{
				if (tableIn.getAttribute().flag[attr_index] == -1) 
				{//��һ������
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
			}//���������ȴ��ļ�������һ��tuper��һ���ж��Ƿ�����Ҫ��

			if (isSatisfied(tableIn, *temp_tuper, mask, w)) 
			{
				readIn.pageData[position] = '#'; //DELETED==EMYTP
				//ɾ����������
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
				//д��buffer
				buf_ptr->writePage(readIn);
				count++;
			}
		}
	}
	return count;
}

//add 
InsertPos RecordManager::getInsertPos(Table& tableIn, Tuper& temp)//�õ��Ѿ������Ԫ���λ��
{
	Page P;
	char* tuper_char;//��Ԫ���Ӧ���ַ���
	tuper_char = Tuper_Char(tableIn, temp);
	int length = tableIn.dataSize() + 1;//ÿ��Ԫ��ĳ���
	int recordNum = PAGE_SIZE / length;//ÿ�����д��Ԫ����
	InsertPos ret;
	ret.BlockNum = ret.Position = -1;//invalid
	bool flag = true;//�����жϴ���ѰԪ����ļ���Ԫ���Ƿ�һ��

	//��һ��ѭ����λΪ�� �ڶ���ѭ����λΪԪ��
	for (int i = 0; i < tableIn.blockNum; i++)//i ���ļ��еڼ�����
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
			if (stringRow.c_str()[0] == '#') continue;//�����ǿյ�
			int c_pos = 1;//��ǰ����������ָ���λ�ã�0��ʾ��λ�Ƿ���Ч��������ݴӵ�һλ��ʼ
			char* value = new char[length];
			memcpy(value, &(stringRow.c_str()[c_pos]), sizeof(char)*length);

			//Tuper* temp_tuper = new Tuper;
			//for (int attr_index = 0; attr_index < tableIn.getAttribute().num; attr_index++)
			//{
			//	if (tableIn.getAttribute().flag[attr_index] == -1)
			//	{//��һ������
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
			char CNM[4], RNM[4];//CNM ���ļ������
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