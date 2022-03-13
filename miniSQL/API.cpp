#include "API.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

bool Comp(Where& w1, Where& w2);

void API::INSERT(std::string TName, Tuper& inTuper)
{
	RecordManager RM(BM);
	CatalogManager CM;
	Table T;
	CM.getTable(TName, T);
	int blockNum = T.blockNum;
	
	//debug
	/*
	for (int i = 0; i < T.getColumnNum(); i++)
	{
		cout << "API收到:";
		switch (T.attr.flag[i])
		{
		case -1: cout << ((DataInt*)inTuper.data[i])->x << endl; break;
		case 0: cout << ((DataFloat*)inTuper.data[i])->x << endl; break;
		default: cout << ((DataChar*)inTuper.data[i])->x << endl; break;
		}
	}
	*/

	InsertPos ret = RM.Insert(T, inTuper);//调用record manger
	//CM.UpdateTable(T.Tname, T);
	if (T.blockNum != blockNum)
	{
		CM.UpdateTableBlockNum(T);
	}
	
	//CM.getTable(T.Tname, T);
	cout << "缓冲区插入位置是 " << ret.BlockNum << " " << ret.Position << endl;

	//InsertPos temp = RM.getInsertPos(T, inTuper);
	InsertPos temp;
	temp.Position = ret.Position;
	temp.BlockNum = T.blockNum - 1;

	//temp = RM.getInsertPos(T, inTuper);//debug

	cout << "文件中插入位置是 " << temp.BlockNum << " " << temp.Position << endl;
	//CM.getTable(TName, T);//得到更新后的表

	int BlockNum = temp.BlockNum;
	int Offset = temp.Position * (T.dataSize() + 1);
	if (T.index.num > 0)
	{
		for (int i = 0; i < 32; i++)
		{
			if (T.index.location[i] == 1 || T.index.location[i] == 2)
			{
				IndexManager IM(T, i, false);
				IM.insert(*inTuper[i], BlockNum, Offset);
			}	
		}
	}


	return;

}

Table API::SELECT(Table& inTable, std::vector<Where> w, std::vector<int> selectAttr)
{
	clock_t start = 0, end = 0;
	RecordManager RM(BM);
	int size = w.size();
	vector<int> mask;

	Table res = inTable;
	bool flag = false;//where 条件中是否出现有索引的属性

	//对w进行排序
	sort(w.begin(), w.end(), Comp);
	for (int i = 0; i < w.size(); i++)
	{
		if (inTable.index.location[w[i].AttributeIndex] > 0)
		{
			flag = true;
			break;
		}
	}

	Table procedure = inTable;//过程中的table
	bool isEmpty = true;//是否是第一次将中间元组放入结果元组
	int AttrWithIndex;
	int neq = 0;
	// = 0     <> 1      < 2    <= 3           > 4          >= 5
	start = clock();//开始计时
	if (flag)//有属性存在索引
	{		
		for (int i = 0; i < w.size(); i++)
		{
			if (inTable.index.location[w[i].AttributeIndex] > 0)//where属性上有索引
			{
				AttrWithIndex = w[i].AttributeIndex;
				if (w[i].flag == eq)
				{
					IndexManager IM(inTable, w[i].AttributeIndex, false);
					IM.select(*(w[i].d), res);
					if (i + 1 < w.size() && w[i + 1].AttributeIndex == w[i].AttributeIndex)
					{
						throw QueryException("ERROR: equal can't exist with less or greater!");
					}
					
					
					end = clock();//停止计时
					double endtime = (double)(end - start) / CLOCKS_PER_SEC;
					cout << "Select time (with index):" << endtime * 1000 << "ms" << endl;
					return res;
				}
				else if (w[i].flag == l)//less 注意要求输入不能包括< 和 <= , > 和>=
				{
					IndexManager IM(inTable, w[i].AttributeIndex, false);
					Data *key1 = NULL, *key2 = NULL;
					bool bound1 = false, bound2 = false;
					key2 = w[i].d;
					if (i + 1 < w.size() && w[i + 1].AttributeIndex == w[i].AttributeIndex)
					{
						if (w[i + 1].flag == g)//value1 < key < value2
						{
							key1 = w[i + 1].d;
						}
						else if (w[i + 1].flag == geq)//value1 <= key < value2
						{
							key1 = w[i + 1].d;
							bound1 = true;
						}
						else
						{
							throw QueryException("ERROR: PLease ask us why!");
						}
						i++;
					}

					IM.range_select(key1, key2, bound1, bound2, procedure);
					break;
				}
				else if (w[i].flag == leq) //key <= value2
				{
					IndexManager IM(inTable, w[i].AttributeIndex, false);
					Data* key1 = NULL, * key2 = NULL;
					bool bound1 = false, bound2 = true;
					key2 = w[i].d;
					if (i + 1 < w.size() && w[i + 1].AttributeIndex == w[i].AttributeIndex)
					{
						if (w[i + 1].flag == g) //value1 < key <= value2
						{
							key1 = w[i + 1].d;
						}
						else if (w[i + 1].flag == geq) // value1 <= key <= value2
						{
							key1 = w[i + 1].d;
							bound1 = true;
						}
						else
						{
							throw QueryException("ERROR: PLease ask us why!");
						}
						i++;
					}
					IM.range_select(key1, key2, bound1, bound2, procedure);
					break;
				}
				else if (w[i].flag == g)
				{
					IndexManager IM(inTable, w[i].AttributeIndex, false);
					Data* key1 = NULL, * key2 = NULL;
					bool bound1 = false, bound2 = false;
					key1 = w[i].d;
					if (i + 1 < w.size() && w[i + 1].AttributeIndex == w[i].AttributeIndex)
					{
						throw QueryException("ERROR169!");
					}
					IM.range_select(key1, key2, bound1, bound2, procedure);
					break;
				}
				else if (w[i].flag == geq)
				{
					IndexManager IM(inTable, w[i].AttributeIndex, false);
					Data* key1 = NULL, * key2 = NULL;
					bool bound1 = true, bound2 = false;
					key1 = w[i].d;
					if (i + 1 < w.size() && w[i + 1].AttributeIndex == w[i].AttributeIndex)
					{
						throw QueryException("ERROR182!");
					}
					IM.range_select(key1, key2, bound1, bound2, procedure);
					break;
				}
				else if (w[i].flag == neq) {
					neq = 1;
					break;
				}
				//将procedure的内容放到res中
				if (isEmpty)
				{
					res = procedure;
					isEmpty = false;
					break;
				}
			}

		}
		res = inTable;
		//现在的索引挑出的元组都在procedure中
		for (int i = 0; i < size; i++)
		{
			if (w[i].AttributeIndex != AttrWithIndex) mask.push_back(w[i].AttributeIndex);//mask中是where子句中属性的序号
		}
		vector<Where> NEW;//用来存除了刚才使用的索引属性之外的属性
		for (int i = 0; i < w.size(); i++)
		{
			if (w[i].AttributeIndex != AttrWithIndex) NEW.push_back(w[i]);
		}
		for (int i = 0; i < procedure.getRowNum(); i++)
		{
			Tuper* temp_tuper = procedure.data[i];
			if (RM.isSatisfied(inTable, *temp_tuper, mask, NEW)) {
				res.addData(temp_tuper);
			}
		}
		end = clock();//停止计时
		double endtime = (double)(end - start) / CLOCKS_PER_SEC;
		cout << "Select time (with index):" << endtime * 1000 << "ms" << endl;
		if(!neq)return res;
	}

	for (int i = 0; i < size; i++)
	{
		mask.push_back(w[i].AttributeIndex);//mask中是where子句中属性的序号
	}

	//没索引老老实实linear search
	Table ret = RM.Select(inTable, selectAttr, mask, w);
	end = clock();//停止计时
	double endtime = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "Select time (without index):" << endtime * 1000 << "ms" << endl;

	cout << "返回Table共有";
	cout << ret.getColumnNum() << "列, ";
	cout << ret.getRowNum() << "行" << endl;

	return ret;
}

int API::DELETE(Table& inTable, std::vector<Where> w, bool hasWhere)
{
	RecordManager RM(BM);
	vector<int> mask;
	int size = w.size();
	for (int i = 0; i < size; i++)
	{
		mask.push_back(w[i].AttributeIndex);
	}
	int count = RM.Delete(inTable, mask, w);

	cout << "Delete number " << count << " ;" << endl;
	return count;
}

void API::CREATETABLE(Table& inTable)
{
	RecordManager RM(BM);
	//调用record manager
	RM.CreateTable(inTable);
	if (inTable.primary != -1)//如果有主键 则对主键建立聚集索引
	{
		IndexManager IM(inTable, inTable.primary, true);
	}
	for (int i = 0; i < inTable.attr.num; i++)
	{
		if (i!=inTable.primary && inTable.attr.isUnique[i])
		{
			IndexManager IM2(inTable, i, true);
		}
	}
	return;

}

void API::CREATEINDEX(Table& inTable, int Attr_index)
{
	RecordManager RM(BM);
	vector<int> attrSelect;
	for (int i = 0; i < inTable.getColumnNum(); i++)
	{
		attrSelect.push_back(i);
	}
	Table temp = RM.Select(inTable, attrSelect);
	IndexManager IM(inTable, Attr_index, true);//建立索引
	Tuper tup;
	int length = inTable.dataSize() + 1;

	Page P;
	P.tableName = inTable.Tname;
	P.pageType = RecordPage;
	int recordNum = PAGE_SIZE / length;
	int flag;
	for (int i = 0; i < inTable.blockNum; i++)
	{
		P.ofs = i;
		BM->readPage(P);
		for (int j = 0; j < recordNum; j++)
		{
			flag = true;
			int position = j * length;
			string stringRow = P.getvalues(position, position + length);
			if (stringRow.c_str()[0] == '#') continue;//该行是空的
			int c_pos = 1;//当前在数据流中指针的位置，0表示该位是否有效，因此数据从第一位开始
			char* value = new char[length];
			memcpy(value, &(stringRow.c_str()[c_pos]), sizeof(char) * length);
			tup = RM.String_Tuper(inTable, stringRow);
			IM.insert(*tup[Attr_index], i, j * (inTable.dataSize() + 1));
		}
	}


}

bool API::DropTable(Table& inTable)
{
	RecordManager RM(BM);
	bool res;
	res = RM.DropTable(inTable);
	if (res) cout << "删掉了文件" << endl;
	if (inTable.index.num > 0)
	{
		for (int i = 0; i < 32; i++)
		{
			if (inTable.index.location[i] == 1 || inTable.index.location[i] == 2)
			{
				//IndexManager IM(inTable.Tname, i, true);//如果某个属性存在索引删除索引文件
				string FileName = inTable.index.IndexName[i];
				remove(FileName.c_str());
			}
		}
	}
	

	return true;
}

void API::DropIndex(Table& inTable, int attr)
{
	RecordManager RM(BM);
	//IndexManager IM(inTable, attr, true);
	string FileName = inTable.index.IndexName[attr];
	remove(FileName.c_str());
}

bool Comp(Where& w1, Where& w2)
{
	if (w1.AttributeIndex < w2.AttributeIndex) return true;
	else if (w1.AttributeIndex == w2.AttributeIndex)
	{
		// = 0     <> 1      < 2    <= 3           > 4          >= 5
		if (w1.flag < w2.flag) return true;
	}
	return false;
}