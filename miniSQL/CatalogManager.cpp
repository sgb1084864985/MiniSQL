#include "CatalogManager.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

bool CatalogManager::hasTable(std::string fileName)
{
	std::ifstream in(fileName);
	if (!in) return false;
	else
	{
		in.close();
		return true;
	}
}

void CatalogManager::createTable(std::string Tname, Attribute attr, short primary, Index index, int blockNumber)
{
	/*create table stu(
		sid char(10),
		name char(10),
		age int,
		account float,
		primary key(sid));*/
	std::string fileName = "T_" + Tname;
	if (hasTable(fileName))//the table name already exists
	{
		//std::cout << "There already has a table named " << Tname << " existed!" << std::endl;
		//return;
		string re = "There already has a table named " + Tname + " existed!";
		throw TableException(re);
	}
	
	std::fstream out(fileName, ios::out | ios::binary);
	//out.write((char*)&Tname, sizeof(Tname));//����
	out.write((char*)&attr.num, sizeof(int));//���Ը���
	out.write((char*)&index.num, sizeof(int));//��������
	out.write((char*)&primary, sizeof(short));//����λ��

	out.write((char*)&blockNumber, sizeof(int));//blocknum

	//out.write((char*)attr.name, sizeof(attr.name));//������
	for (int i = 0; i < attr.num; i++)
	{
		char* tmp = new char[attr.name[i].length() + 1];
		memcpy(tmp, attr.name[i].data(), sizeof(char) * attr.name[i].length());
		tmp[attr.name[i].length()] = '\0';
		//out.write(tmp, sizeof(tmp + 1)); 
		out << tmp;
		out << '\0';
	}
		//out.write((char*)&attr.name[i], sizeof(char) * attr.name[i].length());
	out.write((char*)attr.flag, sizeof(attr.flag));//��������
	out.write((char*)attr.isUnique, sizeof(attr.isUnique));//�����Ƿ�Ψһ
	
	//out.write((char*)index.IndexName, sizeof(index.IndexName));//��������
	for (int i = 0; i < 32; i++)
	{
		char* tmp = new char[index.IndexName[i].length() + 1];
		memcpy(tmp, index.IndexName[i].data(), sizeof(char) * index.IndexName[i].length());
		tmp[index.IndexName[i].length()] = '\0';
		//out.write(tmp, sizeof(tmp + 1)); 
		out << tmp;
		out << '\0';
	}
	out.write((char*)index.location, sizeof(index.location));//����λ��
	out.close();
}

void CatalogManager::getAttribute(std::string Tname, Attribute& attr)
{
	//insert into stu values('313','ww',12,12.12);
	std::string fileName = "T_" + Tname;
	if (!hasTable(fileName))
	{
		std::cout << "No such table named " << Tname << " existed!" << std::endl;
		attr.num = -1;
		return;
	}
	//int AttrNum;
	//string Name[32];
	//short Flag[32];
	//bool Unique[32];
	char tmp[50];
	fstream in(fileName, ios::in | ios::binary);
	in.read((char*)&attr.num, sizeof(int));
	in.seekg(sizeof(int), ios::cur);
	in.seekg(sizeof(short), ios::cur);
	in.seekg(sizeof(int), ios::cur);
	//in.seekg(sizeof(int) * 2 + sizeof(short), ios::beg);
	for (int i = 0; i < attr.num; i++)
	{
		//in.getline(tmp, 50);
		//in >> tmp;
		in.getline(tmp, 50, '\0');
		string WDNMD = tmp;
		//in.seekg(WDNMD.length() * sizeof(char), ios::cur);
		attr.name[i] = tmp;
	}
		
		
	
	
	in.read((char*)attr.flag, sizeof(attr.flag));
	in.read((char*)attr.isUnique, sizeof(attr.isUnique));
	in.close();
	
	/**************************************************************************/
	/*
	//use for debug
	cout << "���Ը���: " << attr.num << endl;
	for (int i = 0; i < attr.num; i++)
	{
		cout << "����" << i + 1 << "�������� " << attr.name[i] << endl;
		if (attr.flag[i] == -1) cout << "���������� INT" << endl;
		else if (attr.flag[i] == 0) cout << "���������� FLOAT" << endl;
		else cout << "���������� CHAR( " << attr.flag[i] << " )" << endl;
	}
	*/
	/**************************************************************************/
	return;
}

void CatalogManager::getTable(std::string Tname, Table& T)
{
	std::string fileName = "T_" + Tname;
	T.Tname = Tname;
	if (!hasTable(fileName))
	{
		std::cout << "No such table named " << Tname << " existed!" << std::endl;
		T.attr.num = -1;
		return;
	}
	//int AttrNum;
	//string Name[32];
	//short Flag[32];
	//bool Unique[32];
	char tmp[50];
	fstream in(fileName, ios::in | ios::binary);
	in.seekg(0, ios::beg);
	in.read((char*)&T.attr.num, sizeof(int));
	in.read((char*)&T.index.num, sizeof(int));
	in.read((char*)&T.primary, sizeof(short));

	in.read((char*)&T.blockNum, sizeof(int));

	for (int i = 0; i < T.attr.num; i++)
	{
		//in.getline(tmp, 50);
		//in >> tmp;
		in.getline(tmp, 50, '\0');
		string WDNMD = tmp;
		T.attr.name[i] = tmp;
	}
	in.read((char*)T.attr.flag, sizeof(T.attr.flag));
	in.read((char*)T.attr.isUnique, sizeof(T.attr.isUnique));
	for (int i = 0; i < 32; i++)
	{
		in.getline(tmp, 50, '\0');
		string WDNMD = tmp;
		//in.seekg(WDNMD.length() * sizeof(char), ios::cur);
		T.index.IndexName[i] = tmp;
	}
	in.read((char*)T.index.location, sizeof(T.index.location));
	in.close();
	
	/**************************************************************************/
	/*
	//used for debug
	cout << "���Ը���: " << T.attr.num << endl;
	for (int i = 0; i < T.attr.num; i++)
	{
		cout << "����" << i + 1 << "�������� " << T.attr.name[i] << endl;
		if (T.attr.flag[i] == -1) cout << "���������� INT" << endl;
		else if (T.attr.flag[i] == 0) cout << "���������� FLOAT" << endl;
		else cout << "���������� CHAR( " << T.attr.flag[i] << " )" << endl;
	}
	for (int i = 0; i < T.attr.num; i++)
	{
		switch (T.index.location[i])
		{
		case 1:
			std::cout << "����" << T.attr.name[i] << "���оۼ�����, ";
			std::cout << "��������" << T.index.IndexName[i] << std::endl;
			break;
		case 2:
			std::cout << "����" << T.attr.name[i] << "���и�������, ";
			std::cout << "��������" << T.index.IndexName[i] << std::endl;
			break;
		default:
			break;
		}
	}
	*/
	/**************************************************************************/
	return;
}

int CatalogManager::getAttrIndex(std::string AttrName, Attribute attr)
{
	for (int i = 0; i < attr.num; i++)
	{
		if (attr.name[i] == AttrName)
		{
			return i;
		}
	}
	return -1;
}

void CatalogManager::UpdateTable(std::string Tname, Table inTable)
{
	if (!hasTable("T_" + Tname))
	{
		//����ɸ�Ϊ�׳��쳣
		//std::cout << "Don't exist the table!" << std::endl;
		//return;
		string re = "Don't exist the table" + Tname;
		throw TableException(re);
	}
	std::string FileName = "T_" + Tname;
	if (remove(FileName.c_str()) == 0)
	{
		std::cout << "Delete successful!" << std::endl;
		createTable(Tname, inTable.attr, inTable.primary, inTable.index, inTable.blockNum);
	}
	else
	{
		std::cout << "Delete failed!" << std::endl;
	}
	return;
}

void CatalogManager::UpdateTableBlockNum(Table& T)
{
	std::string fileName = "T_" + T.Tname;
	if (!hasTable(fileName))
	{
		std::cout << "No such table named " << T.Tname << " existed!" << std::endl;
		return;
	}
	fstream out(fileName, ios::out | ios::binary);
	out.write((char*)&T.attr.num, sizeof(int));//���Ը���
	out.write((char*)&T.index.num, sizeof(int));//��������
	out.write((char*)&T.primary, sizeof(short));//����λ��
	out.write((char*)&T.blockNum, sizeof(int));//blocknum
	for (int i = 0; i < T.attr.num; i++)
	{
		char* tmp = new char[T.attr.name[i].length() + 1];
		memcpy(tmp, T.attr.name[i].data(), sizeof(char) * T.attr.name[i].length());
		tmp[T.attr.name[i].length()] = '\0';
		//out.write(tmp, sizeof(tmp + 1)); 
		out << tmp;
		out << '\0';
	}
	//out.write((char*)&attr.name[i], sizeof(char) * attr.name[i].length());
	out.write((char*)T.attr.flag, sizeof(T.attr.flag));//��������
	out.write((char*)T.attr.isUnique, sizeof(T.attr.isUnique));//�����Ƿ�Ψһ

	//out.write((char*)index.IndexName, sizeof(index.IndexName));//��������
	for (int i = 0; i < 32; i++)
	{
		char* tmp = new char[T.index.IndexName[i].length() + 1];
		memcpy(tmp, T.index.IndexName[i].data(), sizeof(char) * T.index.IndexName[i].length());
		tmp[T.index.IndexName[i].length()] = '\0';
		//out.write(tmp, sizeof(tmp + 1)); 
		out << tmp;
		out << '\0';
	}
	out.write((char*)T.index.location, sizeof(T.index.location));//����λ��
	out.close();
	return;
}