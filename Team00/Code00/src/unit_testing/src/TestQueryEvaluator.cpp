#include "QueryEvaluator.h"
#include "Any.h"
#include "StmtNum.h"
#include "TestResultsTableUtil.h"
#include "PKBStub.h"
#include "catch.hpp"

TEST_CASE("Evaluating query with select clause only, No optional clauses") {
	PKBStub* pkb = new PKBStub();
	Query* query = new Query();
	Declaration declaration = Declaration(EntityType::ASSIGN, "a");
	query->setSelectClause(&declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);
	ResultsTable* resultsTable = qe.evaluate();
	unordered_map<string, int> indexMap = resultsTable->getSynonymIndexMap();
	vector<vector<string>> table = resultsTable->getTableValues();
	REQUIRE(indexMap.size() == 0);
	REQUIRE(table.size() == 0);
}

TEST_CASE("Evaluating query with only one relationship clause") {
	string stmtSynonym = "s";
	string assignSynonym = "a";

	PKBStub* pkb = new PKBStub();
	Query* query = new Query();
	Declaration declaration = Declaration(EntityType::ASSIGN, "a");
	query->setSelectClause(&declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);

	SECTION("relationship clause has 2 synonym input, evaluates to non empty results") {
		QueryInput stmt = Declaration(EntityType::STMT, stmtSynonym);
		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		query->addRelationshipClause(RelationshipType::FOLLOWS, &stmt, &assign);
		
		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };
		
		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to non empty results") {
		QueryInput stmt = Declaration(EntityType::STMT, stmtSynonym);
		QueryInput wildchard = Any("_");
		query->addRelationshipClause(RelationshipType::FOLLOWS, &stmt, &wildchard);

		unordered_map<string, set<string>> pkbRsResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {stmtSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	SECTION("relationship clause has no synonym input, evaluates to non empty results") {
		QueryInput eight = StmtNum(8);
		QueryInput seven = StmtNum(7);
		query->addRelationshipClause(RelationshipType::FOLLOWS_S, &seven, &eight);

		unordered_map<string, set<string>> wrongResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfRSReturnValue(wrongResult);
		pkb->setGetBooleanResultOfRSReturnValue(true);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("relationship clause has 2 synonym input, evaluates to empty results") {
		QueryInput stmt = Declaration(EntityType::STMT, stmtSynonym);
		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		query->addRelationshipClause(RelationshipType::FOLLOWS, &stmt, &assign);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to empty results") {
		QueryInput stmt = Declaration(EntityType::STMT, stmtSynonym);
		QueryInput wildchard = Any("_");
		query->addRelationshipClause(RelationshipType::FOLLOWS, &stmt, &wildchard);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	SECTION("relationship clause has no synonym input, evaluates to empty results") {
		QueryInput eight = StmtNum(8);
		QueryInput seven = StmtNum(7);
		query->addRelationshipClause(RelationshipType::FOLLOWS_S, &seven, &eight);

		unordered_map<string, set<string>> wrongResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfRSReturnValue(wrongResult);
		pkb->setGetBooleanResultOfRSReturnValue(false);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
}

TEST_CASE("Evaluating query with only one pattern clause") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string ifSynonym = "if";

	PKBStub* pkb = new PKBStub();
	Query* query = new Query();
	Declaration declaration = Declaration(EntityType::ASSIGN, "a");
	query->setSelectClause(&declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);
	QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);

	SECTION("Pattern clause has synonym input, evaluates to non empty results") {
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		Expression expression = Expression("x");
		query->addPatternClause(&assign, &var, &expression);

		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {varSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	SECTION("Pattren clause has non synonym input, evaluates to non empty results") {
		QueryInput wildchard = Any("_");
		Expression expression = Expression("_");
		query->addPatternClause(&assign, &wildchard, &expression);

		unordered_map<string, set<string>> pkbRsResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {assignSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	SECTION("Pattern clause has synonym input, evaluates to empty results") {
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		Expression expression = Expression("x");
		query->addPatternClause(&assign, &var, &expression);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Pattren clause has non synonym input, evaluates to empty results") {
		QueryInput wildchard = Any("_");
		Expression expression = Expression("_");
		query->addPatternClause(&assign, &wildchard, &expression);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
}

TEST_CASE("Evaluating query with both relationship and pattern clause") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string ifSynonym = "if";

	PKBStub* pkb = new PKBStub();
	Query* query = new Query();
	Declaration declaration = Declaration(EntityType::ASSIGN, "a");
	query->setSelectClause(&declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);
	QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);

	SECTION("Clauses have 2 common synonyms, all evaluates to non empty results") {
		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		query->addRelationshipClause(RelationshipType::MODIFIES, &assign, &var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		QueryInput assign2 = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var2 = Declaration(EntityType::VAR, varSynonym);
		Expression expr = Expression("_");
		query->addPatternClause(&assign2, &var2, &expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = { { assignSynonym, 0 }, { varSynonym, 1 } };
		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have 1 common synonym, all evaluates to non empty results") {
		QueryInput stmt = Declaration(EntityType::STMT, stmtSynonym);
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		query->addRelationshipClause(RelationshipType::USES, &stmt, &var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var2 = Declaration(EntityType::VAR, varSynonym);
		Expression expr = Expression("_");
		query->addPatternClause(&assign, &var2, &expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {assignSynonym, 2} };
		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have no common synonym, all evaluates to non empty results") {
		QueryInput stmt = Declaration(EntityType::STMT, stmtSynonym);
		QueryInput wh = Declaration(EntityType::WHILE, whileSynonym);
		query->addRelationshipClause(RelationshipType::PARENT_S, &stmt, &wh);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		Expression expr = Expression("_");
		query->addPatternClause(&assign, &var, &expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { whileSynonym, 1 }, {assignSynonym, 2}, 
			{varSynonym, 3} };
		vector<vector<string>> expectedTable = { 
			{ "1", "12", "1", "u" }, { "1", "13", "1", "u" },
			{ "24", "25", "1", "u" }, { "24", "26", "1", "u" },
			{ "1", "12", "1", "x1" }, { "1", "13", "1", "x1" },
			{ "24", "25", "1", "x1" }, { "24", "26", "1", "x1" },
			{ "1", "12", "3", "count" }, { "1", "13", "3", "count" },
			{ "24", "25", "3", "count" }, { "24", "26", "3", "count" },
			{ "1", "12", "3", "p" }, { "1", "13", "3", "p" },
			{ "24", "25", "3", "p" }, { "24", "26", "3", "p" } };
		

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have 2 common synonyms, rs clause evaluates to empty results") {
		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		query->addRelationshipClause(RelationshipType::MODIFIES, &assign, &var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		QueryInput assign2 = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var2 = Declaration(EntityType::VAR, varSynonym);
		Expression expr = Expression("_");
		query->addPatternClause(&assign2, &var2, &expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have 2 common synonyms, pattern clause evaluates to empty results") {
		QueryInput assign = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var = Declaration(EntityType::VAR, varSynonym);
		query->addRelationshipClause(RelationshipType::MODIFIES, &assign, &var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		QueryInput assign2 = Declaration(EntityType::ASSIGN, assignSynonym);
		QueryInput var2 = Declaration(EntityType::VAR, varSynonym);
		Expression expr = Expression("_");
		query->addPatternClause(&assign2, &var2, &expr);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		ResultsTable* resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}
}