#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_
#include <vector>
#include <iostream>
#include "base.h"

class CatalogManager
{
public:
	CatalogManager() {};
	~CatalogManager() {};
	bool hasTable(std::string Tname);//if there is  a table named Tname return true, else return false
	void createTable(std::string Tname, Attribute attr, short primary, Index index, int blockNumber);//create a table
	//����Ľӿڿ��Ա����ģ�����
	//�õ����������Ϣ�������������Ϣ���ڴ���ĵڶ��������
	void getAttribute(std::string Tname, Attribute& attr);//return the table's attributes
	//�õ����������Ϣ��������Ϣ,�������Ϣ���ڴ���ĵڶ��������ע����������ģʽ��Ϣ��
	void getTable(std::string Tname, Table& T);//return the table named Tname
	//�õ������ڱ��е���ţ���������ڷ���-1.
	int getAttrIndex(std::string AttrName, Attribute attr);//get the attribute index
	void UpdateTable(std::string Tname, Table inTable);
	void UpdateTableBlockNum(Table& T);
};



#endif
