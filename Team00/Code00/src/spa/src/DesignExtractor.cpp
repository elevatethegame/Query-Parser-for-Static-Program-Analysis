#include <iostream>
#include <string>
#include <vector>

using namespace std;


#include "PKB.h"
#include "DesignExtractor.h"
#include "EntityType.h"
#include <set>


DesignExtractor::DesignExtractor() {
	this->numberOfStatement = 0;
	parents.emplace_back(0);
	follows.emplace_back(0);
	types.emplace_back(EntityType::NONETYPE);
	uses.emplace_back(0);
	modifies.emplace_back(0);
	expressions.push_back({});
}

void DesignExtractor::increaseNumberOfStatement(EntityType type) {
	this->numberOfStatement++;
	parents.emplace_back(0);
	follows.emplace_back(0);
	types.emplace_back(type);
	uses.emplace_back(0);
	modifies.emplace_back(0);
	expressions.push_back({});
}

void DesignExtractor::insertExpression(int id, const Expression& expression) {
	this->expressions[id].push_back(expression);
}

void DesignExtractor::insertParent(int parent, int child) {
    this->parents[child].emplace_back(parent);
}

void DesignExtractor::insertFollow(int before, int after) {
	this->follows[after].emplace_back(before);
}

void DesignExtractor::insertUses(int index, const string& value) {
	this->uses[index].emplace_back(value);
}

void DesignExtractor::insertModifies(int index, const string& value) {
	this->modifies[index].emplace_back(value);
}

PKB DesignExtractor::extractToPKB() {
	PKB result(this->numberOfStatement);
	for (int i = 1; i <= numberOfStatement; i++) {
		result.setStatementType(i, types[i]);
		for (auto parent: parents[i]) {
			result.insertParent(parent, i);
		}
		for (auto follow: follows[i]) {
			result.insertFollow(follow, i);
		}
		for (auto variable: modifies[i]) {
			result.insertDirectModifies(i, variable);
		}	
		set<string> S;
		copy(S.begin(), S.end(), std::back_inserter(uses[i]));
		result.insertDirectUses(i, S);
	}
	return result;
}

vector<string> DesignExtractor::getUses(int index) const {
	return uses[index];
}

vector<string> DesignExtractor::getModifies(int index) const {
	return modifies[index];
}

