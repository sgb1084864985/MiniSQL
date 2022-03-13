#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include "base.h"
#include <iostream>
#include <string>
#include <vector>
#include "BufferManager.h"
#include "API.h"
extern BufferManager bf;


class Interpreter
{
    private:
        std::string QueryString;//query string
        
    public:
        Interpreter(){};
        void GetQS(); //get input query sentence
        void Simplify(); //format the query statements uniformly(统一语句格式)
        int Execute(); //execute the sentence totally(执行语句总函数)
        void ExecuteSELECT();//SELECT statement
        void ExecuteCreateTable(); //CREATE table
        void ExecuteCreateIndex(); //CREATE index
        void ExecuteInsert(); //INSERT  statement
        void ExecuteDelete();//DELETE statement
        void ExecuteDropTable();//DROP table
        void ExecuteDropIndex();//DROP index
        void ExecuteFile();//运行文件
        int endOfThis(int pos); //return the location of the space of the word
        void DealWithWhere(int pos, std::vector<Where>& w, Table t, Attribute attr);
        bool toInt(int pos, int pos_end, int& res);
        bool toFloat(int pos, int pos_end, float& res);
        
        BufferManager buffer;
};

class QueryException : std::exception {
public:
    QueryException(std::string s) :text(s) {}
    std::string what() {
        return text;
    };
private:
    std::string text;
};

#endif