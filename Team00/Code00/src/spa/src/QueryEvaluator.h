#pragma once
#include <unordered_map>
#include <set>
#include <string>
#include "Query.h"
#include "QueryInputType.h"
#include "PKB.h"
#include "ResultUtil.h"

class QueryEvaluator {
private:
	Query* aQuery;
	PKB* aPKB;

	set<string> evaluate();

	vector<unordered_map<string, string>> evaluateRelationshipClauses(vector<RelationshipClause*>* relationshipClauses);

	vector<unordered_map<string, string>> evaluatePatternClauses(vector<PatternClause*>* patternClauses);

	set<string> evaluateSelectClause(vector<unordered_map<string, string>> results);

	vector<unordered_map<string, string>> mergeResults(vector<unordered_map<string, string>> firstResult, 
		vector<unordered_map<string, string>> secondResult);
	
public:
	
	QueryEvaluator();

	~QueryEvaluator();

	void evaluate(list<string> results);

	void setSelectClause(Declaration* declaration);

	void addRelationshipClause(RelationshipType relationshipType,
		QueryInput* leftQueryInput, QueryInput* rightQueryInput);

	void addPatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression);
};