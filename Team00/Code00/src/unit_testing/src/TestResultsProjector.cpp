#pragma once
#include "ResultsProjector.h"
#include "TestResultsTableUtil.h"
#include "PKBStub.h"
#include "catch.hpp"

TEST_CASE("Projecting list of final results from query evaluator") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string whileSynonym = "w";

	PKBStub* pkb = new PKBStub();
	Declaration declaration = Declaration(EntityType::ASSIGN, assignSynonym);
	SelectClause* selectClause = new SelectClause(&declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	
	SECTION("Select synonym in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		ResultsTable* resultsTable = new ResultsTable();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "22", "33", "44" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	SECTION("Select synonym not in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {whileSynonym, 0}, {stmtSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		ResultsTable* resultsTable = new ResultsTable();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "1", "2", "3", "4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	SECTION("Select synonym in PKB results, some clause unfulfilled") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		ResultsTable* resultsTable = new ResultsTable();
		resultsTable->setTable(indexMap, table);
		resultsTable->setIsNoResult();
		list<string> expectedList = {};
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
}