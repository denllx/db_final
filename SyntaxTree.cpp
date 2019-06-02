#include "SyntaxTree.h"
#include "Record.h"
#include "Value.h"
#include <cassert>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

constexpr int priority_max = 10;

bool is_integer(const std::string &number) {
    return number.find('.') == std::string::npos;
}

int get_priority(const std::string &op_name) {
    const static std::map<std::string, int> priority = {
        {"or", 1}, {"and", 2}, {"<", 3}, {">", 3}, {"=", 3}};
    if (priority.find(op_name) == priority.end()) return priority_max;
    return priority.at(op_name);
}

const static std::map<std::string, std::function<ptr_v(ptr_v, ptr_v)>>
    optr_binary = {
        {"or", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_or(a, b); }},
        {"and", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_and(a, b); }},
        {"<", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_lt(a, b); }},
        {">", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_gt(a, b); }},
        {"=", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_eq(a, b); }}};

std::shared_ptr<Node> Node::create_node(const vs &symbols, const s2i &name2id) {
    if (symbols.empty()) return nullptr;
    if (symbols.size() == 1) {
        std::string s = symbols[0];
        if (name2id.find(s) != name2id.end()) {
            return std::make_shared<AttrNode>(name2id.at(s));
        } else {
            ptr_v value;
            if (s[0] == '\'' || s[0] == '\"') {
                value =
                    std::make_shared<CharValue>(s.substr(1, s.length() - 2));
            } else if (is_integer(s)) {
                value = std::make_shared<IntValue>(std::stoi(s));
            } else {
                value = std::make_shared<DoubleValue>(std::stod(s));
            }
            return std::make_shared<ConstNode>(value);
        }
    }
    std::pair<int, int> pp(priority_max, -1);
    for (int i = 0; i < symbols.size(); ++i) {
        pp = std::min(pp, std::make_pair(get_priority(symbols[i]), i));
    }
    vs left, right;
    for (int i = 0; i < pp.second; ++i) {
        left.push_back(symbols[i]);
    }
    for (int i = pp.second + 1; i < symbols.size(); ++i) {
        right.push_back(symbols[i]);
    }
    return std::make_shared<BinaryNode>(left, right, name2id, symbols[pp.second]);
}

ConstNode::ConstNode(ptr_v value) : value(value) {}
AttrNode::AttrNode(int id) : id(id) {}

ptr_v ConstNode::get_value(const Record &rec) const { return value; }

ptr_v AttrNode::get_value(const Record &rec) const { return rec.get_field(id); }

BinaryNode::BinaryNode(const vs &lop, const vs &rop,
                                 const s2i &name2id, const std::string &op_name)
    : lop(Node::create_node(lop, name2id)),
      rop(Node::create_node(rop, name2id)),
      optr(optr_binary.at(op_name)) {}

ptr_v BinaryNode::get_value(const Record& r) const {
    return optr(lop->get_value(r), rop->get_value(r));
}
