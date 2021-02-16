#include "QueryEvaluator.h"
#include "Any.h"
#include "StmtNum.h"
#include "Query.h"
#include "TestResultsTableUtil.h"
#include "PKBStub.h"
#include "catch.hpp"

TEST_CASE("Evaluating query with select clause only, No optional clauses") {
	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);
	shared_ptr<ResultsTable> resultsTable = qe.evaluate();
	unordered_map<string, int> indexMap = resultsTable->getSynonymIndexMap();
	vector<vector<string>> table = resultsTable->getTableValues();
	REQUIRE(indexMap.size() == 0);
	REQUIRE(table.size() == 0);
}

TEST_CASE("Evaluating query with only one relationship clause") {
	string stmtSynonym = "s";
	string assignSynonym = "a";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);

	SECTION("relationship clause has 2 synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, assign);
		
		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };
		
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, wildcard);

		unordered_map<string, set<string>> pkbRsResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {stmtSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	SECTION("relationship clause has no synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> eight = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(8));
		shared_ptr<QueryInput> seven = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(7));
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, seven, eight);

		unordered_map<string, set<string>> wrongResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfRSReturnValue(wrongResult);
		pkb->setGetBooleanResultOfRSReturnValue(true);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("relationship clause has 2 synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, assign);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, wildcard);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfRSReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	SECTION("relationship clause has no synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> eight = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(8));
		shared_ptr<QueryInput> seven = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(7));
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, seven, eight);

		unordered_map<string, set<string>> wrongResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfRSReturnValue(wrongResult);
		pkb->setGetBooleanResultOfRSReturnValue(false);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
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

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);
	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));

	SECTION("Pattern clause has synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expression = make_shared<Expression>("x");
		query->addPatternClause(assign, var, expression);

		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {varSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	SECTION("Pattren clause has non synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		shared_ptr<Expression> expression = make_shared<Expression>("_");
		query->addPatternClause(assign, wildcard, expression);

		unordered_map<string, set<string>> pkbRsResult = { { "dummy", {"11", "12", "13", "14", "22", "23", "24"}} };
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = { {assignSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	SECTION("Pattern clause has synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expression = make_shared<Expression>("x");
		query->addPatternClause(assign, var, expression);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Pattren clause has non synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		shared_ptr<Expression> expression = make_shared<Expression>("_");
		query->addPatternClause(assign, wildcard, expression);

		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->setGetResultsOfPatternReturnValue(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
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

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	pkb->setGetEntitiesReturnValue({ "1", "2", "3", "4" });
	QueryEvaluator qe = QueryEvaluator(query, pkb);
	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));

	SECTION("Clauses have 2 common synonyms, all evaluates to non empty results") {
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("x");
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = { { assignSynonym, 0 }, { varSynonym, 1 } };
		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have 1 common synonym, all evaluates to non empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_");
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addPatternClause(assign, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {assignSynonym, 2} };
		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have no common synonym, all evaluates to non empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_");
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addPatternClause(assign, var, expr);
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
		

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have 2 common synonyms, rs clause evaluates to empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_");
		query->addPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	SECTION("Clauses have 2 common synonyms, pattern clause evaluates to empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->setGetResultsOfRSReturnValue(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_");
		query->addPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->setGetResultsOfPatternReturnValue(patternClauseResult);

		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}
}