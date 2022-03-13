//
//  main.cpp
//  MiniSQL
//
//  Created by runganghan on 16/5/28.
//  Copyright © 2016年 hrg. All rights reserved.
//

#include <iostream>
#include <string>
#include "Interpreter.h"
#include "API.h"
#include "BufferManager.h"
#include "RecordManager.h"
using namespace std;

BufferManager bf;
extern int Number;
int main() {
    int re = 1;
    InterManager itp;
    cout << "Welcome to MiniSQL!" << endl;
    //itp.GetQs();
    //re = itp.EXEC();
    while(re){
        try{
            cout << ">>>";
            itp.GetQs();
            re = itp.EXEC();
        }
        catch(TableException te){
            cout << te.what() << endl;
        }
        catch(QueryException qe){
            cout << qe.what() << endl;
        }
    }
    return 0;
}
