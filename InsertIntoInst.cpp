#include "Instruction.h"
#include <cstring>
using namespace std;

Parser InsertIntoInst::ps=Parser();

InsertIntoInst::InsertIntoInst(string s){
    ps.reset(s);

    ps.match_token("insert");
    ps.match_token("into");

    tablename = ps.get_str();

    ps.match_token("(");
    attrnames.push_back(move(ps.get_str()));
    while(true) {
        if(ps.lookahead(")")) break;
        ps.match_token(",");
        attrnames.push_back(move(ps.get_str()));
    }
    ps.match_token(")");

    ps.match_token("values");

    ps.match_token("(");
    values.push_back(move(ps.get_str()));
    while(true) {
        if(ps.lookahead(")")) break;
        ps.match_token(",");
        values.push_back(move(ps.get_str()));
    }
    ps.match_token(")");
    assert(attrnames.size() == values.size());
}

void InsertIntoInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    table->insert_into(attrnames, values);
}