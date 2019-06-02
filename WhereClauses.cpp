#include "WhereClauses.h"
#include "Parser.h"
#include "SyntaxTree.h"
#include "utils.h"
#include "Value.h"
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

const std::set<std::string> keywords{"or", "and"};

std::string convert(const std::string &s) {
    std::string lower_str = utils::tolower(s);
    if (keywords.find(lower_str) == keywords.end()) return s;
    return lower_str;
}

WhereClauses::WhereClauses(const std::string &s,
                           const std::map<std::string, int> &name2id) {
    std::vector<std::string> symbols;
    std::string symbol;
    Parser parser(s);
    while (!parser.ended()) {
        symbol = parser.get_str();
        symbols.push_back(convert(symbol));
    }
    syntax_tree = Node::create_node(symbols, name2id);
}

bool WhereClauses::check(const Record &r) const {
    return !syntax_tree || syntax_tree->get_value(r)->oprd_type() == NZero;
}
