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

class LoadDataInst : public Instruction{
    string tablename,filename;
    static Parser ps;
    vector<string> fieldnames;
public:
    LoadDataInst()=delete;
    LoadDataInst(string);
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
	friend class SelectGroupOrderInst;

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
	void output()override;
public:
	SelectMaxPre() = delete;
	SelectMaxPre(const string& type);
	void parse_attrname() override;

};

class SelectMinPre:public SelectPre {
	string minname;
	void output()override;
public:
	SelectMinPre() = delete;
	SelectMinPre(const string& type);
	void parse_attrname() override;
};

class SelectOutFilePre :public SelectPre {
	string filename;
	void output() override;
public:
	SelectOutFilePre() = delete;
	SelectOutFilePre(const string& type);
	void parse_attrname() override;
};

class SelectAvgPre :public SelectPre {
	string avgname;
	void output() override;
public:
	SelectAvgPre() = delete;
	SelectAvgPre(const string& type);
	void parse_attrname() override;
};

class SelectAsciiPre :public SelectPre {
	string asciiname;
	void output() override;
public:
	SelectAsciiPre() = delete;
	SelectAsciiPre(const string& type);
	void parse_attrname()override;
};

class SelectCharLengthPre :public SelectPre {
	string lenname;
	void output() override;
public:
	SelectCharLengthPre() = delete;
	SelectCharLengthPre(const string& type);
	void parse_attrname()override;
};


class SelectPreFactory {
public:
	shared_ptr<SelectPre> createSelectPre(const string& type,const string& info);
};

class SelectInstFactory;

class SelectInst :public Instruction {
protected:
	shared_ptr<Table> _table;
	vector<Attribute> _attrs;
	vector<Record> _records;
	string whereclauses_str;
	shared_ptr<SelectPre> pre;
	bool _grouped, _ordered;

	virtual void select(SQL& sql) = 0;
	virtual void output() = 0;
public:
	SelectInst(shared_ptr<SelectPre> pre);
	void exec(SQL& sql);
	bool grouped() const{ return _grouped; }
	bool ordered() const{ return _ordered; }
	vector<Record> records() { return _records; }
	const vector<Attribute>& attrs() const{ return _attrs; }
	const shared_ptr<Table> table() const{ return _table; }
	virtual void parse_whereclause() = 0;

};

class SelectNoneInst :public SelectInst {
	friend class SelectInstFactory;

	void select(SQL& sql) override;
	void output() override;

public:
	SelectNoneInst(shared_ptr<SelectPre> pre) :SelectInst(pre) { 
		_grouped = false; _ordered = false;
	}
	void parse_whereclause() override;

};

class SelectGroupInst :public SelectInst {
	void select(SQL& sql) override;
	void output() override;

protected:
	string _groupedname;
	vector<shared_ptr<Group>> _groups;

public:
	SelectGroupInst(shared_ptr<SelectPre> pre) :SelectInst(pre) { _grouped = true; _ordered = false; }
	void parse_groupedname();
	string groupedname() { return _groupedname; }
	const vector<shared_ptr<Group>>& groups() const{ return _groups; }
	void parse_whereclause() override;

};

class SelectOrderInst :public SelectInst {
	string orderedname;		//°´ÕÕÄÄ¸ö×Ö¶Î½øÐÐÅÅÐò

	void select(SQL& sql) override;
	void output() override;
public:
	SelectOrderInst(shared_ptr<SelectPre> pre) :SelectInst(pre) { _grouped = false; _ordered = true; }
	void parse_orderedname();
	void parse_whereclause() override;

};

class SelectGroupOrderInst :public SelectGroupInst {
	string orderedop, orderedvalue;	//orderexpr="count" ordervalue="*" or orderexpr=attrname

	void select(SQL& sql) override;
	void output() override;
public:
	SelectGroupOrderInst(shared_ptr<SelectPre> pre) :SelectGroupInst(pre) { _grouped = true; _ordered = true; }
	void parse_grouped_ordered_name();
	void parse_whereclause() override;

};

class SelectInstFactory {
public:
	shared_ptr<SelectInst> createSelectInst(bool, bool, shared_ptr<SelectPre>);
};
