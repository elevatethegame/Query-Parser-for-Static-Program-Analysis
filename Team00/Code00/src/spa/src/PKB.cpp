#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>

#include "PKB.h"
#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInputType.h"

using namespace std;

PKB::PKB(const int& n) : number(n) {}

void PKB::init() {
}

bool PKB::setStatementType(const int& index, const EntityType& type) {
	if (type == EntityType::STMT || type == EntityType::VAR ||
		type == EntityType::CONST || type == EntityType::PROC) {
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
	if (index <= 0 || index > this->number) {
		return false;
	} else {
		int t = RelationshipType::USES;
		string indexString = to_string(index);
		this->relations[t][indexString] = variables;
		for (string v : variables) {
			this->relationsBy[t][v].insert(indexString);
		}
		return true;
	}
}

bool PKB::insertDirectModifies(const int& index, const string& variable) {
	if (index <= 0 || index > this->number) {
		return false;
	} else {
		int t = RelationshipType::MODIFIES;
		string indexString = to_string(index);
		this->relations[t][indexString].insert(variable);
		this->relationsBy[t][variable].insert(indexString);
		return true;
	}
}

bool PKB::insertExpression(const int& index, const long& expression) {
	if (index <= 0 || index > this->number) {
		return false;
	} else {
		this->expressions[expression].insert(to_string(index));
		return true;
	}
}

set<string> PKB::getEntities(const EntityType& type) {
	if (type == EntityType::STMT || type == EntityType::VAR || 
		type == EntityType::CONST || type == EntityType::PROC) {
		return set<string>();
	} else {
		return this->stmts[type];
	}
}

bool PKB::getBooleanResultOfRS(
	const RelationshipType& type, QueryInput input1, QueryInput input2) {
	if (input1.getQueryInputType() == QueryInputType::DECLARATION ||
		input2.getQueryInputType() == QueryInputType::DECLARATION) {
		return false;
	} else {
		set<string> results = this->relations[type][input1.getValue()];
		return (results.find(input2.getValue()) != results.end());
	}
}

unordered_map<string, set<string>> PKB::getResultsOfRS(
	const RelationshipType& type, QueryInput input1, QueryInput input2) {
	return unordered_map<string, set<string>>();
}

bool PKB::getBooleanResultOfPattern(QueryInput input, Expression expression) {
	return true;
}

unordered_map<string, set<string>> PKB::getResultsOfPattern(
	const EntityType& type, QueryInput input, Expression expression) {
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