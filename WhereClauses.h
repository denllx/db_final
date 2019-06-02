#pragma once
#include <map>
#include "Record.h"
#include "SyntaxTree.h"
using namespace std;

class WhereClauses{
    shared_ptr<Node> syntax_tree;
public:
    WhereClauses(const string&, const map<string, int>&);
    bool check(const Record &r) const;
};