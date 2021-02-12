#pragma once
#include <unordered_map>
#include <set>
#include <string>
#include "Query.h"
#include "QueryInputType.h"
#include "PKBInterface.h"
#include "ResultUtil.h"
#include "ResultsTable.h"

class QueryEvaluator {
private:
	Query* aQuery;
	PKBInterface* aPKB;

	ResultsTable* evaluateRelationshipClauses(vector<RelationshipClause*>* relationshipClauses, ResultsTable* results);

	ResultsTable* evaluatePatternClauses(vector<PatternClause*>* patternClauses, ResultsTable* results);

	vector<unordered_map<string, string>> evaluateSelectClause(SelectClause* selectClause);

	ResultsTable* mergeResults(unordered_map <string, set<string>> PKBResults, vector<string> synonyms, 
		ResultsTable* currentResults);
	
public:
	
	QueryEvaluator(Query* query, PKBInterface* pkb);

	ResultsTable* evaluate();
};