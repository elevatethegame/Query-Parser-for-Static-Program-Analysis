#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>

#include "PKB.h"
#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInputType.h"
#include "Declaration.h"

using namespace std;

PKB::PKB(const int& n) : number(n) {
	for (int i = 1; i <= n; i++) {
		this->stmts[EntityType::STMT].insert(to_string(i));
	}
}

void PKB::init() {
	this->extractFollowStar();
	this->extractParentStar();
	this->extractUses();
	this->extractModifies();
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
	} else if (child <= parent) {
		return false;
	} else {
		int t = RelationshipType::PARENT;
		string parentString = to_string(parent);
		string childString = to_string(child);
		if (this->relationByKeys[t].find(childString) != 
			this->relationByKeys[t].end()) { // key exists
			return false;
		} else {
			this->relations[t][parentString].insert(childString);
			this->relationsBy[t][childString].insert(parentString);
			this->relationKeys[t].insert(parentString);
			this->relationByKeys[t].insert(childString);
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
		if (this->relationKeys[t].find(formerString) !=
			this->relationKeys[t].end()) { // key exists
			return false;
		} else {
			this->relations[t][formerString].insert(latterString);
			this->relationsBy[t][latterString].insert(formerString);
			this->relationKeys[t].insert(formerString);
			this->relationByKeys[t].insert(latterString);
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
		this->relationKeys[t].insert(indexString);
		for (string v : variables) {
			this->relationsBy[t][v].insert(indexString);
			this->relationByKeys[t].insert(v);
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
		this->relationKeys[t].insert(indexString);
		this->relationByKeys[t].insert(variable);
		return true;
	}
}

bool PKB::insertExpression(const int& index, const string& expression) {
	if (index <= 0 || index > this->number) {
		return false;
	} else {
		this->expressions[expression].insert(to_string(index));
		return true;
	}
}

set<string> PKB::getEntities(const EntityType& type) {
	if (type == EntityType::VAR || type == EntityType::CONST || type == EntityType::PROC) {
		return set<string>();
	} else {
		return this->stmts[type];
	}
}

bool PKB::getBooleanResultOfRS(
	const RelationshipType& type, QueryInput input1, QueryInput input2) {
	QueryInputType t1 = input1.getQueryInputType();
	QueryInputType t2 = input2.getQueryInputType();
	if (t1 == QueryInputType::DECLARATION || 
		t2 == QueryInputType::DECLARATION) { // eg. uses(1, v)
		return false;
	} else if (t1 != QueryInputType::ANY && t2 != QueryInputType::ANY){ // eg. follows*(1, 3)
		set<string> results = this->relations[type][input1.getValue()];
		return (results.find(input2.getValue()) != results.end());
	} else if (t1 == QueryInputType::ANY && 
		t2 == QueryInputType::ANY) { // eg. parent(_, _)
		return !this->relations[type].empty();
	} else if (t1 == QueryInputType::ANY) { // eg. follows(_, 3)
		return !this->relationsBy[type][input2.getValue()].empty();
	} else if (t2 == QueryInputType::ANY) { // eg. uses(1, _)
		return !this->relations[type][input1.getValue()].empty();
	} else { // otherwise
		return false;
	}
}

unordered_map<string, set<string>> PKB::getResultsOfRS(
	const RelationshipType& type, QueryInput input1, QueryInput input2) {
	QueryInputType t1 = input1.getQueryInputType();
	QueryInputType t2 = input2.getQueryInputType();
	unordered_map<string, set<string>> ans;
	if (t1 != QueryInputType::DECLARATION &&
		t2 != QueryInputType::DECLARATION) { // eg. uses(1, 2)
		return ans;
	} else if (t1 == QueryInputType::DECLARATION &&
		t2 == QueryInputType::DECLARATION) { // eg. follows*(s1, s2)
		return this->relations[type];
	} else if (t1 == QueryInputType::DECLARATION) {
		switch (t2) {
		case QueryInputType::STMT_NUM: { // eg. parent*(s, 3)
			ans[""] = this->relationsBy[type][input2.getValue()];
			break;
		}
		case QueryInputType::IDENT: { // eg. modifies(s, "x")
			ans[""] = this->relationsBy[type][input2.getValue()];
			break;
		}
		case QueryInputType::ANY: { // eg. follows*(s, _)
			ans[""] = this->relationKeys[type];
			break;
		}
		}
		return ans;
	} else if (t2 == QueryInputType::DECLARATION) {
		switch (t1) {
		case QueryInputType::STMT_NUM: { // eg. parent*(3, s)
			ans[""] = this->relations[type][input1.getValue()];
			break;
		}
		case QueryInputType::IDENT: { // eg. modifies("main", v)
			ans[""] = this->relations[type][input1.getValue()];
			break;
		}
		case QueryInputType::ANY: { // eg. follows*(_, s)
			ans[""] = this->relationByKeys[type];
			break;
		}
		}
		return ans;
	} else { // otherwise
		return ans;
	}
}

unordered_map<string, set<string>> PKB::getResultsOfPattern(
	const EntityType& type, QueryInput input, Expression expression) {
	string exp = expression.getValue();
	set<string> res = this->expressions[exp];
	unordered_map<string, set<string>> ans;
	if (type != EntityType::ASSIGN && type != EntityType::STMT) {
		return ans;
	}
	switch (input.getQueryInputType()) {
	case QueryInputType::ANY: { // eg. pattern a(_, _"x"_)
		ans[""] = res;
		break;
	}
	case QueryInputType::DECLARATION: { // eg. pattern a(v, _"x"_)
		for (string s : res) {
			ans[s] = this->relations[MODIFIES][s];
		}
		break;
	}
	case QueryInputType::IDENT: { // eg. pattern a("x", _"x"_)
		for (string s : res) {
			if (input.getValue() == *(relations[MODIFIES][s].begin())) {
				ans[""].insert(s);
			}
		}
		break;
	}
	default: { } // STMT_NUM
	}
	return ans;
}


void PKB::extractFollowStar() {
	int p = RelationshipType::PARENT;
	int fs = RelationshipType::FOLLOWS_T;
	for (int i = 1; i < this->number; i++) {
		for (int j = i + 1; j <= this->number; j++) {
			string former = to_string(i);
			string latter = to_string(j);
			bool e1 = relationByKeys[p].find(former) == relationByKeys[p].end();
			bool e2 = relationByKeys[p].find(latter) == relationByKeys[p].end();
			if (e1 != e2) {
				continue;
			} else if ((e1 && e2) || // no parent or same parent
				relationsBy[p][former] == relationsBy[p][latter]) { 
				relations[fs][former].insert(latter);
				relationsBy[fs][latter].insert(former);
				relationKeys[fs].insert(former);
				relationByKeys[fs].insert(latter);
			}
		}
	}
}

void PKB::extractParentStar() {
	int p = RelationshipType::PARENT;
	int ps = RelationshipType::PARENT_T;
	for (string s : this->relationByKeys[p]) {
		relationByKeys[ps].insert(s);
		set<string> res = this->relationsBy[p][s];
		while (!res.empty()) { // parent*(_, s)
			string parent = *(res.begin());
			this->relations[ps][parent].insert(s);
			this->relationsBy[ps][s].insert(parent);
			this->relationKeys[ps].insert(parent);
			res = this->relationsBy[p][parent];
		}
	}
}

void PKB::extractUses() {
	int ps = RelationshipType::PARENT_T;
	int u = RelationshipType::USES;
	for (string s : this->relationKeys[ps]) {
		for (string c : this->relations[ps][s]) {
			set<string> used = this->relations[u][c];
			if (!used.empty()) {
				this->relationKeys[u].insert(s);
			}
			for (string v : used) {
				this->relations[u][s].insert(v);
				this->relationsBy[u][v].insert(s);
			}
		}
	}
}

void PKB::extractModifies() {
	int ps = RelationshipType::PARENT_T;
	int m = RelationshipType::MODIFIES;
	for (string s : this->relationKeys[ps]) {
		for (string c : this->relations[ps][s]) {
			set<string> modified = this->relations[m][c];
			if (!modified.empty()) {
				this->relationKeys[m].insert(s);
			}
			for (string v : modified) {
				this->relations[m][s].insert(v);
				this->relationsBy[m][v].insert(s);
			}
		}
	}
}