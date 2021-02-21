#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "PKB.h"
#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInputType.h"
#include "Declaration.h"

using namespace std;

PKB::PKB(const int& n) : number(n) {
	for (int i = 1; i <= n; i++) {
		this->entities[EntityType::STMT].insert(to_string(i));
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
			this->entities[type].insert(indexString);
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
	if (index <= 0 || index > this->number || variables.empty()) {
		return false;
	} else {
		int t = RelationshipType::USES;
		string indexString = to_string(index);
		this->relations[t][indexString] = variables;
		this->relationKeys[t].insert(indexString);
		for (string v : variables) {
			this->relationsBy[t][v].insert(indexString);
			this->relationByKeys[t].insert(v);
			this->insertVariable(v);
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
		return true && this->insertVariable(variable);
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

bool PKB::insertVariable(const string& variable) {
	this->entities[EntityType::VAR].insert(variable);
	return true;
}

set<string> PKB::getEntities(const EntityType& type) {
	if (type == EntityType::CONST) {
		return set<string>();
	} else {
		return this->entities[type];
	}
}

bool PKB::getBooleanResultOfRS(const RelationshipType& type, 
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	QueryInputType t1 = input1->getQueryInputType();
	QueryInputType t2 = input2->getQueryInputType();
	if (t1 == QueryInputType::DECLARATION || 
		t2 == QueryInputType::DECLARATION) { // eg. uses(1, v)
		return false;
	} else if (t1 != QueryInputType::ANY && t2 != QueryInputType::ANY){ // eg. follows*(1, 3)
		set<string> results = this->relations[type][input1->getValue()];
		return (results.find(input2->getValue()) != results.end());
	} else if (t1 == QueryInputType::ANY && 
		t2 == QueryInputType::ANY) { // eg. parent(_, _)
		return !this->relations[type].empty();
	} else if (t1 == QueryInputType::ANY) { // eg. follows(_, 3)
		return !this->relationsBy[type][input2->getValue()].empty();
	} else if (t2 == QueryInputType::ANY) { // eg. uses(1, _)
		return !this->relations[type][input1->getValue()].empty();
	} else { // otherwise
		return false;
	}
}

unordered_map<string, set<string>> PKB::getResultsOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	QueryInputType t1 = input1->getQueryInputType();
	QueryInputType t2 = input2->getQueryInputType();
	unordered_map<string, set<string>> ans;
	if (t1 != QueryInputType::DECLARATION &&
		t2 != QueryInputType::DECLARATION) { // eg. uses(1, 2)
		return ans;
	} else if (t1 == QueryInputType::DECLARATION &&
		t2 == QueryInputType::DECLARATION) { // eg. follows*(s1, s2)
		ans = this->relations[type];
		shared_ptr<Declaration> d1 = dynamic_pointer_cast<Declaration>(input1);
		shared_ptr<Declaration> d2 = dynamic_pointer_cast<Declaration>(input2);
		filterMapOfType(d1->getEntityType(), d2->getEntityType(), &ans);
		return ans;
	} else if (t1 == QueryInputType::DECLARATION) {
		switch (t2) {
		case QueryInputType::STMT_NUM: { // eg. parent*(s, 3)
			ans[""] = this->relationsBy[type][input2->getValue()];
			break;
		}
		case QueryInputType::IDENT: { // eg. modifies(s, "x")
			ans[""] = this->relationsBy[type][input2->getValue()];
			break;
		}
		case QueryInputType::ANY: { // eg. follows*(s, _)
			ans[""] = this->relationKeys[type];
			break;
		}
		}
		shared_ptr<Declaration> d = dynamic_pointer_cast<Declaration>(input1);
		filterSetOfType(d->getEntityType(), &(ans[""]));
		return ans;
	} else if (t2 == QueryInputType::DECLARATION) {
		switch (t1) {
		case QueryInputType::STMT_NUM: { // eg. parent*(3, s)
			ans[""] = this->relations[type][input1->getValue()];
			break;
		}
		case QueryInputType::IDENT: { // eg. modifies("main", v)
			ans[""] = this->relations[type][input1->getValue()];
			break;
		}
		case QueryInputType::ANY: { // eg. follows*(_, s)
			ans[""] = this->relationByKeys[type];
			break;
		}
		}
		shared_ptr<Declaration> d = dynamic_pointer_cast<Declaration>(input2);
		filterSetOfType(d->getEntityType(), &(ans[""]));
		return ans;
	} else { // otherwise
		return ans;
	}
}

unordered_map<string, set<string>> PKB::getResultsOfPattern(const EntityType& type, 
	shared_ptr<QueryInput> input, Expression expression) {
	string exp = expression.getValue();
	set<string> res = this->expressions[exp];
	unordered_map<string, set<string>> ans;
	if (type != EntityType::ASSIGN && type != EntityType::STMT) {
		return ans;
	} else if (exp == "_") {
		res = this->getEntities(EntityType::ASSIGN);
	}
	switch (input->getQueryInputType()) {
	case QueryInputType::ANY: { // eg. pattern a(_, _"x"_)
		ans[""] = res;
		// filterSetOfType(type, &(ans[""]));
		break;
	}
	case QueryInputType::DECLARATION: { // eg. pattern a(v, _"x"_)
		for (string s : res) {
			// if (this->types[s] == type) {
			ans[s] = this->relations[MODIFIES][s];
			// }
		}
		break;
	}
	case QueryInputType::IDENT: { // eg. pattern a("x", _"x"_)
		for (string s : res) {
			if (input->getValue() == *(relations[MODIFIES][s].begin())) {
				ans[""].insert(s);
			}
		}
		// filterSetOfType(type, &(ans[""]));
		break;
	}
	default: { } // STMT_NUM
	}
	return ans;
}


void PKB::extractFollowStar() {
	int f = RelationshipType::FOLLOWS;
	int fs = RelationshipType::FOLLOWS_T;
	for (string former : this->relationKeys[f]) {
		set<string> res = this->relations[f][former];
		if (res.empty()) continue;
		this->relationKeys[fs].insert(former);
		while (!res.empty()) { // follow*(s, _)
			string latter = *(res.begin());
			this->relations[fs][former].insert(latter);
			this->relationsBy[fs][latter].insert(former);
			this->relationByKeys[fs].insert(latter);
			res = this->relations[f][latter];
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

void PKB::filterSetOfType(const EntityType& type, set<string>* res) {
	if (type == EntityType::VAR || type == EntityType::CONST ||
		type == EntityType::STMT || type == EntityType::PROC) {
		return;
	}
	set<string> ans = *res;
	for (auto& x : *res) {
		if (this->types[x] != type) {
			ans.erase(x);
		}
	}
	*res = ans;
}

void PKB::filterMapOfType(
	const EntityType& t1, const EntityType& t2,
	unordered_map<string, set<string>>* res) {
	if (t1 == EntityType::VAR || t1 == EntityType::CONST ||
		t1 == EntityType::STMT || t1 == EntityType::PROC) {
		for (auto& x : *res) filterSetOfType(t2, &((*res)[x.first]));
	} else if (t2 == EntityType::VAR || t2 == EntityType::CONST ||
		t2 == EntityType::STMT || t2 == EntityType::PROC) {
		unordered_map<string, set<string>> ans = *res;
		for (auto& x : *res) { 
			if (this->types[x.first] != t1) {
				ans.erase(x.first);
			}
		}
		*res = ans;
	} else {
		unordered_map<string, set<string>> ans = *res;
		for (auto& x : *res) {
			if (this->types[x.first] != t1) {
				ans.erase(x.first);
			} else {
				filterSetOfType(t2, &ans[x.first]);
			}
		}
		*res = ans;
	}

}

