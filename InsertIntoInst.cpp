#include "Instruction.h"
#include <cstring>
using namespace std;

Parser InsertIntoInst::ps;

InsertIntoInst::InsertIntoInst(string s) {
	ps.reset(s);

	ps.match_token("insert");
	ps.match_token("into");

	tablename = ps.get_str();

	ps.match_token("(");
	attrnames.push_back(move(ps.get_str()));
	while (true) {
		if (ps.lookahead(")")) break;
		ps.match_token(",");
		attrnames.push_back(move(ps.get_str()));
	}
	ps.match_token(")");

	ps.match_token("values");

	ps.match_token("(");
	/*string quote;
	while (!ps.lookahead(")")) {
		if (ps.lookahead("\"") || ps.lookahead("\'")) {
			ps.skip();
			while (!(ps.lookahead("\'") || ps.lookahead("\""))){
				quote += ps.get_str();
				quote += ' ';
			}
			ps.skip();
			values.push_back(quote.substr(0, quote.size() - 1));
			quote ="";
		}
		else {
			values.push_back(move(ps.get_str()));
		}
		if (ps.lookahead(",")) ps.skip();
	}
	ps.skip();*/

    //values.push_back(move(ps.get_str()));
	bool in_quotation = false;
	string tmp;
    while(true) {
        if(ps.lookahead(")")) break;
		tmp = ps.get_str();
		if (!in_quotation) {
			values.push_back(tmp);
		}
		else {
			values[values.size() - 1] += ' ';
			values[values.size()-1]+=tmp;
		}
		if (tmp[0] == '\'' || tmp[0] == '\"') in_quotation = true;
		if (tmp[tmp.size() - 1] == '\'' || tmp[tmp.size() - 1] == '\"') in_quotation = false;
		if (!in_quotation) {
			if (ps.lookahead(",")) ps.skip();
		}
    }
	ps.skip();
    assert(attrnames.size() == values.size());
}

void InsertIntoInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    table->insert_into(attrnames, values);
}
