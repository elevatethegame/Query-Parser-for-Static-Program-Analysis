#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>

#include "PKB.h"
#include "EntityType.h"
#include "RelationshipType.h"

using namespace std;

PKB::PKB(const int& n) : number(n) {}

void PKB::init() {
}

bool PKB::setStatementType(const int& index, const EntityType& type) {
	if (type == STMT || type == VAR || type == CONST || type == PROC) {
		return false;
	} else if (index > this->number || index <= 0) {
		return false;
	} else {
		string indexString = to_string(index);
		if (this->types.find(indexString) != types.end()) { // key exists
			return false;
		} else {
			this->types[indexString] = type;
			this->stmts[type].insert(indexString);
			return true;
		}
	}
}

bool PKB::insertParent(const int& parent, const int& child) {
	if (parent <= 0 || parent > this->number) {
		return false;
	} else if (child <= 0 || child > this->number) {
		return false;
	} else if (child >= parent) {
		return false;
	} else {
		int t = RelationshipType::PARENT;
		string parentString = to_string(parent);
		string childString = to_string(child);
		if (!relations[t][parentString].empty()) { // key exists
			return false;
		} else {
			this->relations[t][parentString].insert(childString);
			this->relationsBy[t][childString].insert(parentString);
			return true;
		}
	}
}


bool PKB::insertFollow(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	} else if (latter <= 0 || latter > this->number) {
		return false;
	} else if (former >= latter) {
		return false;
	} else {
		int t = RelationshipType::FOLLOWS;
		string formerString = to_string(former);
		string latterString = to_string(latter);
		if (!relations[t][formerString].empty()) { // key exists
			return false;
		} else {
			this->relations[t][formerString].insert(latterString);
			this->relationsBy[t][latterString].insert(formerString);
			return true;
		}
	}
}


bool PKB::insertDirectUses(const int& index, const set<string>& variables) {
	return true;
}

bool PKB::insertDirectModifies(const int& index, const string& variables) {
		return true;
}

bool PKB::insertExpression(const int& index, const long& expression) {
	return true;
}

set<string> PKB::getEntities(const EntityType& type) {
	if (type == STMT || type == VAR || type == CONST || type == PROC) {
		return set<string>();
	} else {
		return this->stmts[type];
	}
}

bool PKB::getBooleanResultOfRS(const RelationshipType& type,
	const QueryInput& input1, const QueryInput& input2) {
	return true;
}

unordered_map<string, set<string>> PKB::getResultsOfRS(
	const RelationshipType& type, const QueryInput& input1, const QueryInput& input2) {
	return unordered_map<string, set<string>>();
}

bool PKB::getBooleanResultOfPattern(
	const QueryInput& input, const Expression& expression) {
	return true;
}


unordered_map<string, set<string>> PKB::getResultsOfPattern(
	const Declaration& input, const Expression& expression) {
	return unordered_map<string, set<string>>();
}


void PKB::extractFollowStar() {
}

void PKB::extractParentStar() {
}

void PKB::extractUses() {
}

void PKB::extractModifies() {
}