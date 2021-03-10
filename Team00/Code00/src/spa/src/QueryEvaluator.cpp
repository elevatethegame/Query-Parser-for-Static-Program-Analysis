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
		RelationshipType relationshipType = relationshipClause->getRelationshipType();

		// None of query inputs are declarations
		if ((leftQueryInput->getQueryInputType() != QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() != QueryInputType::DECLARATION) || 
			relationshipType == RelationshipType::NEXT || relationshipType == RelationshipType::NEXT_T) {
			bool hasResults = aPKB->getBooleanResultOfRS(relationshipType, leftQueryInput, rightQueryInput);
			if (!hasResults) {
				// clause returns no results, no need to further evaluate
				currentResults->setIsNoResult();
				return currentResults;
			}
			else {
				continue; // clause is fulfilled, skip over to next clause
			}
		}

		// Both query inputs are declarations
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {

			unordered_map<string, set<string>> PKBResults = aPKB->getMapResultsOfRS(relationshipType, leftQueryInput, rightQueryInput);

			if (PKBResults.size() == 0) {
				currentResults->setIsNoResult();
				return currentResults;
			}

			string leftSynonym = leftQueryInput->getValue();
			string rightSynonym = rightQueryInput->getValue();
			currentResults = mergeMapResults(PKBResults, {leftSynonym, rightSynonym}, currentResults);
			continue;
		}

		// only one query input can be declaration
		set<string> PKBResults = aPKB->getSetResultsOfRS(relationshipClause->getRelationshipType(),
			leftQueryInput, rightQueryInput);

		if (PKBResults.size() == 0) {
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

		currentResults = mergeSetResults(PKBResults, synonym, currentResults);

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
		shared_ptr<Expression> expression;
		
		if (queryInput->getQueryInputType() == QueryInputType::DECLARATION) { // 2 synonyms in pattern clause
			unordered_map<string, set<string>> PKBResults;
			switch (synonym->getEntityType()) {
			case EntityType::ASSIGN:
				expression = patternClause->getExpression();
				PKBResults = aPKB->getMapResultsOfAssignPattern(queryInput, *expression);
				break;

			case EntityType::WHILE:
			case EntityType::IF:
				PKBResults = aPKB->getMapResultsOfContainerPattern(synonym->getEntityType(), queryInput);
				break;

			default:
				break;
			}

			if (PKBResults.size() == 0) {
				currentResults->setIsNoResult();
				return currentResults;
			}

			currentResults = mergeMapResults(PKBResults, { synonym->getValue(), queryInput->getValue() }, currentResults);

		}
		else { // else first argument of clause is not a synonym: clause has only 1 synonym
			set<string> PKBResults;
			switch (synonym->getEntityType()) {
			case EntityType::ASSIGN:
				expression = patternClause->getExpression();
				PKBResults = aPKB->getSetResultsOfAssignPattern(queryInput, *expression);
				break;

			case EntityType::WHILE:
			case EntityType::IF:
				PKBResults = aPKB->getSetResultsOfContainerPattern(synonym->getEntityType(), queryInput);
				break;

			default:
				break;
			}

			if (PKBResults.size() == 0) {
				currentResults->setIsNoResult();
				return currentResults;
			}

			currentResults = mergeSetResults(PKBResults, synonym->getValue(), currentResults);
		}
	}

	return currentResults;
}

// PKBResult is assumed to be non empty here - QE must check if results from PKB are empty before merging
// only case when currentResult is empty is when mergining the first PKBResult
shared_ptr<ResultsTable> QueryEvaluator::mergeMapResults(unordered_map <string, set<string>> PKBResults, vector<string> synonyms, shared_ptr<ResultsTable> currentResults) {
	if (currentResults->isTableEmpty()) {
		currentResults->populateWithMap(PKBResults, synonyms);
		return currentResults;
	}

	set<string> commonSynonyms = ResultUtil::getCommonSynonyms(synonyms, currentResults->getSynonyms());
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProductFromMap(PKBResults, synonyms, currentResults);
	}
	else {
		return ResultUtil::getNaturalJoinFromMap(PKBResults, synonyms, currentResults, commonSynonyms);
	}
}

shared_ptr<ResultsTable> QueryEvaluator::mergeSetResults(set<string> PKBResults, string synonym,
	shared_ptr<ResultsTable> currentResults) {
	if (currentResults->isTableEmpty()) {
		currentResults->populateWithSet(PKBResults, synonym);
		return currentResults;
	}

	set<string> commonSynonyms = ResultUtil::getCommonSynonyms({ synonym }, currentResults->getSynonyms());
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProductFromSet(PKBResults, synonym, currentResults);
	}
	else {
		return ResultUtil::getNaturalJoinFromSet(PKBResults, synonym, currentResults, commonSynonyms);
	}
}