#pragma once
#include <string>
#include <set>
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
		const QueryInput& input1, const QueryInput& input2) = 0;

	virtual unordered_map<string, set<string>> getResultsOfRS(
		const RelationshipType& type, const QueryInput& input1, const QueryInput& input2) = 0;

	virtual unordered_map<string, set<string>> getResultsOfPattern(
		const EntityType& type, QueryInput input, Expression expression)= 0;
};