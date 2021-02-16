#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"

int PKB::setProcToAST(PROC p, TNode* r) {
	return 0;
}

TNode* PKB::getRootAST (PROC p){
	return nullptr;
}

set<string> PKB::getEntities(const EntityType& type) {
	set<string> result;
	return result;
}

bool PKB::getBooleanResultOfRS(const RelationshipType& type,
	const QueryInput& input1, const QueryInput& input2) {
	return false;
}

unordered_map<string, set<string>> PKB::getResultsOfRS(
	const RelationshipType& type, const QueryInput& input1, const QueryInput& input2) {
	unordered_map<string, set<string>> result;
	return result;
}


unordered_map<string, set<string>> PKB::getResultsOfPattern(
	const EntityType& type, QueryInput input, Expression expression) {
	unordered_map<string, set<string>> result;
	return result;
}
