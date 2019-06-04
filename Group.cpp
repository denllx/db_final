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

vector<Group> get_group(const vector<Record>& records,int keyid) {
	vector<Group> groups;
	int first = 0, tmp = 1,len=records.size(),ngroup=0;
	if (records.size() > 0) {
		while (tmp < len) {
			groups.push_back(Group());
			groups[ngroup].addRecord(make_shared<Record>(records[first]));
			tmp = first + 1;
			while (tmp < len 
				&& *(records[tmp].get_field(keyid)) == *(records[first].get_field(keyid))) {
				groups[ngroup].addRecord(make_shared<Record>( records[tmp++]));
			}
			ngroup++;
			first = tmp;
		}
	}
	return groups;
}