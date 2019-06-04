#pragma once
#include <string>
#include <memory>
#include <sstream>
#include "SQL.h"
#include "DataBase.h"
#include "Table.h"
#include "Record.h"
#include "Group.h"
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

class SelectInst;

class SelectPre {
protected:
	static Parser ps;
	vector<string> attrnames;
	vector<string> tablenames;
	shared_ptr<SelectInst> inst;

	friend class SelectInst;
	friend class SelectNoneInst;
	friend class SelectGroupInst;
	friend class SelectOrderInst;

	virtual void output() = 0;
public:
	virtual void parse_attrname() = 0;
	void parse_tablenames();
	void setInst(shared_ptr<SelectInst> _inst) { inst = _inst; }
};

class SelectNonePre:public SelectPre {
	void output()override;
public:
	SelectNonePre() = delete;
	SelectNonePre(const string& type);
	void parse_attrname() override;

};

class SelectCountPre:public SelectPre {
	string countedname;
	void output()override;
public:
	SelectCountPre() = delete;
	SelectCountPre(const string& type);
	void parse_attrname() override;

};

class SelectMaxPre:public SelectPre {
	string maxname;
	void output()override {}
public:
	SelectMaxPre() = delete;
	SelectMaxPre(const string& type) {}
	void parse_attrname() override {}

};

class SelectMinPre:public SelectPre {
	string minname;
	void output()override{}
public:
	SelectMinPre() = delete;
	SelectMinPre(const string& type){}
	void parse_attrname() override {}
};


class SelectPreFactory {
public:
	shared_ptr<SelectPre> createSelectPre(const string& type,const string& info);
};

/*class SelectInst : public Instruction{
    string attrname, tablename, whereclauses_str;
    static Parser ps;

public:
    SelectInst()=delete;
    SelectInst(string);
    void exec(SQL &sql);
};*/


class SelectInstFactory;

class SelectInst :public Instruction {
	friend class SelectInstFactory;
	friend class SelectPre;
	friend class SelectCountPre;
	friend class SelectNonePre;
	friend class SelectMaxPre;
	friend class SelectMinPre;
protected:
	shared_ptr<Table> table;
	vector<Attribute> attrs;
	vector<Record> records;
	string whereclauses_str;
	shared_ptr<SelectPre> pre;

	virtual void parse_whereclause() = 0;
	virtual void select(SQL& sql) = 0;
	virtual void output() = 0;
	bool grouped;
public:
	SelectInst(shared_ptr<SelectPre> pre);
	void exec(SQL& sql);
};

class SelectNoneInst :public SelectInst {
	friend class SelectInstFactory;

	void parse_whereclause() override;
	void select(SQL& sql) override;
	void output() override;

	friend class SelectPre;
	friend class SelectCountPre;
	friend class SelectNonePre;
	friend class SelectMaxPre;
	friend class SelectMinPre;
public:
	SelectNoneInst(shared_ptr<SelectPre> pre) :SelectInst(pre) { grouped = false; }
};

class SelectGroupInst :public SelectInst {
	friend class SelectInstFactory;
	friend class SelectPre;
	friend class SelectCountPre;
	friend class SelectNonePre;
	friend class SelectMaxPre;
	friend class SelectMinPre;
	
	string groupedname;
	vector<Group> groups;

	void parse_whereclause() override;
	void select(SQL& sql) override;
	void output() override;
public:
	SelectGroupInst(shared_ptr<SelectPre> pre) :SelectInst(pre) { grouped = true; }
	void parse_groupedname();
};

class SelectOrderInst :public SelectInst {
	friend class SelectInstFactory;
	friend class SelectPre;
	friend class SelectCountPre;
	friend class SelectNonePre;
	friend class SelectMaxPre;
	friend class SelectMinPre;

	string orderedname;		//按照哪个字段进行排序

	void parse_whereclause() override;
	void select(SQL& sql) override;
	void output() override;
public:
	SelectOrderInst(shared_ptr<SelectPre> pre) :SelectInst(pre) { grouped = false; }
	void parse_orderedname();
};

class SelectInstFactory {
public:
	shared_ptr<SelectInst> createSelectInst(bool, bool, shared_ptr<SelectPre>);
};
