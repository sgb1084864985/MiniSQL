
#include"IndexManager.h"


void IndexManager::Establish(string file)
{
	index i(file);
}

void IndexManager::Insert(string file, Data* key, int Addr)
{
	index i(file);
	if (i.Number == 0)
	{
		int ktype;
		if (key->flag == -1)
			ktype = 0;
		else if (key->flag == 0)
			ktype = 1;
		else ktype = 2;
		i.initialize(key, Addr, ktype);
	}
	else
	{
		i.insert(key, Addr);
	}
}

void IndexManager::Delete(string file, Data* key)
{
	index i(file);
	if (i.Number == 0)
	{
		throw TableException("The index is empty!");
	}
	else
	{
		i.Delete(key);
	}
}

int IndexManager::Find(string file, Data* key)
{
	index i(file);
	int re;
	if (i.Number == 0)
	{
		return -1;//The index is empty, and return a "NULL" value! 
		//throw TableException("The index is empty!");
	}
	else
	{
		re = i.find(key);
	}
	return re;
}

void IndexManager::Drop(string file)
{
	remove(file.c_str());
}

int* IndexManager::Range(string file, Data*key1, Data*key2)
{
	index i(file);
	int *re;
	if (i.Number == 0)
	{
		return NULL;//The index is empty, and return a "NULL" value! 
		//throw TableException("The index is empty!");
	}
	else
	{
		re = i.Range(key1,key2);
	}
	return re;
}
