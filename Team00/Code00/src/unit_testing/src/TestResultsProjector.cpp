#pragma once
#include "ResultsProjector.h"
#include "TestResultsTableUtil.h"
#include "PKBStub.h"
#include "catch.hpp"

TEST_CASE("Projecting list of final results from query evaluator") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string whileSynonym = "w";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	
	SECTION("Select synonym in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "22", "33", "44" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	SECTION("Select synonym not in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {whileSynonym, 0}, {stmtSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "1", "2", "3", "4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	SECTION("Select synonym in PKB results, some clause unfulfilled") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		resultsTable->setIsNoResult();
		list<string> expectedList = {};
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
}