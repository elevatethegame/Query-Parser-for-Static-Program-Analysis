#pragma once
#include "PKBInterface.h"

class PKBStub : public PKBInterface {
public:

	unordered_map<string, set<string>> relationshipClauseReturnValue;
	unordered_map<string, set<string>> patternClauseReturnValue;
	set<string> getEntitiesReturnValue;
	bool boolReturnValue = true;

	// original api being stubbed

	set<string> getEntities(const EntityType& type);

	unordered_map<string, set<string>> getResultsOfRS(
		const RelationshipType& type, const QueryInput& input1, const QueryInput& input2);


	bool getBooleanResultOfRS(const RelationshipType& type,
		const QueryInput& input1, const QueryInput& input2);

	unordered_map<string, set<string>> getResultsOfPattern(
		const EntityType& type, QueryInput input, Expression expression);

	// setter methods to set the return values for each different test case

	void setGetEntitiesReturnValue(set<string> value);

	void setGetResultsOfRSReturnValue(unordered_map<string, set<string>> value);

	void setGetBooleanResultOfRSReturnValue(bool value);

	void setGetResultsOfPatternReturnValue(unordered_map<string, set<string>> value);
};