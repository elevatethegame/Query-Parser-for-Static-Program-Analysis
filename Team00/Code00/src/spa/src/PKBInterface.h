#pragma once
#include <string>
#include <set>
#include <memory>
#include <unordered_map>

#include "RelationshipType.h"
#include "EntityType.h"
#include "QueryInput.h"
#include "Expression.h"

using namespace std;

class PKBInterface {

public:

	virtual set<string> getEntities(const EntityType& type) = 0;

	virtual bool getBooleanResultOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) = 0;

	virtual unordered_map<string, set<string>> getMapResultsOfRS(const RelationshipType& type, 
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) = 0;

	virtual set<string> getSetResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) = 0;

	virtual unordered_map<string, set<string>> getMapResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression) = 0;

	virtual set<string> getSetResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression) = 0;

	virtual unordered_map<string, set<string>> getMapResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input) = 0;

	virtual set<string> getSetResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input) = 0;
};