#include <iostream>
#include <string>
#include <vector>
#include <memory>

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

shared_ptr<PKB> DesignExtractor::extractToPKB() {
	auto result = make_shared<PKB>(this->numberOfStatement);
	for (int i = 1; i <= numberOfStatement; i++) {
		result->setStatementType(i, types[i]);
		for (auto parent: parents[i]) {
			result->insertParent(parent, i);
		}
		for (auto follow: follows[i]) {
			result->insertFollow(follow, i);
		}
		for (auto variable: modifies[i]) {
			result->insertDirectModifies(i, variable);
		}	
		set<string> S;
		for (auto variable : uses[i]) {
			S.insert(variable);
		}
		result->insertDirectUses(i, S);
		for (auto expression : expressions[i]) {
			result->insertExpression(i, expression.getValue());
		}
	}

	for (auto c: constants) {
		result->insertConst(c);
	}

	result->insertProcedure(this->procName);

	return result;
}

vector<string> DesignExtractor::getUses(int index) const {
	return uses[index];
}

vector<string> DesignExtractor::getModifies(int index) const {
	return modifies[index];
}

void DesignExtractor::setCalls(map<string, vector<string>> &edges) {
	calls = edges;
}

void DesignExtractor::insertConstant(string c) {
	constants.insert(c);
}

void DesignExtractor::setProcName(string name) {
	this->procName = name;
}

template<typename T>
unordered_map<T, vector<T> > extractStars(unordered_map<T, vector<T> >& edges) {
	unordered_map<T, vector<T> > results;
	unordered_map<T, bool> was;

	function<void(T)> dfs = [&](T u) {
		vector<T> &answer = results[u];
		for (auto& other: edges[u]) {
			if (!was[other]) {
				dfs(other);
			}
			for (auto& v: result[other]) {
				answer.push_back(v);
			}
			answer.push_back(other);
		}
		sort(anser.begin(), answer.end());
		answer.resize(unique(answer.begin(), answer.end()) - answer.begin());
	}

	return results;
}

