#ifndef _BASE_H_
#define _BASE_H_

#include<iostream>
#include<vector>


using namespace std;

class Data
{
public:
	static const int STRING_MAX_SIZE = 255;

	short flag;//-1 is int; 0 is float; 1 ~ 255 is char(n)
	virtual ~Data() {};

	virtual int size()const=0;//里面存的变量数据的大小
	virtual bool operator< (Data& d)const=0;
	virtual bool operator==(Data& d)const=0;
	/*直接比较变量*/
	virtual bool operator<(const void*p)const = 0;
	virtual bool operator==(const void*p)const = 0;
	virtual void DatatoMem(void* dst)const=0;
	virtual void MemtoData(const void* src)=0;

	Data* new_copy()const;
	static int getSize(int flag){
		if (flag == -1) return sizeof(int);
		else if (flag == 0) return sizeof(float);
		else return sizeof(char) * STRING_MAX_SIZE;
	}
};
class DataInt : public Data
{
public:
	int x;
	DataInt(int X) :x(X) { flag = -1; }
	virtual ~DataInt() {}

	int size()const {
		return sizeof(x);
	}
	bool operator<(Data& d)const {
		const DataInt& d_ = dynamic_cast<DataInt&>(d);
		return x < d_.x;
	}
	bool operator==(Data& d)const {
		const DataInt& d_ = dynamic_cast<DataInt&>(d);
		return x == d_.x;
	}
	bool operator<(const void* p) const {
		int k;
		memcpy(&k, p, sizeof(int));
		return x < k;
	}
	bool operator==(const void* p) const {
		int k;
		memcpy(&k, p, sizeof(int));
		return x == k;
	}
	void DatatoMem(void* dst)const {
		memcpy(dst, &x, sizeof(x));
	}
	void MemtoData(const void* dst) {
		memcpy((void*)&x, dst, sizeof(x));
	}
};
class DataFloat : public Data
{
public:
	float x;
	DataFloat(float X) :x(X) { flag = 0; };
	virtual ~DataFloat() {};

	int size()const {
		return sizeof(x);
	}
	bool operator<(Data& d)const {
		const DataFloat& d_ = dynamic_cast<DataFloat&>(d);
		return x < d_.x;
	}
	bool operator==(Data& d)const {
		const DataFloat& d_ = dynamic_cast<DataFloat&>(d);
		return x == d_.x;
	}
	bool operator<(const void* p) const {
		float k;
		memcpy(&k, p, sizeof(float));
		return x < k;
	}
	bool operator==(const void* p) const {
		float k;
		memcpy(&k, p, sizeof(float));
		return x == k;
	}
	void DatatoMem(void* dst)const {
		memcpy(dst, &x, sizeof(x));
	}
	void MemtoData(const void* dst) {
		memcpy((void*)&x, dst, sizeof(x));
	}
};
class DataChar : public Data
{
private:
public:
	std::string x;
	DataChar(std::string X) :x(X)
	{
		flag = X.length();
		if (!flag) flag = 1;
	};
	virtual ~DataChar() {};

	int size()const {
		//return flag;
		return STRING_MAX_SIZE;
	}
	bool operator<(Data& d)const {
		const DataChar& d_ = dynamic_cast<DataChar&>(d);
		return x < d_.x;
	}
	bool operator==(Data& d)const {
		const DataChar& d_ = dynamic_cast<DataChar&>(d);
		return x == d_.x;
	}
	bool operator<(const void* p) const {
		char k[STRING_MAX_SIZE + 1];
		//memcpy(&k, p, STRING_MAX_SIZE * sizeof(char));
		//k[STRING_MAX_SIZE] = '\0';
		strcpy(k, (char*)p);
		return x < k;
	}
	bool operator==(const void* p) const {
		char k[STRING_MAX_SIZE + 1];
		//memcpy(&k, p, STRING_MAX_SIZE * sizeof(char));
		//k[STRING_MAX_SIZE] = '\0';
		strcpy(k, (char*)p);
		return x == k;
	}
	void DatatoMem(void* dst)const {
		strcpy((char*)dst, x.c_str());
	}
	void MemtoData(const void* dst) {
		char s[STRING_MAX_SIZE + 1];
		//memcpy(s, dst, flag*sizeof(char));
		//memcpy(s, dst, STRING_MAX_SIZE * sizeof(char));
		strcpy(s, (char*)dst);

		//s[STRING_MAX_SIZE] = '\0';
		x.assign(s);
	}
};

class DataNull : public Data
{
public:
	bool Null;
	DataNull(bool flag = true) :Null(flag) {};
	virtual ~DataNull() {};

	int size()const { return 0; }//里面存的变量数据的大小
	bool operator< (Data& d)const { return false; }
	bool operator==(Data& d)const { return false; }
	bool operator<(const void* p)const { return false; }
	bool operator==(const void* p)const { return false; }
	void DatatoMem(void* dst)const {}
	void MemtoData(const void* src){}
};

struct Attribute
{
	short flag[32];//at most 32 attributes. records each attribute date type
	std::string name[32];//each attribute's name
	bool isUnique[32];//each attribute is unique or not
	int num;//record the total number of attributes  (if num = -1, the attr is invalid(无效的))

	void PrintInfo()//used for debug
	{
		std::cout << "The number of the attribute is " << num << "." << std::endl;
		for (int i = 0; i < num; i++)
		{
			std::cout << "The name of " << i << "th attribute is " << name[i] << ",";
			if (isUnique[i]) std::cout << "and the attribute is Unique, ";
			std::cout << "the date type is ";
			if (flag[i] == -1) std::cout << "int.";
			else if (flag[i] == 0) std::cout << "float.";
			else std::cout << "char(" << flag[i] << ").";
			std::cout << std::endl;
		}
		return;
	}
};

class Tuper
{
public:
	std::vector<Data*> data;//store values of each attribute
	Tuper() {};
	Tuper(const Tuper& secondTuper);
	int length() { return data.size(); }//return the number of values
	void addData(Data* p) { data.push_back(p); }//add data
	Data* operator[](unsigned short i);
};

struct Index
{
	int num;//the total number of index in the table
	short location[32];//the location of the index
	std::string IndexName[32];//each index's name
	Index()
	{
		num = 0;
		for (int i = 0; i < 32; i++)
		{
			location[i] = 0;
			IndexName[i] = "";
		}
	}
};

class Table
{
public:
	Attribute attr;//attributes
	Index index;//indexs
	std::vector<Tuper*> data;//tupers
	short primary;//record the location primary key; -1 means no primary key
	int blockNum;//number of blocks occupied in data file
	std::string Tname;//table's name

	Table() {};
	Table(Attribute attr, std::string Tname, int blockNum)
	{
		this->attr = attr;
		this->Tname = Tname;
		this->blockNum = blockNum;
	};
	Table(const Table& secondTable);

	int dataSize() { //size of a single tuper;
		int res = 0;
		for (int i = 0; i < attr.num; i++) {
			switch (attr.flag[i]) {
			case -1:res += sizeof(int); break;
			case 0:res += sizeof(float); break;
			default:res += attr.flag[i] + 1; break; //多一位储存'\0' 注意不要改掉了！
				//注意不要改掉了！
			}
		}
		return res;
	}

	int getRowNum() const { return data.size(); }
	int getColumnNum() const { return attr.num; }
	std::string getName() { return Tname; }
	int getBlockNum() { return blockNum; }
	Attribute getAttribute() { return attr; }
	void addData(Tuper* t);
	void TableShow()
	{
		for (int i = 0; i < attr.num; i++)
		{
			cout << attr.name[i] << " ";
		}
		cout << endl;
		int value1;
		float value2;
		string value3;
		for (int i = 0; i < data.size(); i++)
		{
			for (int j = 0; j < attr.num; j++)
			{
				switch (attr.flag[j])
				{
				case -1:
					value1 = ((DataInt*)(data[i]->data)[j])->x;
					cout << value1 << " ";
					break;
				case 0:
					value2 = ((DataFloat*)(data[i]->data)[j])->x;
					cout << value2 << " ";
					break;
				default:
					value3 = ((DataChar*)(data[i]->data)[j])->x;
					cout << value3 << " ";
					break;
				}
			}
			cout << endl;
		}
	}
};
//equal, not equal, less, less or equal, greater, greater or equal
// = 0     <> 1      < 2    <= 3           > 4          >= 5
typedef enum { eq, neq, l, leq, g, geq } OP;
struct Where
{
	int AttributeIndex;
	std::string AttributeName;
	OP flag;
	Data* d;
	Where() {};
	Where(int AttributeIndex, std::string AttributeName, OP flag, Data* d)
	{
		this->AttributeIndex = AttributeIndex;
		this->AttributeName = AttributeName;
		this->flag = flag;
		this->d = d;
	}
};

class TableException : public std::exception {
public:
	TableException(std::string s) :text(s) {}
	std::string what() {
		return text;
	};
private:
	std::string text;
};


#endif
