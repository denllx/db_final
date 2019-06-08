#include <algorithm>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "Group.h"
#include "Instruction.h"
using namespace utils;
using namespace std;

//Parser SelectInst::ps = Parser();

Parser SelectPre::ps;

/*SelectInst::SelectInst(string s) {
    ps.reset(s);

    ps.match_token("select");
    attrname = ps.get_str();
    ps.match_token("from");
    tablename = ps.get_str();

	if(ps.lookahead("where")) {
        ps.skip();
        whereclauses_str = ps.get_str();
        while(!ps.ended() && !ps.lookahead(";")) whereclauses_str = whereclauses_str + " " + ps.get_str();
    }
}*/

/*void SelectInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    vector<Attribute>& attrs = table->export_id2attr();
    vector<Record> records = table->select(WhereClauses(whereclauses_str, table->export_name2id()));

    if(records.empty()) return;
    if(attrname == "*") {
        for(int i = 0; i < attrs.size(); i++) {
            cout << attrs[i].get_name() << "\t\n"[i == attrs.size() - 1];
        }
        for(int i = 0; i < records.size(); i++) {
            auto& cur = records[i];
            assert(cur.size() == attrs.size());
            for(int j = 0; j < cur.size(); j++) {
                cout << *cur.get_field(j) << "\t\n"[j == cur.size() - 1];
            }
        }
    } else {
        int attrid = table->export_name2id()[attrname];

        cout << attrs[attrid].get_name() << endl;
        for(int i = 0; i < records.size(); i++) {
            cout << *records[i].get_field(attrid) << endl;
        }
    }
}*/

shared_ptr<SelectPre> SelectPreFactory::createSelectPre(const string& type,const string& info) {
	shared_ptr<SelectPre> pre;
	if (type == "max") {
		pre=make_shared<SelectMaxPre>(info);
	}
	else if (type == "min") {
		pre=make_shared<SelectMinPre>(info);
	}
	else if (type == "count") {
		pre=make_shared<SelectCountPre>(info);
	}
	else if (type == "outfile") {
		pre = make_shared<SelectOutFilePre>(info);
	}
	else pre = make_shared<SelectNonePre>(info);
	pre->parse_attrname();
	pre->parse_tablenames();
	return pre;
}

void SelectPre::parse_tablenames() {
	ps.match_token("from");
	string tmp;
	tmp = ps.get_str();
	tablenames.push_back(tmp);
	while (!ps.ended() && ps.lookahead(",")) {
		ps.skip();
		tmp = ps.get_str();
		tablenames.push_back(tmp);
	}
}

SelectNonePre::SelectNonePre(const string& info) {
	ps.reset(info);
}

void SelectNonePre::parse_attrname() {
	//从select开始，到from结束
	string tmp;
	ps.match_token("select");
	while (!ps.lookahead("from")) {
		tmp = ps.get_str();
		if (tmp != ",") {
			attrnames.push_back(tmp);
		}
	}
}

//常规模式输出
void SelectNonePre::output(){
	if (!inst->grouped) {
		if (attrnames[0] == "*") {
			for (int i = 0; i < inst->attrs.size(); i++) {
				cout << inst->attrs[i].get_name() << "\t";
			}
			for (int i = 0; i < inst->records.size(); i++) {
				auto& cur = inst->records[i];
				assert(cur.size() == inst->attrs.size());
				for (int j = 0; j < cur.size(); j++) {
					cout << *cur.get_field(j) << "\t";
				}
			}
		}
		else {
			int len = attrnames.size();
			for (int i = 0; i < len; i++) {
				int attrid = inst->table->export_name2id()[attrnames[i]];
				cout << inst->attrs[attrid].get_name() << "\t";
			}
			cout << endl;

			for (int i = 0; i < inst->records.size(); i++) {
				for (int j = 0; j < len; j++) {
					cout << *(inst->records)[i].get_field(inst->table->export_name2id()[attrnames[j]]) << "\t";
				}
				cout << endl;
			}
		}
	}
	else {
		/*
			stu_name
			c
			b
			a
		*/
		auto cast_up = static_cast<SelectGroupInst*>(inst.get());
		assert(attrnames.size()>0 && attrnames[0] == cast_up->groupedname);
		cout << cast_up->groupedname << endl;
		for (int i = 0, len = cast_up->groups.size(); i < len; i++) {
			cout << *(cast_up->groups[i]->get_record(0)->get_field(inst->table->export_name2id()[attrnames[0]])) << endl;
		}
	}
}

SelectCountPre::SelectCountPre(const string& info) {
	ps.reset(info);
}


void SelectCountPre::parse_attrname() {
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("count")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("count");
	ps.match_token("(");
	countedname = ps.get_str();
	ps.match_token(")");
}

//输出count
void SelectCountPre::output() {
	if (!inst->grouped) {
		int record_size = inst->records.size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs.size(); i++) {
					cout << inst->attrs[i].get_name() << "\t\n";
				}
				for (int i = 0; i < record_size; i++) {
					auto& cur = inst->records[i];
					assert(cur.size() == inst->attrs.size());
					for (int j = 0; j < cur.size(); j++) {
						cout << *cur.get_field(j) << "\t\n";
					}
				}
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table->export_name2id()[attrnames[i]];
					cout << inst->attrs[attrid].get_name() << "\t";
				}
				cout << endl;

				for (int i = 0; i < record_size; i++) {
					for (int j = 0; j < len; j++) {
						cout << *(inst->records)[i].get_field(inst->table->export_name2id()[attrnames[j]]) << "\t";
					}
					cout << endl;
				}
			}
		}
		//输出数量
		cout << "COUNT(" << countedname << ")\n";
		if (countedname == "*") {
			cout << record_size << endl;
		}
		else {
			int cnt = 0;
			int countedid = inst->table->export_name2id()[countedname];
			for (int i = 0; i < record_size; i++) {
				ptr_v value = (inst->records)[i].get_field(countedid);
				if (!(value.get()->isnull())) cnt++;
			}
			cout << cnt << endl;
		}
	}
	else {
		/*
			stu_name  count(*)
			c			1
			b			2
			a			3
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname << "\t" << "count(" << countedname << ")\n";
		int len = cast_up->groups.size();
		int keyid = cast_up->table->export_name2id()[cast_up->groupedname];
		int counted_id = cast_up->table->export_name2id()[countedname];
		for (int i = 0; i < len; i++) {
			int groupsize = cast_up->groups[i]->size();
			cout << *(cast_up->groups[i]->get_record(0)->get_field(keyid))<<"\t";
			if (countedname == "*") cout<<groupsize<<endl;
			else {
				int cnt = 0;
				for (int j = 0; j < groupsize; j++) {
					if (cast_up->groups[i]->get_record(j)->get_field(counted_id)->isnull() == false) cnt++;
				}
				cout << cnt << endl;
			}
		}
	}
}

SelectMaxPre::SelectMaxPre(const string& info) {
	ps.reset(info);
}

void SelectMaxPre::parse_attrname() {
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("max")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("max");
	ps.match_token("(");
	maxname = ps.get_str();
	ps.match_token(")");
}

void SelectMaxPre::output() {
	if (!inst->grouped) {
		int record_size = inst->records.size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs.size(); i++) {
					cout << inst->attrs[i].get_name() << "\t\n";
				}
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table->export_name2id()[attrnames[i]];
					cout << inst->attrs[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出最大值
		cout << "MAX(" << maxname << ")\n";
		int maxid = inst->table->export_name2id()[maxname];
		std::sort(inst->records.begin(), inst->records.end(), RecordComparator(maxid));
		cout << *((int*)(inst->records[record_size - 1].get_field(maxid)->getval()))<<endl;
	}
	else {
		/*
			stu_name  max(grade)
			c			100
			b			99
			a			98
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname << "\t" << "max(" << maxname << ")\n";
		int len = cast_up->groups.size();
		int keyid = cast_up->table->export_name2id()[cast_up->groupedname];
		int maxid = cast_up->table->export_name2id()[maxname];
		int maxval;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups[i]->size();
			cout << *(cast_up->groups[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			maxval= *((int*)(cast_up->groups[i]->get_record(0)->get_field(maxid)->getval()));
			int tmp;
			for (int j = 1; j < groupsize; j++) {
				tmp= *((int*)(cast_up->groups[i]->get_record(j)->get_field(maxid)->getval()));
				maxval = std::max(maxval, tmp);
			}
			cout << maxval << endl;
		}
	}
}

SelectMinPre::SelectMinPre(const string& info) {
	ps.reset(info);
}

void SelectMinPre::parse_attrname(){
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("min")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("min");
	ps.match_token("(");
	minname = ps.get_str();
	ps.match_token(")");
}

void SelectMinPre::output() {
	if (!inst->grouped) {
		int record_size = inst->records.size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs.size(); i++) {
					cout << inst->attrs[i].get_name() << "\t\n";
				}
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table->export_name2id()[attrnames[i]];
					cout << inst->attrs[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出最大值
		cout << "MIN(" << minname << ")\n";
		int minid = inst->table->export_name2id()[minname];
		std::sort(inst->records.begin(), inst->records.end(), RecordComparator(minid));
		cout << *((int*)(inst->records[0].get_field(minid)->getval())) << endl;
	}
	else {
		/*
			stu_name  max(grade)
			c			100
			b			99
			a			98
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname << "\t" << "min(" << minname<< ")\n";
		int len = cast_up->groups.size();
		int keyid = cast_up->table->export_name2id()[cast_up->groupedname];
		int minid= cast_up->table->export_name2id()[minname];
		int minval;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups[i]->size();
			cout << *(cast_up->groups[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			minval = *((int*)(cast_up->groups[i]->get_record(0)->get_field(minid)->getval()));
			int tmp;
			for (int j = 1; j < groupsize; j++) {
				tmp = *((int*)(cast_up->groups[i]->get_record(j)->get_field(minid)->getval()));
				minval= std::min(minval, tmp);
			}
			cout << minval << endl;
		}
	}
}

SelectOutFilePre::SelectOutFilePre(const string& info) {
	ps.reset(info);
}

void SelectOutFilePre::parse_attrname() {
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("into")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("into");
	ps.match_token("outfile");
	filename= ps.get_str();
	filename = filename.substr(1, filename.size() - 2);
	ifstream fin;	//不能是一个已经存在的文件
	fin.open(filename);
	assert(!fin);
}

void SelectOutFilePre::output() {
	ofstream fout(filename);
	if (!inst->grouped) {
		if (attrnames[0] == "*") {	//没有表头
			for (int i = 0; i < inst->records.size(); i++) {
				auto& cur = inst->records[i];
				assert(cur.size() == inst->attrs.size());
				for (int j = 0; j < cur.size(); j++) {
					fout << *cur.get_field(j) << "\t";
				}
				fout << endl;
			}
		}
		else {
			int len = attrnames.size();
			for (int i = 0; i < inst->records.size(); i++) {
				for (int j = 0; j < len; j++) {
					fout << *(inst->records)[i].get_field(inst->table->export_name2id()[attrnames[j]]) << "\t";
				}
				fout << endl;
			}
		}
	}
	else {
		/*
			stu_name
			c
			b
			a
		*/
		auto cast_up = static_cast<SelectGroupInst*>(inst.get());
		assert(attrnames.size() > 0 && attrnames[0] == cast_up->groupedname);
		cout << cast_up->groupedname << endl;
		for (int i = 0, len = cast_up->groups.size(); i < len; i++) {
			fout << *(cast_up->groups[i]->get_record(0)->get_field(inst->table->export_name2id()[attrnames[0]])) << endl;
		}
	}
	fout.close();
}



shared_ptr<SelectInst> SelectInstFactory::createSelectInst(bool fg, bool fo,shared_ptr<SelectPre> pre) {
	shared_ptr<SelectInst> inst;
	if (!fg && !fo) {		//no group,no order
		inst = make_shared<SelectNoneInst>(pre);
	}
	else if (fg && !fo) {	//group,no order
		inst = make_shared<SelectGroupInst>(pre);
	}
	else if (!fg && fo) {
		inst = make_shared<SelectOrderInst>(pre);
	}
	else {
		inst = make_shared<SelectGroupOrderInst>(pre);
	}
	pre->setInst(inst);
	inst->parse_whereclause();
	if (fg && !fo) {		//group,no order
		static_cast<SelectGroupInst*>(inst.get())->parse_groupedname();
	}
	else if (!fg && fo) {
		static_cast<SelectOrderInst*>(inst.get())->parse_orderedname();
	}
	else if (fg&&fo) {
		static_cast<SelectGroupOrderInst*>(inst.get())->parse_grouped_ordered_name();
	}
	return inst;
}


SelectInst::SelectInst(shared_ptr<SelectPre> p) {
	this->pre = p;
}

void SelectInst::exec(SQL& sql) {
	select(sql);
	output();
}

void SelectNoneInst::parse_whereclause() {
	//从where开始，到分号结束
	if (SelectPre::ps.lookahead("where")) {
		SelectPre::ps.skip();
		whereclauses_str = SelectPre::ps.get_str();
		while (!SelectPre::ps.ended() && !SelectPre::ps.lookahead(";")) {
			whereclauses_str = whereclauses_str + " " + SelectPre::ps.get_str();
		}
	}
}

void SelectNoneInst::select(SQL& sql) {
	table = sql.get_current_database()->get_table(pre->tablenames[0]);
	attrs = table->export_id2attr();
	records = table->select(WhereClauses(whereclauses_str, table->export_name2id()));
}

void SelectNoneInst::output() {
	if (records.empty()) return;
	pre.get()->output();
}

void SelectGroupInst::parse_whereclause() {
	//从where开始，到group结束
	if (SelectPre::ps.lookahead("where")) {
		SelectPre::ps.skip();
		whereclauses_str = SelectPre::ps.get_str();
		while (!SelectPre::ps.ended() && !SelectPre::ps.lookahead("group")) {
			whereclauses_str = whereclauses_str + " " + SelectPre::ps.get_str();
		}
	}
}

void SelectGroupInst::parse_groupedname() {
	SelectPre::ps.match_token("group");
	SelectPre::ps.match_token("by");
	groupedname = SelectPre::ps.get_str();
}

void SelectGroupInst::select(SQL& sql) {
	table = sql.get_current_database()->get_table(pre->tablenames[0]);
	attrs = table->export_id2attr();
	records = table->select(WhereClauses(whereclauses_str, table->export_name2id()));
	int keyid = table->export_name2id()[groupedname];
	//将records按照groupedname排序
	std::sort(records.begin(), records.end(), RecordComparator(keyid));
	//Group对象
	groups = get_group(records,keyid);
	output();
}

void SelectGroupInst::output() {
	if (groups.empty()) return;
	pre.get()->output();
}

void SelectOrderInst::parse_whereclause() {
	//从where开始，到ordered结束
	if (SelectPre::ps.lookahead("where")) {
		SelectPre::ps.skip();
		whereclauses_str = SelectPre::ps.get_str();
		while (!SelectPre::ps.ended() && !SelectPre::ps.lookahead("order")) {
			whereclauses_str = whereclauses_str + " " + SelectPre::ps.get_str();
		}
	}
}

void SelectOrderInst::parse_orderedname() {
	SelectPre::ps.match_token("order");
	SelectPre::ps.match_token("by");
	orderedname = SelectPre::ps.get_str();
}

void SelectOrderInst::select(SQL& sql) {
	table = sql.get_current_database()->get_table(pre->tablenames[0]);
	attrs = table->export_id2attr();
	records = table->select(WhereClauses(whereclauses_str, table->export_name2id()));
	//按照orderedname排序
	int ordered_id = table->export_name2id()[orderedname];
	std::sort(records.begin(), records.end(), RecordComparator(ordered_id));
}

void SelectOrderInst::output() {
	if (records.empty()) return;
	pre.get()->output();
}

void SelectGroupOrderInst::parse_whereclause() {
	if (SelectPre::ps.lookahead("where")) {
		SelectPre::ps.skip();
		whereclauses_str = SelectPre::ps.get_str();
		while (!SelectPre::ps.ended() && !SelectPre::ps.lookahead("group")) {
			whereclauses_str = whereclauses_str + " " + SelectPre::ps.get_str();
		}
	}
}

void SelectGroupOrderInst::parse_grouped_ordered_name() {
	SelectPre::ps.match_token("group");
	SelectPre::ps.match_token("by");
	groupedname = SelectPre::ps.get_str();
	SelectPre::ps.match_token("order");
	SelectPre::ps.match_token("by");
	orderedop = SelectPre::ps.get_str();

	//如果是order by count(*)类型，则orderedop=count,ordervalue=*
	if (SelectPre::ps.lookahead("(")) {
		SelectPre::ps.skip();
		orderedvalue = SelectPre::ps.get_str();
		SelectPre::ps.skip();
		orderedop = tolower(orderedop);
	}
	//否则，orderedop=attrname,orderevalue=空串
	else {
		orderedvalue = "";
	}
}

void SelectGroupOrderInst::select(SQL& sql) {
	table = sql.get_current_database()->get_table(pre->tablenames[0]);
	attrs = table->export_id2attr();
	records = table->select(WhereClauses(whereclauses_str, table->export_name2id()));
	int keyid = table->export_name2id()[groupedname];
	groups = get_group(records, keyid);
	if (orderedvalue=="") {	//ordered by stuname,orderedop="stuname"
		std::sort(groups.begin(), groups.end(), GroupComparatorById(keyid));
	}
	else {					//ordered by count(*),orderedop="count",orderedvalue="*"
		int orderedid;
		if (orderedvalue == "*")	orderedid = -1;
		else orderedid = table->export_name2id()[orderedvalue];
		for (auto group : groups) {
			group->setValue(name_expr.at(orderedop)(group, orderedid));
		}
		std::sort(groups.begin(), groups.end(), GroupComparatorByExpr());
	}
}

void SelectGroupOrderInst::output() {
	if (groups.empty()) return;
	pre.get()->output();
}
