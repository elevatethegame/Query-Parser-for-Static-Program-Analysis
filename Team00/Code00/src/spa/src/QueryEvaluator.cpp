#include "QueryEvaluator.h"
#include "QueryInputType.h"

QueryEvaluator::QueryEvaluator(Query* query, PKBInterface* pkb) {
	this->aQuery = query;
	this->aPKB = pkb;
}

ResultsTable* QueryEvaluator::evaluate() {
	vector<RelationshipClause*>* relationshipClauses = aQuery->getRelationshipClauses();
	vector<PatternClause*>* patternClauses = aQuery->getPatternClauses();
	ResultsTable* currentResults = new ResultsTable();

	if (relationshipClauses->size() != 0) {
		currentResults = evaluateRelationshipClauses(relationshipClauses, currentResults);
		if (currentResults->isNoResult()) { // at least one clause does not have any results, no need to continue evaluating
			return currentResults;
		}
	}

	if (patternClauses->size() != 0) {
		currentResults = evaluatePatternClauses(patternClauses, currentResults);
		if (currentResults->isNoResult()) { // at least one clause does not have any results, no need to continue evaluating
			return currentResults;
		}
	}

	return currentResults;
}

ResultsTable* QueryEvaluator::evaluateRelationshipClauses(vector<RelationshipClause*>* relationshipClauses, ResultsTable* results) {
	ResultsTable* currentResults = results;

	for (vector<RelationshipClause*>::iterator iterator = relationshipClauses->begin();
		iterator != relationshipClauses->end(); iterator++) {
		RelationshipClause* relationshipClause = *iterator;
		QueryInput* leftQueryInput = relationshipClause->getLeftInput();
		QueryInput* rightQueryInput = relationshipClause->getRightInput();

		// None of query inputs are delcarations
		if (leftQueryInput->getQueryInputType() != QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() != QueryInputType::DECLARATION) {
			bool hasResults = aPKB->getBooleanResultOfRS(relationshipClause->getRelationshipType(),
				*leftQueryInput, *rightQueryInput);
			if (!hasResults) {
				// clause returns no results, no need to further evaluate
				currentResults->setIsNoResult();
				return currentResults;
			}
			else {
				continue; // clause is fulfilled, skip over to next clause
			}
		}

		unordered_map<string, set<string>> PKBResults = aPKB->getResultsOfRS(relationshipClause->getRelationshipType(),
			*leftQueryInput, *rightQueryInput);

		if (PKBResults.size() == 0) {
			currentResults->setIsNoResult();
			return currentResults;
		}

		// Both query inputs are declarations
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {

			string leftSynonym = leftQueryInput->getValue();
			string rightSynonym = rightQueryInput->getValue();
			currentResults = mergeResults(PKBResults, {leftSynonym, rightSynonym}, currentResults);
			continue;
		}

		// only one query input can be declaration
		string synonym;
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			synonym = leftQueryInput->getValue();
		}
		else {
			synonym = rightQueryInput->getValue();
		}

		currentResults = mergeResults(PKBResults, { synonym }, currentResults);

	}

	return currentResults;
}

ResultsTable* QueryEvaluator::evaluatePatternClauses(vector<PatternClause*>* patternClauses, ResultsTable* results) {
	ResultsTable* currentResults = results;

	for (vector<PatternClause*>::iterator iterator = patternClauses->begin();
		iterator != patternClauses->end(); iterator++) {
		PatternClause* patternClause = *iterator;
		Declaration* synonym = (Declaration*) patternClause->getSynonym();
		QueryInput* queryInput = patternClause->getQueryInput();
		Expression* expression = patternClause->getExpression();

		unordered_map<string, set<string>> PKBResults = aPKB->getResultsOfPattern(synonym->getEntityType(), *queryInput, *expression);

		if (PKBResults.size() == 0) {
			currentResults->setIsNoResult();
			return currentResults;
		}

		vector<string> synonyms = { synonym->getValue() };
		if (queryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			synonyms.push_back(queryInput->getValue());
		}

		currentResults = mergeResults(PKBResults, synonyms, currentResults);
	}

	return currentResults;
}

// PKBResult is assumed to be non empty here - QE must check if results from PKB are empty before merging
// only case when currentResult is empty is when mergining the first PKBResult
ResultsTable* QueryEvaluator::mergeResults(unordered_map <string, set<string>> PKBResults, vector<string> synonyms, ResultsTable* currentResults) {
	if (currentResults->isTableEmpty()) {
		currentResults->populate(PKBResults, synonyms);
		return currentResults;
	}

	set<string> commonSynonyms = ResultUtil::getCommonSynonyms(synonyms, currentResults->getSynonyms());
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProduct(PKBResults, synonyms, currentResults);
	}
	else {
		return ResultUtil::getNaturalJoin(PKBResults, synonyms, currentResults, commonSynonyms);
	}
}