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
	} else {
		int t = RelationshipType::FOLLOWS;
		this->relations[t][to_string(parent)].insert(to_string(child));
		this->relationsBy[t][to_string(child)].insert(to_string(parent));
		return true;
	}
}


bool PKB::insertFollow(const int& former, const int& latter) {
	return true;
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

void PKB::extractFollowStar() {
}

void PKB::extractParentStar() {
}

void PKB::extractUses() {
}

void PKB::extractModifies() {
}