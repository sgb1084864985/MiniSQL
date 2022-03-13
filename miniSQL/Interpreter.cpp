#include "Interpreter.h"
#include "CatalogManager.h"
#include "API.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void Interpreter::GetQS()
{
    QueryString = ""; //Initiallize
    string tmp; //store each line string
    do
    {
        getline(cin,tmp);
        QueryString += " ";
        QueryString += tmp;
    }while(QueryString.at(QueryString.length() - 1) != ';');
    //cout << QueryString << endl;
    
}

void Interpreter::Simplify()
{
    int pos = 0;
    bool flag = false;//if there has a " " between two words
    while(1)
    {
        if(QueryString.at(pos) == ';')
        {
            if(QueryString.at(pos -1) != ' ')
            {
                QueryString.insert(pos, " ");
                pos++;
            }
            QueryString.insert(pos + 1, " ");
            QueryString.erase(pos + 1);
            break;
        }
        if(QueryString.at(pos) == ' ' && !flag)
        {
            flag = true;
            pos++;
            continue;
        }
        if(QueryString.at(pos) == ' ' && flag)
        {
            QueryString.erase(pos,1);
            continue;
        }
        if(QueryString.at(pos) == '>' || QueryString.at(pos) == '<' || QueryString.at(pos) == '='
        || QueryString.at(pos) == '(' || QueryString.at(pos) == ')' || QueryString.at(pos) == '*' || QueryString.at(pos) == ',')
        {
            flag = false;
            if(QueryString.at(pos - 1) != ' ')
            {
                QueryString.insert(pos," ");
                pos++;
            }
            if(QueryString.at(pos + 1) != ' ')
            {
                QueryString.insert(pos + 1, " ");
                pos++;
            }
            else pos++;
            continue;
        }
        flag = false;
        pos++;
    }
    QueryString.erase(0,1);
    
    cout << "Simplify:" << QueryString << endl;
    return ;
}

int Interpreter::Execute()
{
    Simplify();
    if(QueryString.substr(0,6) == "select")
    {
        
        //cout << "执行SELECT语句" << endl;
        ExecuteSELECT();
        cout << "Successful Select!" << endl;
        return 1;
    }
    else if(QueryString.substr(0,4) == "exit")
    {
        cout << "退出系统" << endl;
        return 0;
    }
    else if (QueryString.substr(0, 6) == "create")
    {
        if (QueryString.substr(7, 5) == "table")
        {
            //cout << "执行CREATE TABELE语句" << endl;
            ExecuteCreateTable();
            cout << "Successful Table create!" << endl;
            return 1;
        }
        else if (QueryString.substr(7, 5) == "index")
        {
            //cout << "执行CREATE INDEX语句" << endl;
            ExecuteCreateIndex();
            cout << "Successful Index create!" << endl;
            return 1;
        }
        else
        {
            //cout << "Invalid query format!" << endl;
            throw QueryException("ERROR: Invalid Insert format!");
            return 1;
        }
    }
    else if (QueryString.substr(0, 12) == "insert into ")
    {
        //cout << "执行INSERT语句" << endl;
        ExecuteInsert();
        cout << "Successful Insert!" << endl;
        return 1;
    }
    else if (QueryString.substr(0, 12) == "delete from ")
    {
        //cout << "执行DELETE语句" << endl;
        ExecuteDelete();
        cout << "Successful Delete!" << endl;
        return 1;
    }
    else if (QueryString.substr(0, 4) == "drop")
    {
        if (QueryString.substr(5, 5) == "table")
        {
            //cout << "执行DROP TABLE语句" << endl;
            ExecuteDropTable();
            cout << "Successful Drop Table!" << endl;
            return 1;
        }
        else if (QueryString.substr(5, 5) == "index")
        {
            //cout << "执行DROP INDEX语句" << endl;
            ExecuteDropIndex();
            cout << "Successful Drop Index!" << endl;
            return 1;
        }
        else
        {
            throw QueryException("Invalid Drop Format!");
            return 1;
        }
    }
    else if (QueryString.substr(0, 8) == "execfile")
    {
        ExecuteFile();
        return 1;
    }
    else
    {
        throw QueryException("Invalid query format!");
    }
    return 0;
}

int Interpreter::endOfThis(int pos)//return the location of the space of the word
{
    int pos_end = pos;
    while (QueryString.at(pos_end) != ' ' && pos_end < QueryString.length() - 1)
    {
        pos_end++;
    }
    return pos_end;
}

void Interpreter::ExecuteCreateTable()
{
    API api(buffer);
    std::string tableName;//Table's name
    Attribute attr;//Table's attribute
    short primaryKey = -1;//the loaction of primaryKey
    
    
    attr.num = 0;//initiallize number of attribute
    int pos = 13;//the pos is the location of table name's first letter
    int pos_end = endOfThis(pos);//the pos_end is the location of the space after the table's name
    tableName = QueryString.substr(pos, pos_end - pos);
    pos = pos_end + 1;//now pos is the location of the '( '
    pos_end = endOfThis(pos);//usually now pos_end is the loaction of the space after the '( '
    if (QueryString.substr(pos, pos_end - pos) != "(")
    {
        //cout << "Invalid Create Table format!" << endl;
        //return;
        throw QueryException("Invalid Create Table format!");
    }
    else while (QueryString.substr(pos, pos_end - pos) != ")")
    {
        attr.isUnique[attr.num] = false;//initiallize
        pos = pos_end + 1;
        pos_end = endOfThis(pos);//the attribute name
        if (QueryString.substr(pos, pos_end - pos) == "primary")
        {
            pos = pos_end + 1;
            pos_end = endOfThis(pos);
            if (QueryString.substr(pos, pos_end - pos) == "key")
            {
                pos = pos_end + 1;
                pos_end = endOfThis(pos);
                if (QueryString.substr(pos, pos_end - pos) != "(")
                {
                    //std::cout << "Invalid create format!" << std::endl;
                    //return;
                    throw QueryException("Invalid Create Table format!");
                }
                pos = pos_end + 1;
                pos_end = endOfThis(pos);
                std::string tmp_primary = QueryString.substr(pos, pos_end - pos);
                for (int i = 0; i < attr.num; i++)
                {
                    if (attr.name[i] == tmp_primary)
                    {
                        primaryKey = i;
                        attr.isUnique[i] = true;
                        break;
                    }
                }
                pos = pos_end + 1;//now pos is location of )
                pos_end = endOfThis(pos);
                pos = pos_end + 1;// now pos is ,
                pos_end = endOfThis(pos);
                continue;
            }
            else
            {
                //std::cout << "Invalid create format! Don't use 'primary' as a attribute name!" << std::endl;
                //return;
                throw QueryException("Invalid create format! Don't use 'primary' as a attribute name!");
            }
        }
        attr.name[attr.num] = QueryString.substr(pos, pos_end - pos);
        pos = pos_end + 1;
        pos_end = endOfThis(pos);//the attribute data type
        if (QueryString.substr(pos, pos_end - pos) == "int")
        {
            attr.flag[attr.num] = -1;
            pos = pos_end + 1;//pos is  ','
        }
        else if (QueryString.substr(pos, pos_end - pos) == "float")
        {
            attr.flag[attr.num] = 0;
            pos = pos_end + 1;//pos is ','
        }
        else if (QueryString.substr(pos, pos_end - pos) == "char")
        {
            if (QueryString.at(pos_end + 1) != '(')
            {
                //cout << "Invalid Create Table format!" << endl;
                throw QueryException("Invalid Create Table format!");
            }
            pos = pos_end + 1;//now pos is the location of the '( '
            pos = pos + 2;//now pos is the location of the N
            pos_end = endOfThis(pos);
            int N = 0;
            for (int i = pos; i < pos_end; i++)
            {
                N *= 10;
                N += QueryString.at(i) - '0';
            }
            attr.flag[attr.num] = N;
            pos = pos_end + 1;//pos is the location of ') '
            if (QueryString.at(pos) != ')')
            {
                //cout << "Invalid Create Table format!" << endl;
                //return;
                throw QueryException("Invalid Create Table format!");
            }
            pos = pos + 2;
        }
        if (QueryString.at(pos) != ',' && QueryString.at(pos) != ')')
        {
            pos_end = endOfThis(pos);
            if (QueryString.substr(pos, pos_end - pos) == "unique")
            {
                attr.isUnique[attr.num] = true;
                pos = pos_end + 1;//normally pos is ','
            }
            else if (QueryString.substr(pos, pos_end - pos) == "primary")
            {
                pos = pos_end + 1;
                pos_end = endOfThis(pos);
                if (QueryString.substr(pos, pos_end - pos) == "key")
                {
                    primaryKey = attr.num;
                    attr.isUnique[attr.num] = true;
                    pos = pos_end + 1;//normally pos is ','
                }
            }
        }
        pos_end = endOfThis(pos);
        attr.num++;
    }
    pos = pos_end + 1;
    if (QueryString.at(pos) != ';')
    {
        //cout << "Ivalid Create Table format!" << endl;
        //return;
        throw QueryException("Invalid Create Table format!");
    }
    //Table table(attr, tableName, 0);
    
    //attr.PrintInfo();//used for debug

    //std::cout << "table's name is " << tableName << std::endl;//used for debug

    CatalogManager CM;
    Index tmp;
    if (primaryKey != -1)//if there is a primary key, then build a primary index on it
    {
       tmp.num = 1;
       tmp.location[primaryKey] = 1;//primary index
       tmp.IndexName[primaryKey] = "PrimaryKeyIndex";
    }
    for (int i = 0; i < attr.num; i++)
    {
        if (i != primaryKey && attr.isUnique[i])
        {
            tmp.location[i] = 2;
            tmp.IndexName[i] = "Unique" + i;
        }
    }
    CM.createTable(tableName, attr, primaryKey, tmp, 0);

    /*******************************************************/
    Table T;
    CM.getTable(tableName, T);
    api.CREATETABLE(T);
    /*******************************************************/
    return;
}

void Interpreter::ExecuteInsert()
{
    CatalogManager CM;
    API api(buffer);
    std::string Tname;//Table name
    Attribute attr;//get the attributes from catalog manager(为了便于后面数据的读取)
    Tuper* insertTuper = new Tuper(); 
    
    int pos = 12;//pos is the first letter of the table name
    int pos_end = endOfThis(pos);
    Tname = QueryString.substr(pos, pos_end - pos);
    Table T;
    CM.getTable(Tname, T);
    //CM.getAttribute(Tname, attr);
    attr = T.attr;

    if (attr.num == -1)//No table named Tname exists
    {
        //return;
        throw QueryException("ERROR: Table doesn't exist!");
    }
    pos = pos_end + 1;//normally now pos is the location of the first letter of "values"
    pos_end = endOfThis(pos);
    if (QueryString.substr(pos, pos_end - pos) != "values")
    {
        //std::cout << "Invalid Insert fotmat!" << std::endl;
        //return;
        throw QueryException("ERROR: Invalid Insert format!");
    }
    pos = pos_end + 1;//normally pos is '( '
    if (QueryString.at(pos) != '(')
    {
        //std::cout << "Invalid Insert format!" << std::endl;
        //return;
        throw QueryException("ERROR: Invalid Insert format!");
    }
    pos = pos + 2;//now pos is the first value's name
    pos_end = endOfThis(pos);
    try{
    for (int i = 0; i < attr.num; i++)
    {
        if (QueryString.substr(pos, pos_end - pos) == "null")
        {
            DataNull* data1 = new DataNull();
            //insertTuper->data.push_back(data1);
            insertTuper->addData(data1);
        }
        else if (attr.flag[i] == -1)//int
        {
            int x;// = std::stoi(QueryString.substr(pos, pos_end - pos));
            if (toInt(pos, pos_end, x))
            {
                DataInt* data1 = new DataInt(x);
                //insertTuper->data.push_back(data1);
                insertTuper->addData(data1);
            }
            else
            {
                //cout << "Invalid insert format!" << endl;
                //return;
                throw QueryException("ERROR: Invalid Insert format!");
            }

        }
        else if (attr.flag[i] == 0)//float
        {
            float x;// = std::stof(QueryString.substr(pos, pos_end - pos));
            if (toFloat(pos, pos_end, x))
            {
                DataFloat* data1 = new DataFloat(x);
                //insertTuper->data.push_back(data1);
                insertTuper->addData(data1);
            }
            else
            {
                //cout << "Invalid insert format!" << endl;
                //return;
                throw QueryException("ERROR: Invalid Insert format!");
            }

        }
        else//char(n)
        {
            DataChar* data1 = new DataChar(QueryString.substr(pos + 1, pos_end - pos - 2));//没有去掉''
            //insertTuper->data.push_back(data1);
            insertTuper->addData(data1);
        }
        pos = pos_end + 1;//pos is , or )
        pos_end = endOfThis(pos);
        if (QueryString.substr(pos, pos_end - pos) == ",")
        {
            pos = pos_end + 1;
            pos_end = endOfThis(pos);
        }
    }
    }
        catch(QueryException qe) 
        {
        delete insertTuper;
        throw qe;

        }
    //pos is ')'  pos_end is the space after ')'
    if (QueryString.at(pos) != ')')
    {
        //std::cout << "Invalid Insert format!" << std::endl;
        //return;
        throw QueryException("ERROR: Invalid Insert format!");
    }
    pos = pos_end + 1;
    if (QueryString.at(pos) != ';')
    {
        //std::cout << "Invalid Insert format!" << std::endl;
        //return;
        throw QueryException("ERROR: Invalid Insert format!");
    }
    /***************************************************************/
    //used for debug
    /*
    cout << "下面是要插入的元组：" << endl;
    for (int i = 0; i < attr.num; i++)
    {
        if(attr.flag[i] == -1)  cout << ((DataInt*)insertTuper->data[i])->x << endl;
        else if(attr.flag[i] == 0) cout << ((DataFloat*)insertTuper->data[i])->x << endl;
        else cout << ((DataChar*)insertTuper->data[i])->x << endl;
    }
    */
    /***************************************************************/
    api.INSERT(Tname, *insertTuper);//执行insert操作，可以传入

    //delete insertTuper;
    //return;
}

void Interpreter::ExecuteSELECT()
{
    /*
    select sid, age
    from Stu
    where age = 19 and sid = '123123';
    */
    CatalogManager CM;
    API api(buffer);
    vector<int> selectAttr;//select attribute
    vector<string> selectAttrName;//selected attributes' name 
    int selectAttrNum = 0;//the number of the selected attributes
    std::string Tname;//table name
    std::vector<Where> w;//store where condition(WHERE中的约束条件)

    int pos = 7;
    int pos_end = endOfThis(pos);
    if (QueryString.at(pos) == '*')
    {
        selectAttrNum = -1;//-1 means all
        pos = pos_end + 1;
        pos_end = endOfThis(pos);
    }
    else while (1)
    {
        selectAttrName.push_back(QueryString.substr(pos, pos_end - pos));
        selectAttrNum++;
        pos = pos_end + 1;
        pos_end = endOfThis(pos);
        if (QueryString.at(pos) != ',') break;
        pos = pos + 2;
        pos_end = endOfThis(pos);
    }
    if (QueryString.substr(pos, pos_end - pos) != "from")
    {
        //std::cout << "Invalid Select format!" << std::endl;
        //return;
        throw QueryException("Invalid Select foramt!");
    }
    pos = pos_end + 1;//pos is the location of the table name
    pos_end = endOfThis(pos);
    Tname = QueryString.substr(pos, pos_end - pos);
    Table table;
    CM.getTable(Tname, table);
    Attribute attr = table.attr;
    if (selectAttrNum == -1)
    {
        for (int i = 0; i < table.attr.num; i++)
        {
            selectAttr.push_back(i);
        }
    }
    else for (int i = 0; i < selectAttrNum; i++)
    {
        int j;
        for (j = 0; j < table.attr.num; j++)
        {
            if (selectAttrName[i] == table.attr.name[j])
            {
                selectAttr.push_back(j);
                break;
            }
                
        }
        if (j == table.attr.num)
        {
            //cout << "不存在该属性" << selectAttrName[i] << endl;
            string re = "Not exist the attribute named " + selectAttrName[i];
            throw QueryException(re);
        }
    }
    pos = pos_end + 1;//pos is the location of the first letter in "where"
    pos_end = endOfThis(pos);
    if (QueryString.at(pos) == ';')
    {        
        //**************************************调用API*********************************//
        Table ret = api.SELECT(table, w, selectAttr);
        ret.TableShow();
        return;
    }
    if (QueryString.substr(pos, pos_end - pos) != "where")
    {
        //std::cout << "Invalid Select format!" << std::endl;
        //return;
        throw QueryException("Invalid Select foramt!");
    }
    pos = pos_end + 1;//now pos is the location of the first where attribute
    DealWithWhere(pos, w, table, attr);

    //**************************************调用API*********************************//
    Table ret = api.SELECT(table, w, selectAttr);
    ret.TableShow();
    return;
}

void Interpreter::ExecuteCreateIndex()//没加api
{
    //create index IndexName on TableName(AttrName);
    //             13
    CatalogManager CM;
    API api(buffer);
    Attribute attr;
    Table T;
    std::string IndexName;
    std::string TableName;
    std::string AttrName;
    int Attr_index;//需要建立索引的属性序号
    int pos, pos_end;

    //Get Index Name
    pos = 13;
    pos_end = endOfThis(pos);
    IndexName = QueryString.substr(pos, pos_end - pos);

    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    if (QueryString.substr(pos, pos_end - pos) != "on")
    {
        throw QueryException("Invalid Create Index Format!");
    }
    
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    TableName = QueryString.substr(pos, pos_end - pos);
    CM.getTable(TableName, T);
    //CM.getAttribute(TableName, attr);
    attr = T.attr;
    if (attr.num == -1)
    {
        //std::cout << "No Table Named" << TableName << std::endl;
        //return;//这里后面可以改为抛出异常
        string re = "No Table Named" + TableName;
        throw QueryException(re);
    }
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    AttrName = QueryString.substr(pos, pos_end - pos);
    Attr_index = CM.getAttrIndex(AttrName, attr);
    if (Attr_index == -1)
    {
        //std::cout << "Don't exist the attr in the table!" << endl;
        //return;//后面改为抛出异常
        throw QueryException("Don't exist the attr in the table!");
    }
    T.index.IndexName[Attr_index] = IndexName;
    if (T.index.num > 0) T.index.location[Attr_index] = 2;//如果存在索引了 就建立辅助索引
    else T.index.location[Attr_index] = 1;//如果不存在 建立辅助索引
    T.index.num++;
    CM.UpdateTable(TableName, T);

    //API
    api.CREATEINDEX(T, Attr_index);
    CM.UpdateTable(TableName, T);
    return;
}

void Interpreter::ExecuteDelete()
{
    //delete from TableName;
    //delete from Stu where sid = '3131231';
    CatalogManager CM;
    API api(buffer);
    string TableName;
    vector<Where> W;
    Attribute attr;
    Table T;
    int pos, pos_end;
    pos = 12;
    pos_end = endOfThis(pos);
    TableName = QueryString.substr(pos, pos_end - pos);
    CM.getTable(TableName, T);
    attr = T.attr;
    if (T.attr.num == -1)//该表不存在
    {
        //return;
        string re = "Don't exist the table named" + TableName;
        throw QueryException(re);
    }
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    if (QueryString.at(pos) == ';')
    {
        /********************************************************************/
        //调用API
        api.DELETE(T, W, false);//Delete all;
        return;
        /*******************************************************************/
    }
    else if (QueryString.substr(pos, pos_end - pos) != "where")
    {
        throw QueryException("Invalid Delete Format!");
        //return;
    }
    pos = pos_end + 1;
    DealWithWhere(pos, W, T, attr);
    //调用API
    api.DELETE(T, W, true);//有条件的DELETE
    return;
}

void Interpreter::ExecuteDropTable()
{
    //drop table TableName;
    CatalogManager CM;
    API api(buffer);
    string TableName;
    Table T;
    int pos = 11;
    int pos_end = endOfThis(pos);
    TableName = QueryString.substr(pos, pos_end - pos);
    CM.getTable(TableName, T);
    api.DropTable(T);
    string FileName = "T_" + TableName;
    if (!CM.hasTable(FileName))
    {
        //cout << "Don't exist Table named" << TableName << endl;
        //return;
        string re = "Don't exist Table named" + TableName;
        throw QueryException(re);
    }
    if (remove(FileName.c_str()) == 0)
    {
        cout << "Drop table successful!" << endl;
        return;
    }
    else
    {
        cout << "Drop table failed!" << endl;
        return;
    }
    
    return;
}

void Interpreter::ExecuteDropIndex()
{
    //drop index IndexName on TableName(AttributeName);
    CatalogManager CM;
    API api(buffer);
    string IndexName, TableName, AttrName;
    Table T;
    Attribute attr;
    int Attr_index;
    int pos = 11;
    int pos_end = endOfThis(pos);
    IndexName = QueryString.substr(pos, pos_end - pos);
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    if (QueryString.substr(pos, pos_end - pos) != "on")
    {
        throw QueryException("Invalid Drop Index Format!");
    }
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    TableName = QueryString.substr(pos, pos_end - pos);
    if (!CM.hasTable(TableName))
    {
        throw QueryException("Don't exist Table named " + TableName);
    }
    CM.getTable(TableName, T);
    attr = T.attr;
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    if (QueryString.at(pos) != '(')
    {
        throw QueryException("Invail Drop Table Format!");
    }
    pos = pos_end + 1;
    pos_end = endOfThis(pos);
    AttrName = QueryString.substr(pos, pos_end - pos);
    Attr_index = CM.getAttrIndex(AttrName, attr);
    if (Attr_index == -1)
    {
        throw QueryException("Don't exist attribute named " + AttrName);
    }
    if (T.index.IndexName[Attr_index] != IndexName)
    {
        throw QueryException("属性上的索引不叫你给的这名儿!");
    }
    //API
    api.DropIndex(T, Attr_index);
    T.index.num--;
    T.index.location[Attr_index] = 0;
    T.index.IndexName[Attr_index] = "";
    

    CM.UpdateTable(TableName, T);
    return;
}

void Interpreter::ExecuteFile()
{
    //execfile Name;
    int pos = 9;
    int pos_end = endOfThis(pos);
    string fileName = QueryString.substr(pos, pos_end - pos);
    fstream file(fileName, ios::in);
    if (!file)
    {
        throw QueryException("No such file!");
    }
    QueryString = " ";
    string temp;
    while (file.peek() != EOF)
    {
        file >> temp;
        QueryString += temp;
        QueryString += " ";
        if (temp.at(temp.length() - 1) == ';')
        {
            cout << QueryString << endl;
            Execute();
            QueryString = " ";
        }
    }
    return;
}

void Interpreter::DealWithWhere(int pos, std::vector<Where>& w, Table t, Attribute attr)
{
    int pos_end = endOfThis(pos);
    while (1)
    {
        std::string tmp = QueryString.substr(pos, pos_end - pos);//attribute name
        int index_attr;//attribute index
        OP flag_attr;//{eq, neq, l, leq, g,geq}
        for (int i = 0; i < t.getColumnNum(); i++)
        {
            if (tmp == attr.name[i])
            {
                index_attr = i;
                if (attr.flag[i] == -1)//int
                {
                    pos = pos_end + 1;
                    pos_end = endOfThis(pos);
                    int x;// attribute op x
                    if (QueryString.substr(pos, pos_end - pos) == "=")
                    {
                        flag_attr = eq;//equal
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (toInt(pos, pos_end, x))
                        {
                            DataInt* data = new DataInt(x);
                            Where tmp_where(i, tmp, flag_attr, data);
                            w.push_back(tmp_where);
                        }
                        else
                        {
                            //std::cout << "Invalid Where format!" << std::endl;//后改为抛出异常
                            //return;
                            throw QueryException("Invalid Where format!");
                        }
                    }
                    else if (QueryString.substr(pos, pos_end - pos) == ">")
                    {
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (toInt(pos, pos_end, x))//greater
                        {
                            DataInt* data = new DataInt(x);
                            flag_attr = g;//greater
                            Where tmp_where(i, tmp, flag_attr, data);
                            w.push_back(tmp_where);
                        }
                        else
                        {
                            if (QueryString.substr(pos, pos_end - pos) == "=")
                            {
                                pos = pos_end + 1;
                                pos_end = endOfThis(pos);
                                if (toInt(pos, pos_end, x))//greater or equal
                                {
                                    DataInt* data = new DataInt(x);
                                    flag_attr = geq;
                                    Where tmp_where(index_attr, tmp, flag_attr, data);
                                    w.push_back(tmp_where);
                                }
                                else
                                {
                                    //std::cout << "Invalid where format!" << std::endl;
                                    //return;
                                    throw QueryException("Invalid Where format!");
                                }
                            }
                        }
                    }
                    else if (QueryString.substr(pos, pos_end - pos) == "<")
                    {
                        if (QueryString.at(pos_end + 1) == '>')
                        {
                            pos = pos_end + 1;//pos is the location of '>'
                            pos_end = endOfThis(pos);
                            pos = pos_end + 1;
                            pos_end = endOfThis(pos);
                            if (toInt(pos, pos_end, x))//not equal
                            {
                                DataInt* data = new DataInt(x);
                                flag_attr = neq;
                                Where tmp_where(i, tmp, flag_attr, data);
                                w.push_back(tmp_where);
                            }
                            else
                            {
                                //std::cout << "Invalid Where format!" << std::endl;
                                //return;
                                throw QueryException("Invalid Where format!");
                            }
                        }
                        else if (QueryString.at(pos_end + 1) == '=')
                        {
                            pos = pos_end + 1;
                            pos_end = endOfThis(pos);
                            pos = pos_end + 1;
                            pos_end = endOfThis(pos);
                            if (toInt(pos, pos_end, x))//less or equal
                            {
                                DataInt* data = new DataInt(x);
                                flag_attr = leq;
                                Where tmp_where(index_attr, tmp, flag_attr, data);
                                w.push_back(tmp_where);
                            }
                        }
                        else
                        {
                            pos = pos_end + 1;
                            pos_end = endOfThis(pos);                      
                            if (toInt(pos, pos_end, x))
                            {
                                DataInt* data = new DataInt(x);
                                flag_attr = l;
                                Where tmp_where(i, tmp, flag_attr, data);
                                w.push_back(tmp_where);
                            }
                            else
                            {
                                //std::cout << "Invalid Where format!" << std::endl;
                                //return;
                                throw QueryException("Invalid Where format!");
                            }
                        }
                
                    }
                }
                else if (attr.flag[i] == 0)//is float
                {
                    pos = pos_end + 1;
                    pos_end = endOfThis(pos);
                    float x;
                    if (QueryString.substr(pos, pos_end - pos) == "=")
                    {
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (toFloat(pos, pos_end, x))//equal
                        {
                            DataFloat* data = new DataFloat(x);
                            flag_attr = eq;
                            Where tmp_where(index_attr, tmp, flag_attr, data);
                            w.push_back(tmp_where);
                        }
                        else
                        {
                            //std::cout << "Invalid Where format!" << std::endl;
                            //return;
                            throw QueryException("Invalid Where format!");
                        }
                    }
                    else if (QueryString.substr(pos, pos_end - pos) == ">")
                    {
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (toFloat(pos, pos_end, x))//大于 greater
                        {
                            DataFloat* data = new DataFloat(x);
                            flag_attr = g;
                            Where tmp_where(index_attr, tmp, flag_attr, data);
                            w.push_back(tmp_where);
                        }
                        else if (QueryString.substr(pos, pos_end - pos) == "=")
                        {
                            pos = pos_end + 1;
                            pos_end = endOfThis(pos);
                            if (toFloat(pos, pos_end, x))//大于等于 greater or equal
                            {
                                DataFloat* data = new DataFloat(x);
                                flag_attr = geq;
                                Where tmp_where(index_attr, tmp, flag_attr, data);
                                w.push_back(tmp_where);
                            }
                            else
                            {
                                //std::cout << "Invalid Where format!" << std::endl;
                                //return;
                                throw QueryException("Invalid Where format!");
                            }
                        }
                    }
                    else if (QueryString.substr(pos, pos_end - 1) == "<")
                    {
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (toFloat(pos, pos_end, x))//小于 less
                        {
                            DataFloat* data = new DataFloat(x);
                            flag_attr = l;
                            Where tmp_where(index_attr, tmp, flag_attr, data);
                            w.push_back(tmp_where);
                        }
                        else
                        {
                            if (QueryString.substr(pos, pos_end - pos) == "=")
                            {
                                pos = pos_end + 1;
                                pos_end = endOfThis(pos);
                                if (toFloat(pos, pos_end, x))
                                {
                                    DataFloat* data = new DataFloat(x);
                                    flag_attr = leq;
                                    Where tmp_where(index_attr, tmp, flag_attr, data);
                                    w.push_back(tmp_where);
                                }
                                else
                                {
                                    //std::cout << "Invalid where format!" << std::endl;
                                    //return;
                                    throw QueryException("Invalid Where format!");
                                }
                            }
                            else if (QueryString.substr(pos, pos_end - pos) == ">")
                            {
                                pos = pos_end + 1;
                                pos_end = endOfThis(pos);
                                if (toFloat(pos, pos_end, x))//不等于 not equal
                                {
                                    DataFloat* data = new DataFloat(x);
                                    flag_attr = neq;
                                    Where tmp_where(index_attr, tmp, flag_attr, data);
                                    w.push_back(tmp_where);
                                }
                                else
                                {
                                    //std::cout << "Invalid where format!" << std::endl;
                                    //return;
                                    throw QueryException("Invalid Where format!");
                                }
                            }
                            else
                            {
                                //std::cout << "what are you doing???" << std::endl;
                                throw QueryException("what are you doing???");
                            }
                        }
                    }
                }
                else//char(n)
                {
                    pos = pos_end + 1;
                    pos_end = endOfThis(pos);
                    std::string x;
                    if (QueryString.substr(pos, pos_end - pos) == "=")
                    {
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (QueryString.at(pos) == '\'' && QueryString.at(pos_end - 1) == '\'')
                        {
                            x = QueryString.substr(pos + 1, pos_end - pos - 2);
                            flag_attr = eq;
                            DataChar* data = new DataChar(x);
                            Where tmp_where(index_attr, tmp, flag_attr, data);
                            w.push_back(tmp_where);
                        }
                        else
                        {
                            //std::cout << "string need ' '! Error format!" << std::endl;
                            //return;
                            throw QueryException("string need ' '! Error format!");
                        }
                    }
                    else if (QueryString.substr(pos, pos_end - pos) == "<")
                    {
                        pos = pos_end + 1;
                        pos_end = endOfThis(pos);
                        if (QueryString.substr(pos, pos_end - pos) == ">")
                        {
                            pos = pos_end + 1;
                            pos_end = endOfThis(pos);
                            if (QueryString.at(pos) == '\'' && QueryString.at(pos_end - 1) == '\'')
                            {
                                x = QueryString.substr(pos + 1, pos_end - pos - 2);
                                flag_attr = neq;
                                DataChar* data = new DataChar(x);
                                Where tmp_where(index_attr, tmp, flag_attr, data);
                                w.push_back(tmp_where);
                            }
                            else
                            {
                                //std::cout << "string need ' '! Error format!" << std::endl;
                                //return;
                                throw QueryException("string need ' '! Error format!");
                            }
                        }
                    }
                    else
                    {
                        //std::cout << "Invalid where format!" << endl;
                        //return;
                        throw QueryException("Invalid where format!");
                    }
                }
                break;
            }
        }
        pos = pos_end + 1;//the location of "and" or ";"
        pos_end = endOfThis(pos);
        if (QueryString.at(pos) == ';')
        {
            break;
        }
        else if (QueryString.substr(pos, pos_end - pos) == "and")
        {
            pos = pos_end + 1;
            pos_end = endOfThis(pos);
        }
    }

    /**************************************************************************/
    //used for debug
    /*
    int size = w.size();
    cout << "有" << size << "个where条件" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << w[i].AttributeName;
        switch (w[i].flag)
        {
        case eq:
            cout << "=";
            break;
        case neq:
            cout << "<>";
            break;
        case l:
            cout << "<";
            break;
        case leq:
            cout << "<=";
            break;
        case g:
            cout << ">";
            break;
        case geq:
            cout << ">=";
            break;
        default:
            break;
        }
        switch (attr.flag[w[i].AttributeIndex])
        {
        case -1:
            cout << ((DataInt*)w[i].d)->x << endl;
            break;
        case 0:
            cout << ((DataFloat*)w[i].d)->x << endl;
            break;
        default:
            cout << ((DataChar*)w[i].d)->x << endl;
            break;
        }
        
        
    }*/
    /**************************************************************************/
    return;
}

bool Interpreter::toInt(int pos, int pos_end, int& res)
{
    res = 0;
    for (int i = pos; i < pos_end; i++)
    {
        if (QueryString.at(i) >= '0' && QueryString.at(i) <= '9')
        {
            res *= 10;
            res += QueryString.at(i) - '0';
        }
        else return false;
    }
    return true;
}

bool Interpreter::toFloat(int pos, int pos_end, float& res)
{
    res = 0;
    int flag = 0;//小数点前flag=0 小数点后flag=1
    float mask = 0.1;
    for (int i = pos; i < pos_end; i++)
    {
        if (QueryString.at(i) >= '0' && QueryString.at(i) <= '9' && flag == 0)//小数点之前
        {
            res *= 10;
            res += QueryString.at(i) - '0';
        }
        else if (QueryString.at(i) == '.')
        {
            flag = 1;
        }
        else if (QueryString.at(i) > '0' && QueryString.at(i) < '9' && flag == 1)//小数点之后
        {
            float tmp = (QueryString.at(i) - '0') * mask;
            mask *= 0.1;
            res += tmp;
        }
        else return false;
    }
    return true;
}
