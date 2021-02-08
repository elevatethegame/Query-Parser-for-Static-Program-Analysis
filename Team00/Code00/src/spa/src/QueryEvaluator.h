#pragma once
#include <unordered_map>
#include <set>
#include <string>
#include "Query.h"
#include "QueryInputType.h"
#include "PKB.h"

class QueryEvaluator {
private:
	Query* aQuery;
	PKB* aPKB;

	set<string> evaluate();

	vector<unordered_map<string, string>> evaluateRelationshipClauses(vector<RelationshipClause*>* relationshipClauses);

	vector<unordered_map<string, string>> evaluatePatternClauses(vector<PatternClause*>* patternClauses);

	set<string> evaluateSelectClause(vector<unordered_map<string, string>> results);

	vector<unordered_map<string, string>> mergeResults(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult);
	
	vector<unordered_map<string, string>> convertPKBResult(unordered_map<string, set<string>> PKBResults, string leftSynonym, string rightSynonym);
	
	vector<unordered_map<string, string>> convertPKBResult(unordered_map<string, set<string>> PKBResults, string synonym);
	
	set<string> getCommonSynonyms(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult);
	
	vector<unordered_map<string, string>> getCartesianProduct(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult);
	
	vector<unordered_map<string, string>> getNaturalJoin(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult, set<string> commonSynonyms);

public:
	
	QueryEvaluator();

	void evaluate(list<string> results);

	void setSelectClause(Declaration* declaration);

	void addRelationshipClause(RelationshipType relationshipType,
		QueryInput* leftQueryInput, QueryInput* rightQueryInput);

	void addPatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression);
};