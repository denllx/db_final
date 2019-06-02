#include<iostream>
#include<sstream>
#include<string>
#include<cctype>
#include "SQL.h"
#include "Reader.h"
using namespace std;

int main(){
    SQL sql;
    Reader reader;
    shared_ptr<Instruction> inst;
    while(inst=reader.read()){
        inst->exec(sql);
    }
    return 0;
}