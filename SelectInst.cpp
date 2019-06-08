#include <algorithm>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "Group.h"
#include "Instruction.h"
using namespace utils;
using namespace std;


Parser SelectPre::ps;

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
	else if (type == "avg") {
		pre = make_shared<SelectAvgPre>(info);
	}
	else if (type == "ascii") {
		pre = make_shared<SelectAsciiPre>(info);
	}
	else if (type == "char_length") {
		pre = make_shared<SelectCharLengthPre>(info);
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
	if (!inst->grouped()) {
		if (attrnames[0] == "*") {
			for (int i = 0; i < inst->attrs().size(); i++) {
				cout << inst->attrs()[i].get_name() << "\t";
			}
			cout << endl;
			vector<Record> records = inst->records();
			int records_size = records.size();
			for (int i = 0; i < records_size; i++) {
				auto& cur = records[i];
				assert(cur.size() == inst->attrs().size());
				for (int j = 0; j < cur.size(); j++) {
					cout << *cur.get_field(j) << "\t";
				}
				cout << endl;
			}
			cout << endl;
		}
		else {
			int len = attrnames.size();
			for (int i = 0; i < len; i++) {
				int attrid = inst->table()->export_name2id()[attrnames[i]];
				cout << inst->attrs()[attrid].get_name() << "\t";
			}
			cout << endl;

			for (int i = 0; i < inst->records().size(); i++) {
				for (int j = 0; j < len; j++) {
					cout << *(inst->records())[i].get_field(inst->table()->export_name2id()[attrnames[j]]) << "\t";
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
		assert(attrnames.size()>0 && attrnames[0] == cast_up->groupedname());
		cout << cast_up->groupedname() << endl;
		for (int i = 0, len = cast_up->groups().size(); i < len; i++) {
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(inst->table()->export_name2id()[attrnames[0]])) << endl;
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
	if (!inst->grouped()) {
		int record_size = inst->records().size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs().size(); i++) {
					cout << inst->attrs()[i].get_name() << "\t\n";
				}
				for (int i = 0; i < record_size; i++) {
					auto& cur = inst->records()[i];
					assert(cur.size() == inst->attrs().size());
					for (int j = 0; j < cur.size(); j++) {
						cout << *cur.get_field(j) << "\t\n";
					}
				}
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table()->export_name2id()[attrnames[i]];
					cout << inst->attrs()[attrid].get_name() << "\t";
				}
				cout << endl;

				for (int i = 0; i < record_size; i++) {
					for (int j = 0; j < len; j++) {
						cout << *(inst->records())[i].get_field(inst->table()->export_name2id()[attrnames[j]]) << "\t";
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
			int countedid = inst->table()->export_name2id()[countedname];
			for (int i = 0; i < record_size; i++) {
				ptr_v value = (inst->records())[i].get_field(countedid);
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
		cout << cast_up->groupedname() << "\t" << "count(" << countedname << ")\n";
		int len = cast_up->groups().size();
		int keyid = cast_up->table()->export_name2id()[cast_up->groupedname()];
		int counted_id = cast_up->table()->export_name2id()[countedname];
		for (int i = 0; i < len; i++) {
			int groupsize = cast_up->groups()[i]->size();
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(keyid))<<"\t";
			if (countedname == "*") cout<<groupsize<<endl;
			else {
				int cnt = 0;
				for (int j = 0; j < groupsize; j++) {
					if (cast_up->groups()[i]->get_record(j)->get_field(counted_id)->isnull() == false) cnt++;
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
	if (!inst->grouped()) {
		int record_size = inst->records().size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs().size(); i++) {
					cout << inst->attrs()[i].get_name() << "\t\n";
				}
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table()->export_name2id()[attrnames[i]];
					cout << inst->attrs()[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出最大值
		cout << "MAX(" << maxname << ")\n";
		int maxid = inst->table()->export_name2id()[maxname];
		vector<Record> records = inst->records();
		std::sort(records.begin(), records.end(), RecordComparator(maxid));
		cout << *((int*)(inst->records()[record_size - 1].get_field(maxid)->getval()))<<endl;
	}
	else {
		/*
			stu_name  max(grade)
			c			100
			b			99
			a			98
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname() << "\t" << "max(" << maxname << ")\n";
		int len = cast_up->groups().size();
		int keyid = cast_up->table()->export_name2id()[cast_up->groupedname()];
		int maxid = cast_up->table()->export_name2id()[maxname];
		int maxval;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups()[i]->size();
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			maxval= *((int*)(cast_up->groups()[i]->get_record(0)->get_field(maxid)->getval()));
			int tmp;
			for (int j = 1; j < groupsize; j++) {
				tmp= *((int*)(cast_up->groups()[i]->get_record(j)->get_field(maxid)->getval()));
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
	if (!inst->grouped()) {
		int record_size = inst->records().size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0,len=inst->attrs().size(); i < len; i++) {
					cout << inst->attrs()[i].get_name() << "\t\n";
				}
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table()->export_name2id()[attrnames[i]];
					cout << inst->attrs()[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出最大值
		cout << "MIN(" << minname << ")\n";
		int minid = inst->table()->export_name2id()[minname];
		vector<Record> records=inst->records();
		std::sort(records.begin(), records.end(), RecordComparator(minid));
		cout << *((int*)(inst->records()[0].get_field(minid)->getval())) << endl;
	}
	else {
		/*
			stu_name  max(grade)
			c			100
			b			99
			a			98
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname() << "\t" << "min(" << minname<< ")\n";
		int len = cast_up->groups().size();
		int keyid = cast_up->table()->export_name2id()[cast_up->groupedname()];
		int minid= cast_up->table()->export_name2id()[minname];
		int minval;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups()[i]->size();
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			minval = *((int*)(cast_up->groups()[i]->get_record(0)->get_field(minid)->getval()));
			int tmp;
			for (int j = 1; j < groupsize; j++) {
				tmp = *((int*)(cast_up->groups()[i]->get_record(j)->get_field(minid)->getval()));
				minval= std::min(minval, tmp);
			}
			cout << minval << endl;
		}
	}
}

SelectAvgPre::SelectAvgPre(const string& info) {
	ps.reset(info);
}

void SelectAvgPre::parse_attrname() {
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("avg")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("avg");
	ps.match_token("(");
	avgname= ps.get_str();
	ps.match_token(")");
}

void SelectAvgPre::output() {		
	if (!inst->grouped()) {
		int record_size = inst->records().size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0,len=inst->attrs().size(); i < len; i++) {
					cout << inst->attrs()[i].get_name() << "\t";
				}
				cout << endl;
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table()->export_name2id()[attrnames[i]];
					cout << inst->attrs()[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出平均值
		cout << "AVG(" << avgname<< ")\n";
		int avgid= inst->table()->export_name2id()[avgname];
		double sum = 0,cnt=0;
		ptr_v tmpptr = nullptr;
		for (int i = 0; i < record_size; i++) {
			tmpptr = inst->records()[i].get_field(avgid);
			if (tmpptr->isnull() == false) {
				cnt++;
				sum += *((double*)(tmpptr->getval()));
			}
		}
		if (cnt != 0) cout << (sum / cnt) << endl;
		else cout << "NULL" << endl;
	}
	else {
		/*
			stu_name  avg(grade)
			c			93.3
			b			92.2
			a			91.1
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname() << "\t" << "avg(" << avgname<< ")\n";
		int len = cast_up->groups().size();
		int keyid = cast_up->table()->export_name2id()[cast_up->groupedname()];
		int avgid = cast_up->table()->export_name2id()[avgname];
		double sum = 0;
		int cnt = 0;
		ptr_v tmpptr = nullptr;;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups()[i]->size();
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			sum = 0, cnt = 0;
			for (int j = 0; j < groupsize; j++) {
				tmpptr = cast_up->groups()[i]->get_record(j)->get_field(avgid);
				if (tmpptr->isnull() == false) {
					cnt++;
					switch (tmpptr->oprd_type())
					{
					case Double:
						sum += *((double*)(tmpptr->getval()));
						break;
					case NZero:
						sum += *((int*)(tmpptr->getval()));
					default:
						break;
					}
				}
			}
			if (cnt > 0) cout << (sum / cnt) << endl;
			else cout << "NULL" << endl;
		}
	}
}

SelectAsciiPre::SelectAsciiPre(const string& info) {
	ps.reset(info);
}

void SelectAsciiPre::parse_attrname() {
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("ascii")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("ascii");
	ps.match_token("(");
	asciiname = ps.get_str();
	ps.match_token(")");
}

void SelectAsciiPre::output() {
	if (!inst->grouped()) {
		int record_size = inst->records().size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs().size(); i++) {
					cout << inst->attrs()[i].get_name() << "\t";
				}
				cout << endl;
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table()->export_name2id()[attrnames[i]];
					cout << inst->attrs()[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出首字母
		cout << "Ascii(" << asciiname << ")\n";
		int avgid = inst->table()->export_name2id()[asciiname];
		ptr_v tmpptr = nullptr;
		for (int i = 0; i < record_size; i++) {
			tmpptr = inst->records()[i].get_field(avgid);
			if (tmpptr->isnull() == false) {
				char* v = (char*)(tmpptr->getval());
				cout << v[0] << endl;
			}
		}
	}
	else {
		/*
			stu_name  ascii(stu_name)
			c			l
			b			a
			a			s
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname() << "\t" << "ascii(" << asciiname<< ")\n";
		int len = cast_up->groups().size();
		int keyid = cast_up->table()->export_name2id()[cast_up->groupedname()];
		int asciiid= cast_up->table()->export_name2id()[asciiname];
		ptr_v tmpptr = nullptr;;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups()[i]->size();
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			for (int j = 0; j < groupsize; j++) {
				tmpptr = cast_up->groups()[i]->get_record(j)->get_field(asciiid);
				if (tmpptr->isnull() == false) {
					char* v = (char*)(tmpptr->getval());
					cout << v[0] << endl;
				}
			}
		}
	}
}

SelectCharLengthPre::SelectCharLengthPre(const string& info) {
	ps.reset(info);
}

void SelectCharLengthPre::parse_attrname() {
	ps.match_token("select");
	string tmpattr;
	while (!ps.lookahead("char_length")) {
		tmpattr = ps.get_str();
		if (tmpattr != ",") {
			attrnames.push_back(tmpattr);
		}
	}
	ps.match_token("char_length");
	ps.match_token("(");
	lenname= ps.get_str();
	ps.match_token(")");
}

void SelectCharLengthPre::output() {
	if (!inst->grouped()) {
		int record_size = inst->records().size();
		//输出header
		if (attrnames.size() > 0) {
			if (attrnames[0] == "*") {
				for (int i = 0; i < inst->attrs().size(); i++) {
					cout << inst->attrs()[i].get_name() << "\t";
				}
				cout << endl;
			}
			else {
				int len = attrnames.size();
				for (int i = 0; i < len; i++) {
					int attrid = inst->table()->export_name2id()[attrnames[i]];
					cout << inst->attrs()[attrid].get_name() << "\t";
				}
				cout << endl;
			}
		}
		//输出首字母
		cout << "Char_Length(" << lenname<< ")\n";
		int avgid = inst->table()->export_name2id()[lenname];
		ptr_v tmpptr = nullptr;
		for (int i = 0; i < record_size; i++) {
			tmpptr = inst->records()[i].get_field(avgid);
			if (tmpptr->isnull() == false) {
				char* v = (char*)(tmpptr->getval());
				cout << strlen(v) << endl;
			}
		}
	}
	else {
		/*
			stu_name  ascii(stu_name)
			c			l
			b			a
			a			s
		*/
		SelectGroupInst* cast_up = dynamic_cast<SelectGroupInst*>(inst.get());
		cout << cast_up->groupedname() << "\t" << "ascii(" << lenname<< ")\n";
		int len = cast_up->groups().size();
		int keyid = cast_up->table()->export_name2id()[cast_up->groupedname()];
		int asciiid = cast_up->table()->export_name2id()[lenname];
		ptr_v tmpptr = nullptr;;
		for (int i = 0; i < len; i++) {		//对于每个组
			int groupsize = cast_up->groups()[i]->size();
			cout << *(cast_up->groups()[i]->get_record(0)->get_field(keyid)) << "\t";	//输出排序依据
			for (int j = 0; j < groupsize; j++) {
				tmpptr = cast_up->groups()[i]->get_record(j)->get_field(asciiid);
				if (tmpptr->isnull() == false) {
					char* v = (char*)(tmpptr->getval());
					cout << strlen(v)<< endl;
				}
			}
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
	if (!inst->grouped()) {
		if (attrnames[0] == "*") {	//没有表头
			for (int i = 0,len= inst->records().size(); i < len; i++) {
				auto& cur = inst->records()[i];
				assert(cur.size() == inst->attrs().size());
				for (int j = 0; j < cur.size(); j++) {
					fout << *cur.get_field(j) << "\t";
				}
				fout << endl;
			}
		}
		else {
			int len = attrnames.size();
			for (int i = 0; i < inst->records().size(); i++) {
				for (int j = 0; j < len; j++) {
					fout << *(inst->records())[i].get_field(inst->table()->export_name2id()[attrnames[j]]) << "\t";
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
		assert(attrnames.size() > 0 && attrnames[0] == cast_up->groupedname());
		cout << cast_up->groupedname() << endl;
		for (int i = 0, len = cast_up->groups().size(); i < len; i++) {
			fout << *(cast_up->groups()[i]->get_record(0)->get_field(inst->table()->export_name2id()[attrnames[0]])) << endl;
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
	_table = sql.get_current_database()->get_table(pre->tablenames[0]);
	_attrs = _table->export_id2attr();
	_records = _table->select(WhereClauses(whereclauses_str, _table->export_name2id()));
}

void SelectNoneInst::output() {
	if (_records.empty()) return;
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
	_groupedname = SelectPre::ps.get_str();
}

void SelectGroupInst::select(SQL& sql) {
	_table = sql.get_current_database()->get_table(pre->tablenames[0]);
	_attrs = _table->export_id2attr();
	_records = _table->select(WhereClauses(whereclauses_str, _table->export_name2id()));
	int keyid = _table->export_name2id()[_groupedname];
	//将records按照groupedname排序
	std::sort(_records.begin(), _records.end(), RecordComparator(keyid));
	//Group对象
	_groups = get_group(_records,keyid);
	output();
}

void SelectGroupInst::output() {
	if (_groups.empty()) return;
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
	_table = sql.get_current_database()->get_table(pre->tablenames[0]);
	_attrs = _table->export_id2attr();
	_records = _table->select(WhereClauses(whereclauses_str, _table->export_name2id()));
	//按照orderedname排序
	int ordered_id = _table->export_name2id()[orderedname];
	std::sort(_records.begin(), _records.end(), RecordComparator(ordered_id));
}

void SelectOrderInst::output() {
	if (_records.empty()) return;
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
	_groupedname = SelectPre::ps.get_str();
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
	_table = sql.get_current_database()->get_table(pre->tablenames[0]);
	_attrs = _table->export_id2attr();
	_records = _table->select(WhereClauses(whereclauses_str, _table->export_name2id()));
	int keyid = _table->export_name2id()[_groupedname];
	_groups = get_group(_records, keyid);
	if (orderedvalue=="") {	//ordered by stuname,orderedop="stuname"
		std::sort(_groups.begin(), _groups.end(), GroupComparatorById(keyid));
	}
	else {					//ordered by count(*),orderedop="count",orderedvalue="*"
		int orderedid;
		if (orderedvalue == "*")	orderedid = -1;
		else orderedid = _table->export_name2id()[orderedvalue];
		for (auto group : _groups) {
			group->setValue(name_expr.at(orderedop)(group, orderedid));
		}
		std::sort(_groups.begin(), _groups.end(), GroupComparatorByExpr());
	}
}

void SelectGroupOrderInst::output() {
	if (_groups.empty()) return;
	pre.get()->output();
}
