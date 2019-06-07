#pragma once
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <cassert>
#include "Record.h"
using namespace std;

class Group {
	vector<shared_ptr<Record> > records;
	int value;
public:
	int size() const;
	shared_ptr<Record> get_record(int record_id) const;

	void addRecord(shared_ptr<Record> r);
	void setValue(int v) { value = v; }
	int getValue() const{ return value; }
};

class GroupComparatorById {
	int keyid;
public:
	GroupComparatorById(int key) :keyid(key) {}
	bool operator()(const shared_ptr<Group>&, const shared_ptr<Group>&) const;
};

class GroupComparatorByExpr {
public:
	GroupComparatorByExpr(){}
	bool operator()(const shared_ptr<Group>&, const shared_ptr<Group>&)const;
};

vector<shared_ptr<Group>> get_group(const vector<Record>& records,int);

const static std::map<std::string, std::function<int(shared_ptr<Group>, int)>>
name_expr = {
	{"count", [](shared_ptr<Group> group, int attrid) -> int { 
	int s = group->size();
	if (attrid == -1) return s;
	else {
		int cnt = 0;
		for (int i = 0; i < s; i++) {
			auto record = group->get_record(i);
			if (record->get_field(attrid)->isnull() == false) cnt++;
		}
		return cnt;
	} }},
	{"max", [](shared_ptr<Group> group, int attrid) -> int {
		int s = group->size();
		assert(s > 0);
		auto tmpptr = group->get_record(0)->get_field(attrid);
		int maxval=-INFINITY;
		if (tmpptr->isnull() == false) {
			maxval = *((int*)(tmpptr->getval()));
		}
		int tmp;
		for (int i = 1; i < s; i++) {
			tmpptr = group->get_record(i)->get_field(attrid);
			if (tmpptr->isnull() == false) {
				tmp = *((int*)(tmpptr->getval()));
				maxval = std::max(maxval, tmp);
			}
			else continue;
		}
		return maxval;
	 }},
	{"min", [](shared_ptr<Group> group, int attrid) -> int {
	int s = group->size();
		assert(s > 0);
		auto tmpptr = group->get_record(0)->get_field(attrid);
		int minval = -INFINITY;
		if (tmpptr->isnull() == false) {
			minval = *((int*)(tmpptr->getval()));
		}
		int tmp;
		for (int i = 1; i < s; i++) {
			tmpptr = group->get_record(i)->get_field(attrid);
			if (tmpptr->isnull() == false) {
				tmp = *((int*)(tmpptr->getval()));
				minval = std::min(minval, tmp);
			}
			else continue;
		}
		return minval; }},
	};


