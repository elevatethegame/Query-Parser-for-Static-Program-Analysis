#pragma once
#include <unordered_map>
#include <set>
#include <string>
#include <memory>
#include "QueryInterface.h"
#include "QueryInputType.h"
#include "PKBInterface.h"
#include "ResultUtil.h"
#include "ResultsTable.h"

class QueryEvaluator {
private:
	shared_ptr<QueryInterface> aQuery;
	shared_ptr<PKBInterface> aPKB;

	shared_ptr<ResultsTable> evaluateRelationshipClauses(vector<shared_ptr<RelationshipClause>> relationshipClauses, shared_ptr<ResultsTable> results);

	shared_ptr<ResultsTable> evaluatePatternClauses(vector<shared_ptr<PatternClause>> patternClauses, shared_ptr<ResultsTable> results);

	shared_ptr<ResultsTable> mergeResults(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);
	
public:
	
	QueryEvaluator(shared_ptr<QueryInterface> query, shared_ptr<PKBInterface> pkb);

	shared_ptr<ResultsTable> evaluate();
};