#include <memory>

#include "PKBStub.h"

set<string> PKBStub::getEntities(const EntityType& type) {
	return this->getEntitiesReturnValue;
}

unordered_map<string, set<string>> PKBStub::getResultsOfRS(const RelationshipType& type, 
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	return this->relationshipClauseReturnValue;
}

bool PKBStub::getBooleanResultOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	return this->boolReturnValue;
}

unordered_map<string, set<string>> PKBStub::getResultsOfPattern(
	const EntityType& type, shared_ptr<QueryInput> input, Expression expression) {
	return this->patternClauseReturnValue;
}

// setter methods to set the return values for each different test case

void PKBStub::setGetEntitiesReturnValue(set<string> value) {
	this->getEntitiesReturnValue = value;
}

void PKBStub::setGetResultsOfRSReturnValue(unordered_map<string, set<string>> value) {
	this->relationshipClauseReturnValue = value;
}


void PKBStub::setGetBooleanResultOfRSReturnValue(bool value) {
	this->boolReturnValue = value;
}

void PKBStub::setGetResultsOfPatternReturnValue(unordered_map<string, set<string>> value) {
	this->patternClauseReturnValue = value;
}