#include "Group.h"
#include <memory>
#include <vector>



int Group::size() const {
	return records.size();
}

shared_ptr<Record> Group::get_record(int id) const{
	return records[id];
}

void Group::addRecord(shared_ptr<Record> r) {
	records.push_back(r);
}

vector<shared_ptr<Group>> get_group(const vector<Record>& records,int keyid) {
	vector<shared_ptr<Group>> groups;
	int first = 0, tmp = 1,len=records.size(),ngroup=0;
	if (records.size() > 0) {
		while (tmp < len) {
			groups.push_back(make_shared<Group>());
			groups[ngroup]->addRecord(make_shared<Record>(records[first]));
			tmp = first + 1;
			while (tmp < len 
				&& *(records[tmp].get_field(keyid)) == *(records[first].get_field(keyid))) {
				groups[ngroup]->addRecord(make_shared<Record>( records[tmp++]));
			}
			ngroup++;
			first = tmp;
		}
	}
	return groups;
}

bool GroupComparatorById::operator()(const shared_ptr<Group>& group1, const shared_ptr<Group>& group2) const{
	return *(group1->get_record(0)->get_field(keyid)) < *(group2->get_record(0)->get_field(keyid));
}

bool GroupComparatorByExpr::operator()(const shared_ptr<Group>& group1, const shared_ptr<Group>& group2) const {
	return group1->getValue() > group2->getValue();		//Ωµ–Ú≈≈¡–
}

