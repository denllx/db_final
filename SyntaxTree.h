#pragma once
#include "Record.h"
#include "Value.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

using s2i = std::map<std::string, int>;
using vs = std::vector<std::string>;

class Node {
  public:
    static std::shared_ptr<Node> create_node(const vs &, const s2i &);
    virtual ptr_v get_value(const Record &) const = 0;
};

class ConstNode : public Node {
    ptr_v value;

  public:
    ConstNode(ptr_v value);
    ptr_v get_value(const Record &) const override;
};

class AttrNode : public Node {
    int id;

  public:
    AttrNode(int id);
    ptr_v get_value(const Record &) const override;
};

class BinaryNode : public Node {
    std::shared_ptr<Node> lop, rop;
    std::function<ptr_v(ptr_v, ptr_v)> optr;

  public:
    BinaryNode(const vs &lop, const vs &rop, const s2i &,
               const std::string &op_name);
    ptr_v get_value(const Record &) const override;
};
