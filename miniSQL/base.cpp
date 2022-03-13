#include"base.h"
#include<iostream>
#include<vector>
#include<cstring>
//g++ base.cpp BptNode.cpp BufferManager.cpp IndexException.cpp IndexFileHead.cpp IndexManager.cpp main.cpp
//virtual int size()const;//里面存的变量数据的大小
//virtual bool operator< (const Data& d)const;
//virtual bool operator==(const Data& d)const;
//virtual void DatatoMem(void* dst)const;
//virtual void MemtoData(const void* src);

Data* Data::new_copy() const{
	Data* rslt = nullptr;
	const Data* p = const_cast<Data*>(this);
	if (flag <0) {
		DataInt* i;
		memcpy(&i, &p, sizeof(void*));
		rslt = new DataInt(i->x);
	}
	else if (flag == 0) {
		DataFloat* i;
		memcpy(&i, &p, sizeof(void*));
		rslt = new DataFloat(i->x);
	}
	else if (flag > 0) {
		DataChar* i;
		memcpy(&i, &p, sizeof(void*));
		rslt = new DataChar(i->x);
	}
	return rslt;
}






/////////////////////////////////
Tuper::Tuper(const Tuper& secondTuper)
{
	int size = secondTuper.data.size();
	for (int i = 0; i < size; i++)
	{
		Data* tmp;
		switch (secondTuper.data[i]->flag)
		{
		case -1:
			tmp = new DataInt(((DataInt*)secondTuper.data[i])->x);
			this->data.push_back(tmp);
			break;
		case 0:
			tmp = new DataFloat(((DataFloat*)secondTuper.data[i])->x);
			this->data.push_back(tmp);
			break;
		default:
			tmp = new DataChar(((DataChar*)secondTuper.data[i])->x);
			this->data.push_back(tmp);
			break;
		}

	}
}

Table::Table(const Table& secondTable)
{
	this->attr = secondTable.attr;
	this->blockNum = secondTable.blockNum;
	this->index = secondTable.index;
	this->primary = secondTable.primary;
	this->Tname = secondTable.Tname;
	for (int i = 0; i < secondTable.getRowNum(); i++)
	{
		Tuper* t = new Tuper(*secondTable.data[i]);
		this->data.push_back(t);
	}
}

Data* Tuper::operator[](unsigned short i) {
	/*if (i >= data.size())
		throw std::out_of_range("out of range in t[i]");*/
	return data[i];
}

void Table::addData(Tuper* t) {
	/*if (t->length() != attr.num)
		throw TableException("Illegal Tuper Inserted: unequal column size!");*/
	int i, j;
	for (i = 0; i < attr.num; i++) {
		j = (*t)[i]->flag;
	}
	data.push_back(t);
}