#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>

using namespace std;


#include "PKB.h"
#include "DesignExtractor.h"
#include "EntityType.h"
#include "DesignExtractorHelper.h"
#include <set>


DesignExtractor::DesignExtractor() {
	this->numberOfStatement = 0;
	parents.emplace_back(0);
	follows.emplace_back(0);
	types.emplace_back(EntityType::NONETYPE);
	uses.emplace_back(0);
	modifies.emplace_back(0);
	nexts.emplace_back(0);
	affects.emplace_back(0);
	expressions.push_back({});
}

void DesignExtractor::increaseNumberOfStatement(EntityType type) {
	this->numberOfStatement++;
	parents.emplace_back(0);
	follows.emplace_back(0);
	types.emplace_back(type);
	uses.emplace_back(0);
	modifies.emplace_back(0);
	nexts.emplace_back(0);
	affects.emplace_back(0);
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

void DesignExtractor::insertStatementCall(int index, const string& procName) {
	this->statementCalls[index].emplace_back(procName);
}

void DesignExtractor::insertNext(int id1, int id2) {
	nexts[id1].emplace_back(id2);
}


int DesignExtractor::buildCFGBlock(int stmt) {
	auto stmtType = types[stmt];

	assert(follows[stmt].size() <= 1);
	int nextStmt = follows[stmt].empty() ? -1 : follows[stmt][0];

	if (stmtType == EntityType::IF) { /// if statement, jump in one scope
		int firstBlockIfSt = stmt + 1; /// alway the next statement is in first if block
		int firstBlockIfEnd = buildCFGBlock(firstBlockIfSt);
		
		/// the next statement after last statement of 1st if block is begin of 2nd if block
		int sndBlockIfSt = firstBlockIfEnd + 1; 
		int sndBlockIfEnd = buildCFGBlock(sndBlockIfSt);

		insertNext(stmt, firstBlockIfSt);
		insertNext(stmt, sndBlockIfSt);
		if (nextStmt != -1) {
			insertNext(firstBlockIfEnd, nextStmt);
			insertNext(sndBlockIfEnd, nextStmt);
		}
	} 
	else if (stmtType == EntityType::WHILE) {
		/// while statement, jump in 1 scope 

		int blockSt = stmt + 1; /// alway the next statement is in first if block
		int blockEnd = buildCFGBlock(blockSt);

		insertNext(stmt, blockSt);
		insertNext(blockEnd, stmt);

		if (nextStmt != -1) {
			insertNext(stmt, nextStmt);
		}
	}
	else {
		if (nextStmt != -1) {
			insertNext(stmt, nextStmt);
		}
	}

	/// if this statement does not have a follow statement, then it should be the last statement in this scope.
	if (nextStmt == -1) {
		return stmt;
	}
	else {
		return buildCFGBlock(nextStmt);
	}
}

void DesignExtractor::buildCFG() {
	int currentStmt = 1;
	while (currentStmt <= numberOfStatement) {
		currentStmt = buildCFGBlock(currentStmt) + 1;
	}
}

void DesignExtractor::affectDFS(int startStmt, int curStmt, string var, vector<int>& visited){
	if (visited[curStmt] == startStmt) {
		/// already visit the statement 
		return; 
	}
	visited[curStmt] = startStmt;

	if (types[curStmt] == EntityType::ASSIGN) {
		///TODO : check if curStmt use the var 
		if (true) {
			affects[startStmt].push_back(curStmt);
		}
	}
	/// TODO : check if curStmt modifies the var

	/// if statement call, then need relationship Modifies 
	if (types[curStmt] == EntityType::CALL) {
		if (true) {
			return;
		}
	}
	///else, only need direct modifies 
	if (true) {
		return;
	}

	for (auto nextStmt : nexts[curStmt]) {
		affectDFS(startStmt, curStmt, var, visited);
	}
}

void DesignExtractor::buildDirectAffect() {
	vector<int> visited(numberOfStatement, 0);
	for (int startStmt = 1; startStmt <= numberOfStatement; startStmt++) {
		if (types[startStmt] == EntityType::ASSIGN) {
			assert(modifies[startStmt].size() == 1);
			string var = modifies[startStmt][0];
			for (auto nextStmt : nexts[startStmt]) {
				affectDFS(startStmt, nextStmt, var, visited);
			}
		}
	}
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

vector<string> DesignExtractor::getIndirectUses(int index) const {
	if (indirectUses.find(index) == indirectUses.end()) return {};
	return indirectUses.at(index);
}

vector<string> DesignExtractor::getIndirectModifies(int index) const {
	if (indirectModifies.find(index) == indirectModifies.end()) return {};
	return indirectModifies.at(index);
}

vector<string> DesignExtractor::getCallStars(string procName) const {
	if (callStar.find(procName) == callStar.end()) return {};
	return callStar.at(procName);
}

vector<int> DesignExtractor::getNext(int index) const {
	return nexts[index];
}

vector<int> DesignExtractor::getNextStar(int index) const {
	if (nextStar.find(index) == nextStar.end()) return {};
	return nextStar.at(index);
}

void DesignExtractor::setCalls(unordered_map<string, vector<string>> &edges) {
	calls = edges;
}

void DesignExtractor::insertConstant(string c) {
	constants.insert(c);
}

void DesignExtractor::setProcName(string name) {
	this->procName = name;
}

void DesignExtractor::setProcedure(string name, int low, int high) {
	procedures[name].resize(high - low + 1);
	iota(procedures[name].begin(), procedures[name].end(), low);
}

void DesignExtractor::buildIndirectRelationships() {
	callStar = extractStars<string>(calls);
	parentStar = extractStars<int>(convertToMapForm<int, int>(parents, 1, numberOfStatement));
	followStar = extractStars<int>(convertToMapForm<int, int>(follows, 1, numberOfStatement));
	nextStar = extractStars<int>(convertToMapForm<int, int>(nexts, 1, numberOfStatement));
	directUses = convertToMapForm<int, string>(uses, 1, numberOfStatement);
	directModifies = convertToMapForm<int, string>(modifies, 1, numberOfStatement);
	directProcedureUses = convolute<string, int, string>(procedures, directModifies);
	directProcedureModifies = convolute<string, int, string>(procedures, directModifies);

	indirectProcedureUses = combine(
		directProcedureUses,
		convolute<string, string, string>(callStar, directProcedureUses)
	);

	indirectProcedureModifies = combine(
		directProcedureModifies,
		convolute<string, string, string>(callStar, directProcedureModifies)
	);

	directUses = combine<int, string>(
		directUses,
		convolute<int, string, string>(statementCalls, indirectProcedureUses)
	);

	directModifies = combine<int, string>(
		directModifies,
		convolute<int, string, string>(statementCalls, indirectProcedureModifies)
	);

	indirectUses = extractOwnerships<int, string>(parentStar, directUses);

	indirectModifies = extractOwnerships<int, string>(parentStar, directModifies);
}


