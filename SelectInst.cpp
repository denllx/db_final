#include "Instruction.h"
using namespace std;

Parser SelectInst::ps = Parser();

SelectInst::SelectInst(string s) {
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
}

void SelectInst::exec(SQL &sql){
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
}