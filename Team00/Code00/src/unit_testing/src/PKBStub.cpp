#include "PKBStub.h"

#include <set>
#include <vector>
#include <unordered_map>

void PKBStub::resetCounts() {
	this->setResultsCount = 0;
	this->mapResultsCount = 0;
	this->boolCount = 0;;
}

PKBStub::PKBStub() {
	this->resetCounts();
}

set<string> PKBStub::getEntities(const EntityType& type) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

unordered_map<string, set<string> > PKBStub::getMapResultsOfRS(
	const RelationshipType& type, shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

set<string> PKBStub::getSetResultsOfRS(
	const RelationshipType& type, shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

unordered_map<string, set<string> > PKBStub::getMapResultsOfAssignPattern(
	shared_ptr<QueryInput> input, Expression& expression) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

set<string> PKBStub::getSetResultsOfAssignPattern(
	shared_ptr<QueryInput> input, Expression& expression) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

unordered_map<string, set<string> > PKBStub::getMapResultsOfContainerPattern(
	const EntityType& type, shared_ptr<QueryInput> input) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

set<string> PKBStub::getSetResultsOfContainerPattern(
	const EntityType& type, shared_ptr<QueryInput> input) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

bool PKBStub::getBooleanResultOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	if (this->boolReturnValues.size() <= this->boolCount) {
		return {};
	}
	return this->boolReturnValues.at(this->boolCount++);
}

// setter methods to set the return values for each different test case
void PKBStub::addBooleanResult(bool value) {
	this->boolReturnValues.push_back(value);
}

void PKBStub::addMapResult(unordered_map<string, set<string> > value) {
	this->mapResults.push_back(value);
}

void PKBStub::addSetResult(set<string> value) {
	this->setResults.push_back(value);
}