#include <algorithm>
#include "Group.h"
#include "Instruction.h"
using namespace std;

//Parser SelectInst::ps = Parser();

Parser SelectPre::ps = Parser();

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
				cout << inst->attrs[i].get_name() << "\t\n"[i == inst->attrs.size() - 1];
			}
			for (int i = 0; i < inst->records.size(); i++) {
				auto& cur = inst->records[i];
				assert(cur.size() == inst->attrs.size());
				for (int j = 0; j < cur.size(); j++) {
					cout << *cur.get_field(j) << "\t\n"[j == cur.size() - 1];
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
		//stu_name c b a
		auto cast_up = static_cast<SelectGroupInst*>(inst.get());
		assert(attrnames.size()>0 && attrnames[0] == cast_up->groupedname);
		cout << cast_up->groupedname << endl;
		for (int i = 0, len = cast_up->groups.size(); i < len; i++) {
			cout << *(cast_up->groups[i].get_record(0)->get_field(inst->table->export_name2id()[attrnames[0]])) << endl;
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

void SelectCountPre::output() {
	int record_size = inst->records.size();
	if (attrnames.size() > 0) {
		if (attrnames[0] == "*") {
			for (int i = 0; i < inst->attrs.size(); i++) {
				cout << inst->attrs[i].get_name() << "\t\n"[i == inst->attrs.size() - 1];
			}
			for (int i = 0; i < record_size; i++) {
				auto& cur = inst->records[i];
				assert(cur.size() == inst->attrs.size());
				for (int j = 0; j < cur.size(); j++) {
					cout << *cur.get_field(j) << "\t\n"[j == cur.size() - 1];
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
	//输出count的内容
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

shared_ptr<SelectInst> SelectInstFactory::createSelectInst(bool fg, bool fo,shared_ptr<SelectPre> pre) {
	shared_ptr<SelectInst> inst;
	if (!fg && !fo) {		//no group,no order
		inst = make_shared<SelectNoneInst>(pre);
	}
	else if (fg && !fo) {	//group,no order
		inst = make_shared<SelectGroupInst>(pre);
	}
	pre->setInst(inst);
	inst->parse_whereclause();
	if (fg && !fo) {		//group,no order
		static_cast<SelectGroupInst*>(inst.get())->parse_groupedname();
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