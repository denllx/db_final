#pragma once
#include <vector>
#include <memory>
#include "Value.h"
using namespace std;

class Record{
    vector<shared_ptr<Value> > field;
public:
    Record(const vector<shared_ptr<Value> >&);
    shared_ptr<Value> get_field(int attr_id) const; /* 0-based */
    void update(int id, shared_ptr<Value>);
    int size() const;
};

class RecordComparator {
	int keyid;
public:
	RecordComparator(int key):keyid(key){}
	bool operator()(const Record&, const Record&) const;
};