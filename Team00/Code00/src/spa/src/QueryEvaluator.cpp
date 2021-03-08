#include "QueryEvaluator.h"
#include "QueryInputType.h"

QueryEvaluator::QueryEvaluator(shared_ptr<QueryInterface> query, shared_ptr<PKBInterface> pkb) {
	this->aQuery = query;
	this->aPKB = pkb;
}

shared_ptr<ResultsTable> QueryEvaluator::evaluate() {
	vector<shared_ptr<RelationshipClause>> relationshipClauses = aQuery->getRelationshipClauses();
	vector<shared_ptr<PatternClause>> patternClauses = aQuery->getPatternClauses();
	shared_ptr<ResultsTable> currentResults = make_shared<ResultsTable>();

	if (relationshipClauses.size() != 0) {
		currentResults = evaluateRelationshipClauses(relationshipClauses, currentResults);
		if (currentResults->isNoResult()) { // at least one clause does not have any results, no need to continue evaluating
			return currentResults;
		}
	}

	if (patternClauses.size() != 0) {
		currentResults = evaluatePatternClauses(patternClauses, currentResults);
		if (currentResults->isNoResult()) { // at least one clause does not have any results, no need to continue evaluating
			return currentResults;
		}
	}

	return currentResults;
}

shared_ptr<ResultsTable> QueryEvaluator::evaluateRelationshipClauses(vector<shared_ptr<RelationshipClause>> relationshipClauses, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;

	for (vector<shared_ptr<RelationshipClause>>::iterator iterator = relationshipClauses.begin();
		iterator != relationshipClauses.end(); iterator++) {
		shared_ptr<RelationshipClause> relationshipClause = *iterator;
		shared_ptr<QueryInput> leftQueryInput = relationshipClause->getLeftInput();
		shared_ptr<QueryInput> rightQueryInput = relationshipClause->getRightInput();

		// None of query inputs are delcarations
		if (leftQueryInput->getQueryInputType() != QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() != QueryInputType::DECLARATION) {
			bool hasResults = aPKB->getBooleanResultOfRS(relationshipClause->getRelationshipType(),
				leftQueryInput, rightQueryInput);
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
			leftQueryInput, rightQueryInput);


		// Both query inputs are declarations
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {

			if (PKBResults.size() == 0) {
				currentResults->setIsNoResult();
				return currentResults;
			}

			string leftSynonym = leftQueryInput->getValue();
			string rightSynonym = rightQueryInput->getValue();
			currentResults = mergeResults(PKBResults, {leftSynonym, rightSynonym}, currentResults);
			continue;
		}

		// only one query input can be declaration
		if (PKBResults.find("")->second.size() == 0) {
			currentResults->setIsNoResult();
			return currentResults;
		}

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

shared_ptr<ResultsTable> QueryEvaluator::evaluatePatternClauses(vector<shared_ptr<PatternClause>> patternClauses, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;

	for (vector<shared_ptr<PatternClause>>::iterator iterator = patternClauses.begin();
		iterator != patternClauses.end(); iterator++) {
		shared_ptr<PatternClause> patternClause = *iterator;
		shared_ptr<Declaration> synonym = dynamic_pointer_cast<Declaration>(patternClause->getSynonym());
		shared_ptr<QueryInput> queryInput = patternClause->getQueryInput();
		shared_ptr<Expression> expression = patternClause->getExpression();

		unordered_map<string, set<string>> PKBResults = aPKB->getResultsOfPattern(synonym->getEntityType(), queryInput, *expression);

		

		vector<string> synonyms = { synonym->getValue() };
		if (queryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			if (PKBResults.size() == 0) {
				currentResults->setIsNoResult();
				return currentResults;
			}

			synonyms.push_back(queryInput->getValue());
		}

		if (PKBResults.find("")->second.size() == 0) {
			currentResults->setIsNoResult();
			return currentResults;
		}

		currentResults = mergeResults(PKBResults, synonyms, currentResults);
	}

	return currentResults;
}

// PKBResult is assumed to be non empty here - QE must check if results from PKB are empty before merging
// only case when currentResult is empty is when mergining the first PKBResult
shared_ptr<ResultsTable> QueryEvaluator::mergeResults(unordered_map <string, set<string>> PKBResults, vector<string> synonyms, shared_ptr<ResultsTable> currentResults) {
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