#pragma once
#include <unordered_map>
#include <set>
#include <string>
#include <memory>
#include "Query.h"
#include "QueryInputType.h"
#include "PKBInterface.h"
#include "ResultUtil.h"
#include "ResultsTable.h"

class QueryEvaluator {
private:
	shared_ptr<Query> aQuery;
	shared_ptr<PKBInterface> aPKB;

	shared_ptr<ResultsTable> evaluateRelationshipClauses(vector<shared_ptr<RelationshipClause>> relationshipClauses, shared_ptr<ResultsTable> results);

	shared_ptr<ResultsTable> evaluatePatternClauses(vector<shared_ptr<PatternClause>> patternClauses, shared_ptr<ResultsTable> results);

	vector<unordered_map<string, string>> evaluateSelectClause(shared_ptr<SelectClause> selectClause);

	shared_ptr<ResultsTable> mergeResults(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);
	
public:
	
	QueryEvaluator(shared_ptr<Query> query, shared_ptr<PKBInterface> pkb);

	shared_ptr<ResultsTable> evaluate();
};