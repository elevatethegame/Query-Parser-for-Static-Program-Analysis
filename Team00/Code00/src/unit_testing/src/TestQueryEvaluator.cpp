#include "QueryEvaluator.h"
#include "Any.h"
#include "StmtNum.h"
#include "Query.h"
#include "TestResultsTableUtil.h"
#include "PKBStub.h"
#include "catch.hpp"

// Select a
TEST_CASE("Evaluating query with select clause only, No optional clauses") {
	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	pkb->addSetResult({ "1", "2", "3", "4" });
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

	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	
	//Select a such that Follows(s, a)
	SECTION("relationship clause has 2 synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, assign);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };
		
		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Follows(s, _)
	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, wildcard);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {stmtSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Follows(7, 8)
	SECTION("relationship clause has no synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> eight = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("8"));
		shared_ptr<QueryInput> seven = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("7"));
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, seven, eight);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> wrongResult = { "11", "12", "13", "14", "22", "23", "24" };
		pkb->addSetResult(wrongResult);
		pkb->addBooleanResult(true);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Follows(s, a)
	SECTION("relationship clause has 2 synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, assign);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Follows(s, _)
	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, wildcard);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = {};
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Follows*(7, 8)
	SECTION("relationship clause has no synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> eight = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("8"));
		shared_ptr<QueryInput> seven = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("7"));
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, seven, eight);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> wrongResult = {"11", "12", "13", "14", "22", "23", "24"};
		pkb->addSetResult(wrongResult);
		pkb->addBooleanResult(false);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
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

	
	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
	shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
	shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));

	//Select a pattern a(v, _x_)
	SECTION("Assign pattern clause has synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expression = make_shared<Expression>("x", ExpressionType::EXACT);
		query->addAssignPatternClause(assign, var, expression);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {varSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	//Select a pattern a(_, _)
	SECTION("Assign pattern clause has non synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		shared_ptr<Expression> expression = make_shared<Expression>("_", ExpressionType::EMPTY);
		query->addAssignPatternClause(assign, wildcard, expression);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {assignSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	//Select a pattern a(v, _x_)
	SECTION("Assign pattern clause has synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expression = make_shared<Expression>("x", ExpressionType::EXACT);
		query->addAssignPatternClause(assign, var, expression);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a pattern a(_, _)
	SECTION("Assign pattern clause has non synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		shared_ptr<Expression> expression = make_shared<Expression>("_", ExpressionType::EMPTY);
		query->addAssignPatternClause(assign, wildcard, expression);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = {};
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	//Select w pattern w(v, _)
	SECTION("While pattern clause has synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addContainerPatternClause(synWhile, var);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {whileSynonym, 0}, {varSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w pattern w(_, _)
	SECTION("While pattern clause has non synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addContainerPatternClause(synWhile, wildcard);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {whileSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w pattern w(v, _)
	SECTION("While pattern clause has synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addContainerPatternClause(synWhile, var);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w pattern w(_, _)
	SECTION("While pattern clause has non synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addContainerPatternClause(synWhile, wildcard);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = {};
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs pattern ifs(v, _, _)
	SECTION("If pattern clause has synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addContainerPatternClause(synIf, var);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {ifSynonym, 0}, {varSynonym, 1} };
		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs pattern ifs(_, _)
	SECTION("If pattern clause has non synonym input, evaluates to non empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addContainerPatternClause(synIf, wildcard);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = { {ifSynonym, 0} };
		vector<vector<string>> expectedTable = { {"11"}, {"12"}, {"13"}, {"14"}, {"22"}, {"23"}, {"24"} };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs pattern ifs(v, _)
	SECTION("If pattern clause has synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addContainerPatternClause(synIf, var);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		unordered_map<string, set<string>> pkbRsResult = {};
		pkb->addMapResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs pattern ifs(_, _)
	SECTION("If pattern clause has non synonym input, evaluates to empty results") {
		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
		query->addContainerPatternClause(synIf, wildcard);

		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
		set<string> pkbRsResult = {};
		pkb->addSetResult(pkbRsResult);
		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
}

TEST_CASE("Evaluating query with both such that and pattern clause") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string ifSynonym = "if";

	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));

	//Select a such that Modifies(a, v) pattern a(v, _x_)
	SECTION("such that and assign pattern clauses have 2 common synonyms, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { assignSynonym, 0 }, { varSynonym, 1 } };
		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Uses(s, v) pattern a(v, _)
	SECTION("such that and assign pattern clauses have 1 common synonym, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {assignSynonym, 2} };
		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Parent*(s, w) pattern a(v, _)
	SECTION("such that and assign pattern clauses have no common synonym, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign, var, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
		pkb->addMapResult(patternClauseResult);

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
		
		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Modifies(a, v) pattern a(v, _)
	SECTION("such that and assign pattern clauses have 2 common synonyms, rs clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Modifies(a, v) pattern a(v, _)
	SECTION("such that and assign pattern clauses have 2 common synonyms, pattern clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}

	//Select a such that Uses(s, v) pattern a(v, _)
	SECTION("such that and assign pattern clauses have 1 common synonym, rs clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Uses(s, v) pattern a(v, _)
	SECTION("such that and assign pattern clauses have 1 common synonym, pattern clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}
	
	//Select w such that Modifies(w, v) pattern w(v, _)
	SECTION("such that and while pattern clauses have 2 common synonyms, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, synWhile, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile2, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { whileSynonym, 0 }, { varSynonym, 1 } };
		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	//Select w such that Uses(s, v) pattern w(v, _)
	SECTION("such that and while pattern clauses have 1 common synonym, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {whileSynonym, 2} };
		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w such that Parent*(ifs, s) pattern w(v, _)
	SECTION("such that and while pattern clauses have no common synonym, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		query->addRelationshipClause(RelationshipType::PARENT_T, synIf, stmt);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile, var);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { ifSynonym, 0 }, { stmtSynonym, 1 }, {whileSynonym, 2},
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

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w such that Modifies(w, v) pattern w(v, _)
	SECTION("such that and while pattern clauses have 2 common synonyms, rs clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, synWhile, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile2, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w such that Modifies(w, v) pattern w(v, _)
	SECTION("such that and while pattern clauses have 2 common synonyms, pattern clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, synWhile, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile2, var2);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}

	//Select w such that Uses(s, v) pattern w(v, _)
	SECTION("such that and while pattern clauses have 1 common synonym, rs clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select w such that Uses(s, v) pattern w(v, _)
	SECTION("such that and while pattern clauses have 1 common synonym, pattern clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile, var2);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}
	
	//Select ifs such that Modifies(ifs, v) pattern ifs(v, _, _)
	SECTION("such that and if pattern clauses have 2 common synonyms, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, synIf, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf2, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { ifSynonym, 0 }, { varSynonym, 1 } };
		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs such that Uses(s, v) pattern ifs(v, _)
	SECTION("such that and if pattern clauses have 1 common synonym, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {ifSynonym, 2} };
		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs such that Parent*(w, s) pattern ifs(v, _)
	SECTION("such that and if pattern clauses have no common synonym, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		query->addRelationshipClause(RelationshipType::PARENT_T, synWhile, stmt);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf, var);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { whileSynonym, 0 }, { stmtSynonym, 1 }, {ifSynonym, 2},
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

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs such that Modifies(ifs, v) pattern ifs(v, _)
	SECTION("such that and if pattern clauses have 2 common synonyms, rs clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, synIf, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf2, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs such that Modifies(ifs, v) pattern ifs(v, _)
	SECTION("such that and if pattern clauses have 2 common synonyms, pattern clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, synIf, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf2, var2);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}

	//Select ifs such that Uses(s, v) pattern ifs(v, _)
	SECTION("such that and if pattern clauses have 1 common synonym, rs clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = {};
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf, var2);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select ifs such that Uses(s, v) pattern ifs(v, _)
	SECTION("such that and if pattern clauses have 1 common synonym, pattern clause evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::USES, stmt, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf, var2);
		unordered_map<string, set<string>> patternClauseResult = {};
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}
}

TEST_CASE("Evaluating query with multiple such that and pattern clauses") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string ifSynonym = "if";
	string progLine1Synonym = "n1";
	string progLine2Synonym = "n2";

	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
	query->setSelectClause(declaration);
	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));

	//Select a such that Modifies(a, v) such that Parent(s, a) pattern a(v, _x_)
	SECTION("2 such that and 1 assign pattern clauses, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} }, { "8", {"y"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> assign1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		query->addRelationshipClause(RelationshipType::PARENT, stmt, assign1);
		unordered_map<string, set<string>> rsClauseResult2 = { { "2", {"3"} }, { "4", {"5","6"} }, { "7", {"8","10"} } };
		pkb->addMapResult(rsClauseResult2);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} }, { "8", {"x", "y"} } };
		pkb->addMapResult(patternClauseResult);

		unordered_map<string, int> expectedMap = { { assignSynonym, 0 }, { varSynonym, 1 }, { stmtSynonym, 2 } };
		vector<vector<string>> expectedTable = { { "3", "count", "2" }, { "8", "y", "7" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
	
	//Select a such that Modifies(a, v) pattern a(v, _x_) pattern w(v, _)
	SECTION("1 such that and 2 pattern clauses, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		shared_ptr<QueryInput> var3 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile, var3);
		unordered_map<string, set<string>> patternClauseResult2 = { { "7", {"u1", "u", "x"} }, { "20", {"i"} }, { "10", {"count"} } };
		pkb->addMapResult(patternClauseResult2);

		unordered_map<string, int> expectedMap = { { assignSynonym, 0 }, { varSynonym, 1 }, { whileSynonym, 2 } };
		vector<vector<string>> expectedTable = { { "1", "u", "7" }, { "3", "count", "10" } };

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Next(1, 2) such that Parent*(s, w) pattern a(v, _) pattern ifs(v, _, _)
	SECTION("2 such that and 2 pattern clauses, all evaluates to non empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> progLine1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROG_LINE, progLine1Synonym));
		shared_ptr<QueryInput> progLine2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROG_LINE, progLine2Synonym));
		query->addRelationshipClause(RelationshipType::NEXT, progLine1, progLine2);
		pkb->addBooleanResult(true);

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign, var, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
		pkb->addMapResult(patternClauseResult);

		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf, var1);
		unordered_map<string, set<string>> patternClauseResult2 = { { "30", {"count", "y"} }, { "38", {"x1", "u"} }, { "40", {"x3", "u3"} } };
		pkb->addMapResult(patternClauseResult2);

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { whileSynonym, 1 }, {assignSynonym, 2},
			{varSynonym, 3}, { ifSynonym, 4} };
		vector<vector<string>> expectedTable = {
			{ "1", "12", "1", "u", "38" }, { "1", "13", "1", "u", "38" },
			{ "24", "25", "1", "u", "38" }, { "24", "26", "1", "u", "38" },
			{ "1", "12", "1", "x1", "38" }, { "1", "13", "1", "x1", "38" },
			{ "24", "25", "1", "x1", "38" }, { "24", "26", "1", "x1", "38" },
			{ "1", "12", "3", "count", "30" }, { "1", "13", "3", "count", "30" },
			{ "24", "25", "3", "count", "30" }, { "24", "26", "3", "count", "30" } };
		
		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(!resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

	//Select a such that Modifies(a, v) such that Parent(s, a) pattern a(v, _x_)
	SECTION("2 such that and 1 assign pattern clauses, evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} }, { "8", {"y"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> assign1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		query->addRelationshipClause(RelationshipType::PARENT, stmt, assign1);
		unordered_map<string, set<string>> rsClauseResult2 = {};
		pkb->addMapResult(rsClauseResult2);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} }, { "8", {"x", "y"} } };
		pkb->addMapResult(patternClauseResult);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}

	//Select a such that Modifies(a, v) pattern a(v, _x_) pattern w(v, _)
	SECTION("1 such that and 2 pattern clauses, evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
		shared_ptr<QueryInput> assign2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign2, var2, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
		pkb->addMapResult(patternClauseResult);

		shared_ptr<QueryInput> var3 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addContainerPatternClause(synWhile, var3);
		unordered_map<string, set<string>> patternClauseResult2 = {};
		pkb->addMapResult(patternClauseResult2);

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
	}

	//Select a such that Next(1, 2) such that Parent*(s, w) pattern a(v, _) pattern ifs(v, _, _)
	SECTION("2 such that and 2 pattern clauses, evaluates to empty results") {
		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

		shared_ptr<QueryInput> progLine1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROG_LINE, progLine1Synonym));
		shared_ptr<QueryInput> progLine2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROG_LINE, progLine2Synonym));
		query->addRelationshipClause(RelationshipType::NEXT, progLine1, progLine2);
		pkb->addBooleanResult(false);

		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
		unordered_map<string, set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
		pkb->addMapResult(rsClauseResult);

		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
		query->addAssignPatternClause(assign, var, expr);
		unordered_map<string, set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
		pkb->addMapResult(patternClauseResult);

		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
		query->addContainerPatternClause(synIf, var1);
		unordered_map<string, set<string>> patternClauseResult2 = { { "30", {"count", "y"} }, { "38", {"x1", "u"} }, { "40", {"x3", "u3"} } };
		pkb->addMapResult(patternClauseResult2);

		unordered_map<string, int> expectedMap = {};
		vector<vector<string>> expectedTable = {};

		QueryEvaluator qe = QueryEvaluator(query, pkb);
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		REQUIRE(resultsTable->isNoResult());
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();
		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}
}