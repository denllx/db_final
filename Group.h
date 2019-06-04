#pragma once
#include <vector>
#include <memory>
#include "Record.h"
using namespace std;

class Group {
	vector<shared_ptr<Record> > records;
public:
	int size() const;
	shared_ptr<Record> get_record(int record_id) const;

	void addRecord(shared_ptr<Record> r);
};

vector<Group> get_group(const vector<Record>& records,int);
