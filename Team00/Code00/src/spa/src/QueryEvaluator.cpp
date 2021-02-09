#include "QueryEvaluator.h"
#include "QueryInputType.h"

QueryEvaluator::QueryEvaluator() {
	aQuery = new Query();
	aPKB = new PKB();
}

QueryEvaluator::~QueryEvaluator() {
	delete aQuery;
	delete aPKB;
}

// API to be used by query parser ############################################################################
void QueryEvaluator::setSelectClause(Declaration* declaration) {
	SelectClause* selectClause = new SelectClause(declaration);
	aQuery->setSelectClause(selectClause);
}

void QueryEvaluator::addRelationshipClause(RelationshipType relationshipType, QueryInput* leftQueryInput, QueryInput* rightQueryInput) {
	RelationshipClause* relationshipClause = new RelationshipClause(relationshipType, leftQueryInput, rightQueryInput);
	aQuery->addRelationshipClause(relationshipClause);
}

void QueryEvaluator::addPatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression) {
	PatternClause* patternClause = new PatternClause(synonym, queryInput, expression);
	aQuery->addPatternClause(patternClause);
}


// Evaluate functions ############################################################################
void QueryEvaluator::evaluate(list<string> results) {
	set<string> finalResults = evaluate();
	for (set<string>::iterator it = finalResults.begin(); it != finalResults.end(); it++) {
		results.push_back(*it);
	}
}


set<string> QueryEvaluator::evaluate() {
	vector<RelationshipClause*>* relationshipClauses = aQuery->getRelationshipClauses();
	vector<PatternClause*>* patternClauses = aQuery->getPatternClauses();
	vector<unordered_map<string, string>> mergedResults;
	set<string> finalResults;

	if (relationshipClauses->size() == 0 && patternClauses->size() == 0) {
		Declaration* selectClauseDeclaration = aQuery->getSelectClause()->getDeclaration();
		string selectClauseSynonym = selectClauseDeclaration->getValue();
		finalResults = aPKB->getEntities(selectClauseDeclaration->getEntityType());
		return finalResults;
	}

	if (relationshipClauses->size() != 0) {
		vector<unordered_map<string, string>> RSClauseResults = evaluateRelationshipClauses(relationshipClauses);
		if (RSClauseResults.size() == 0) { // at least one clause does not have any results, query should return none
			finalResults.clear();
			return finalResults;
		}
		else {
			mergedResults = mergeResults(mergedResults, RSClauseResults);
		}
	}

	if (patternClauses->size() != 0) {
		vector<unordered_map<string, string>> patternClauseResults = evaluatePatternClauses(patternClauses);
		if (patternClauseResults.size() == 0) { // at least one clause does not have any results, query should return none
			finalResults.clear();
			return finalResults;
		}
		else {
			mergedResults = mergeResults(mergedResults, patternClauseResults);
		}
	}

	finalResults = evaluateSelectClause(mergedResults);
	return finalResults;
}

vector<unordered_map<string, string>> QueryEvaluator::evaluateRelationshipClauses(vector<RelationshipClause*>* relationshipClauses) {
	vector<unordered_map<string, string>> results;

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
				// clause returns no results, return emptylist
				vector<unordered_map<string, string>> emptyResults;
				return emptyResults;
			}
			else {
				continue; // clause is fulfilled, skip over to next clause
			}
		}

		// Both query inputs are declarations
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
			rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			unordered_map<string, set<string>> resultFromPKB = aPKB->getResultsOfRS(relationshipClause->getRelationshipType(),
				*leftQueryInput, *rightQueryInput);
			vector<unordered_map<string, string>> convertedResults = ResultUtil::convertPKBResult(resultFromPKB, leftQueryInput->getValue(), rightQueryInput->getValue());
			results = mergeResults(results, convertedResults);
			continue;
		}

		// only one query input can be declaration
		unordered_map<string, set<string>> resultFromPKB = aPKB->getResultsOfRS(relationshipClause->getRelationshipType(),
			*leftQueryInput, *rightQueryInput);
		string synonym;
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			synonym = leftQueryInput->getValue();
		}
		else {
			synonym = rightQueryInput->getValue();
		}

		vector<unordered_map<string, string>> convertedResults = ResultUtil::convertPKBResult(resultFromPKB, synonym);
		results = mergeResults(results, convertedResults);

	}

	return results;
}

vector<unordered_map<string, string>> QueryEvaluator::evaluatePatternClauses(vector<PatternClause*>* patternClauses) {
	vector<unordered_map<string, string>> results;
	for (vector<PatternClause*>::iterator iterator = patternClauses->begin();
		iterator != patternClauses->end(); iterator++) {
		PatternClause* patternClause = *iterator;
		Declaration* synonym = (Declaration*) patternClause->getSynonym();
		QueryInput* queryInput = patternClause->getQueryInput();
		Expression* expression = patternClause->getExpression();

		unordered_map<string, set<string>> resultFromPKB = aPKB->getResultsOfPattern(synonym->getEntityType(), *queryInput, *expression);

		vector<unordered_map<string, string>> convertedResults;
		if (queryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			convertedResults = ResultUtil::convertPKBResult(resultFromPKB, synonym->getValue(), queryInput->getValue());
		}
		else {
			convertedResults = ResultUtil::convertPKBResult(resultFromPKB, synonym->getValue());
		}

		if (convertedResults.size() == 0) {
			results.clear();
			return results;
		}

		results = mergeResults(results, convertedResults);
	}

	return results;
}

set<string> QueryEvaluator::evaluateSelectClause(vector<unordered_map<string, string>> results) {
	set<string> finalResults;
	string synonym = aQuery->getSelectClause()->getDeclaration()->getValue();

	for (vector<unordered_map<string, string>>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
		unordered_map<string, string> currentMap = *iterator;
		if (currentMap.count(synonym) == 1) {
			finalResults.insert(currentMap.find(synonym)->second);
		}
	}
	
	return finalResults;
}

// Utility functions ############################################################################
vector<unordered_map<string, string>> QueryEvaluator::mergeResults(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult) {
	if (firstResult.size() == 0 && secondResult.size() == 0) {
		return firstResult;
	}

	if (firstResult.size() == 0) {
		return secondResult;
	}

	if (secondResult.size() == 0) {
		return firstResult;
	}

	set<string> commonSynonyms = ResultUtil::getCommonSynonyms(firstResult, secondResult);
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProduct(firstResult, secondResult);
	}
	else {
		return ResultUtil::getNaturalJoin(firstResult, secondResult, commonSynonyms);
	}
}