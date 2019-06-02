#pragma once
#include <string>
#include <memory>
#include <sstream>
#include "SQL.h"
#include "DataBase.h"
#include "Table.h"
#include "Record.h"
#include "Value.h"
#include "WhereClauses.h"
#include "Parser.h"
using namespace std;

class Instruction{

public:
    virtual void exec(SQL &sql)=0;
    virtual ~Instruction()=default;
};

/*
specific instructions 
Instruction(string) : parse the input (letters are all in lower case)
exec(SQL&) : execute the instruction on the SQL
*/

class CreateDataBaseInst : public Instruction{
    string dbname;
public:
    CreateDataBaseInst()=delete;
    CreateDataBaseInst(string);
    void exec(SQL &sql);
};

class DropDataBaseInst : public Instruction{
    string dbname;
public:
    DropDataBaseInst()=delete;
    DropDataBaseInst(string);
    void exec(SQL &sql);
};

class UseInst : public Instruction{
    string dbname;
public:
    UseInst()=delete;
    UseInst(string);
    void exec(SQL &sql);
};

class ShowDataBasesInst : public Instruction{
    /*no args*/
public:
    ShowDataBasesInst()=default;
    ShowDataBasesInst(string);
    void exec(SQL &sql);
};

class CreateTableInst : public Instruction{
    string tableName;
    vector<Attribute> attrs;
    string primary;
    static Parser ps;
    void parseAttrs();
    void parseAttribute();
public:
    CreateTableInst()=delete;
    CreateTableInst(string);
    void exec(SQL &sql);
};

class DropTableInst : public Instruction{
    string tableName;
public:
    DropTableInst()=delete;
    DropTableInst(string);
    void exec(SQL &sql);
};

class ShowTablesInst : public Instruction{
    /*no args*/
public:
    ShowTablesInst()=default;
    ShowTablesInst(string);
    void exec(SQL &sql);
};

class ShowColumnsFromInst : public Instruction{
    string tableName;
public:
    ShowColumnsFromInst()=delete;
    ShowColumnsFromInst(string);
    void exec(SQL &sql);
};

class InsertIntoInst : public Instruction{
    string tablename;
    vector<string> attrnames, values;
    static Parser ps;
    
public:
    InsertIntoInst()=delete;
    InsertIntoInst(string);
    void exec(SQL &sql);
};

class DeleteFromInst : public Instruction{
    string tablename, whereclauses_str;
    static Parser ps;

public:
    DeleteFromInst()=delete;
    DeleteFromInst(string);
    void exec(SQL &sql);
};

class UpdateInst : public Instruction {
    string tablename, whereclauses_str;
    string attrname, attrexp;
    static Parser ps;
    
public:
    UpdateInst()=delete;
    UpdateInst(string);
    void exec(SQL &sql);
};

class SelectInst : public Instruction{
    string attrname, tablename, whereclauses_str;
    static Parser ps;

public:
    SelectInst()=delete;
    SelectInst(string);
    void exec(SQL &sql);
};