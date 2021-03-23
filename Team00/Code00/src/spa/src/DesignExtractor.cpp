#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cassert>

using namespace std;


#include "PKB.h"
#include "EntityType.h"
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
	this->parents[parent].emplace_back(child);
}

void DesignExtractor::insertFollow(int before, int after) {
	this->follows[before].emplace_back(after);
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




vector<int> DesignExtractor::buildCFGBlock(int stmt) {
	auto stmtType = types[stmt];

	assert(follows[stmt].size() <= 1);
	int nextStmt = follows[stmt].empty() ? -1 : follows[stmt][0];

	if (stmtType == EntityType::IF) { /// if statement, jump in one scope
		int firstBlockIfSt = stmt + 1; /// alway the next statement is in first if block
		vector<int> firstBlockIfEnd = buildCFGBlock(firstBlockIfSt);
		
		/// the next statement after last statement of 1st if block is begin of 2nd if block
		int sndBlockIfSt = -1;
		for (auto endStmt : firstBlockIfEnd) {
			sndBlockIfSt = max(sndBlockIfSt, endStmt + 1);
		}
		vector<int> sndBlockIfEnd = buildCFGBlock(sndBlockIfSt);

		vector<int> blockEnds;
		for (auto stmt : firstBlockIfEnd) {
			blockEnds.push_back(stmt);
		}
		for (auto stmt : sndBlockIfEnd) {
			blockEnds.push_back(stmt);
		}

		insertNext(stmt, firstBlockIfSt);
		insertNext(stmt, sndBlockIfSt);
		if (nextStmt != -1) {
			for (auto stmt : blockEnds) {
				insertNext(stmt, nextStmt);
			}
			return buildCFGBlock(nextStmt);
		}
		else {
			return blockEnds;
		}
	} 
	else if (stmtType == EntityType::WHILE) {
		/// while statement, jump in 1 scope 

		int blockSt = stmt + 1; /// alway the next statement is in first if block
		vector<int> blockEnds = buildCFGBlock(blockSt);

		insertNext(stmt, blockSt);		
		for (auto endStmt : blockEnds) {
			insertNext(endStmt, stmt);
		}

		if (nextStmt != -1) {
			insertNext(stmt, nextStmt);
			return buildCFGBlock(nextStmt);
		}
		else {
			return { stmt };
		}
	}
	else {
		if (nextStmt != -1) {
			insertNext(stmt, nextStmt);
			return buildCFGBlock(nextStmt);
		}
		else {
			return { stmt };
		}
	}
}

void DesignExtractor::buildCFG() {
	int currentStmt = 1;
	while (currentStmt <= numberOfStatement) {
		vector<int> blockEnds = buildCFGBlock(currentStmt);
		int lastStmt = -1;
		for (auto stmt : blockEnds) {
			lastStmt = max(lastStmt, stmt);
		}
		currentStmt = lastStmt + 1;
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

template<typename K, typename V>
void sendInformation(const Ownership<K, V>& edges, function<void(K first, V second)> consumer) {
	for (auto& entry: edges) {
		auto& x = entry.first;
		for (auto& y: entry.second) {
			consumer(x, y);
		}
	}
}

shared_ptr<PKB> DesignExtractor::extractToPKB() {
	auto result = make_shared<PKB>(this->numberOfStatement);
	this->buildCFG();
	this->buildIndirectRelationships();

	for (auto c : constants) {
		result->insertConst(c);
	}
	for (int i = 1; i <= numberOfStatement; i++) {
		result->setStatementType(i, types[i]);
	}
	
	///extract parent
	sendInformation<int, int>(convertToMapForm<int, int>(parents, 1, numberOfStatement),
		[&](int a, int b) {
			result->insertParent(a, b);
		}	
	);

	///extract parentStar
	sendInformation<int, int>(parentStar, [&](int a, int b) {
		result->insertParentStar(a, b);
	});

	///extract follow
	sendInformation<int, int>(convertToMapForm<int, int>(follows, 1, numberOfStatement),
		[&](int a, int b) {
			result->insertFollow(a, b);
		}
	);

	///extract followStar
	sendInformation<int, int>(followStar, [&](int a, int b) {
		result->insertFollowStar(a, b);
	});

	///extract next
	sendInformation<int, int>(convertToMapForm<int, int>(nexts, 1, numberOfStatement), 
		[&](int a, int b) {
			result->insertNext(a, b);
		}
	);

	//extract nextStar
	sendInformation<int, int>(nextStar, [&](int a, int b) {
		result->insertNextStar(a, b);
	});

	///extract uses
	sendInformation<int, string>(indirectUses, [&](int a, string b){
		result->insertUses(a, b);
	});

	///extract ProcUses
	sendInformation<string, string>(indirectProcedureUses, [&](string a, string b) {
		result->insertProcUses(a, b);
	});

	///extract modifies
	sendInformation<int, string>(indirectModifies, [&](int a, string b) {
		result->insertModifies(a, b);
	});

	///extract ProcUses
	sendInformation<string, string>(indirectProcedureModifies, [&](string a, string b) {
		result->insertProcModifies(a, b);
	});

	///exgtract Calls
	sendInformation<string, string>(calls, [&](string a, string b) {
		result->insertCalls(a, b);
	});
	
	///extract CallStar
	sendInformation<string, string>(callStar, [&](string a, string b) {
		result->insertCallsStar(a, b);
	});

	///extract procedure
	for (auto x: proceduresList) {
		result->insertProcedure(x);
	}

	///extract variables
	for (auto x: variables) {
		result->insertVariable(x);
	}

	///extract control variable for IF statement
	auto controlIF = filterOwnership<int, string>(
		convertToMapForm<int, string>(uses, 1, numberOfStatement),
		[&](int index) {
			return types[index] == EntityType::IF;
		}
	);
	sendInformation<int, string>(controlIF, [&](int a, string b) {
		result->setControlVariable(a, b);
	});


	///extract control variable for WHILE statement
	auto controlWHILE = filterOwnership<int, string>(
		convertToMapForm<int, string>(uses, 1, numberOfStatement),
		[&](int index) {
			return types[index] == EntityType::WHILE;
		}
	);

	sendInformation<int, string>(controlWHILE, [&](int a, string b) {
		result->setControlVariable(a, b);
	});


	///extract expression
	sendInformation<int, Expression>(convertToMapForm<int, Expression>(expressions, 1, numberOfStatement),
		[&](int a, Expression b) {
			result->insertExpression(a, b);
		}
	);

	return result;
}

vector<int> DesignExtractor::getFollows(int index) const {
	return follows[index];
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

vector<Expression> DesignExtractor::getExpression(int index) const {
	return expressions[index];
}

void DesignExtractor::setCalls(unordered_map<string, vector<string>> &edges) {
	calls = edges;
}

void DesignExtractor::insertConstant(string c) {
	constants.insert(c);
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
	directProcedureUses = convolute<string, int, string>(procedures, directUses);
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

	indirectUses = combine<int, string> (
		directUses,
		extractOwnerships<int, string>(parentStar, directUses)
	);

	indirectModifies = combine<int, string> (
		directModifies,
		extractOwnerships(parentStar, directModifies)
	);
}


