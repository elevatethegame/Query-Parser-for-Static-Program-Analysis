#include <memory>
#include <list>

#include "PKB.h"
#include "Query.h"
#include "QueryEvaluator.h"
#include "QueryInput.h"
#include "QueryInputType.h"
#include "StmtNum.h"
#include "Ident.h"
#include "Any.h"
#include "ResultsProjector.h"
#include "TestResultsTableUtil.h"
#include "catch.hpp"

TEST_CASE("testSuchThatClauseSuccess") {
	
	shared_ptr<PKB> pkb = make_shared<PKB>(10);
	
	// 1 2 3 4 {5 {6, 7} 8} 9, 10
	REQUIRE(pkb->insertFollow(1, 2));
	REQUIRE(pkb->insertFollow(2, 3));
	REQUIRE(pkb->insertFollow(3, 4));
	REQUIRE(pkb->insertParent(4, 5));
	REQUIRE(pkb->insertParent(5, 6));
	REQUIRE(pkb->insertParent(5, 7));
	REQUIRE(pkb->insertFollow(6, 7));
	REQUIRE(pkb->insertParent(4, 8));
	REQUIRE(pkb->insertFollow(9, 10));
	REQUIRE(pkb->insertDirectUses(3, set<string>{ "x" }));
	REQUIRE(pkb->insertExpression(3, "x"));
	REQUIRE(pkb->setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(4, EntityType::WHILE));
	REQUIRE(pkb->insertDirectModifies(8, "y"));
	REQUIRE(pkb->insertExpression(8, "y"));
	REQUIRE(pkb->setStatementType(8, EntityType::ASSIGN));

	pkb->init();

	SECTION("nonBooleanResultsFollowsT") {
		// select s such that follow*(1, s)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmt =
			dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
		shared_ptr<QueryInput> stmtNum =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(1));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmtNum, stmt);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "10", "2", "3", "4", "9" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsFollows") {
		// select s such that follows(s, 2)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmtNum2 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(2));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::FOLLOWS, declaration, stmtNum2);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "1" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsParent") {
		// select s such that Parent(5, s)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmtNum5 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(5));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::PARENT, stmtNum5, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "6", "7" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsParentT") {
		// select s such that Parent*(4, s)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmtNum4 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(4));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::PARENT_T, stmtNum4, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "5", "6", "7", "8" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsUses") {
		// select a such that Uses(a, "x")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, declaration, identX);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "3" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsUses") {
		// select w such that Modifies(w, "y")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::WHILE, "w");

		shared_ptr<QueryInput> identY =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("y"));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::MODIFIES, declaration, identY);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "4" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("booleanResults") {
		// select s such that follow*(1, 3)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmtNum1 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(1));
		shared_ptr<QueryInput> stmtNum3 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(3));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmtNum1, stmtNum3);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "1", "10", "2", "3", "4", "5", "6", "7", "8", "9" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}
	
}

TEST_CASE("testSuchThatClauseFail") {

	shared_ptr<PKB> pkb = make_shared<PKB>(10);

	// 1 2 3 4 {5 {6, 7} 8} 9, 10
	REQUIRE(pkb->insertFollow(1, 2));
	REQUIRE(pkb->insertFollow(2, 3));
	REQUIRE(pkb->insertFollow(3, 4));
	REQUIRE(pkb->insertParent(4, 5));
	REQUIRE(pkb->insertParent(5, 6));
	REQUIRE(pkb->insertParent(5, 7));
	REQUIRE(pkb->insertFollow(6, 7));
	REQUIRE(pkb->insertParent(4, 8));
	REQUIRE(pkb->insertFollow(9, 10));

	pkb->init();

	// select s such that follow(4, 5)
	shared_ptr<QueryInterface> query =
		dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
	shared_ptr<Declaration> declaration =
		make_shared<Declaration>(EntityType::STMT, "s");

	shared_ptr<QueryInput> stmtNum4 =
		dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(4));
	shared_ptr<QueryInput> stmtNum5 =
		dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(5));

	query->setSelectClause(declaration);
	query->addRelationshipClause(RelationshipType::FOLLOWS, stmtNum4, stmtNum5);

	QueryEvaluator qe = QueryEvaluator(query, pkb);

	list<string> expected{ };
	list<string> result{ };
	shared_ptr<ResultsTable> resultsTable = qe.evaluate();
	ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
	TestResultsTableUtil::checkList(result, expected);
}

TEST_CASE("testPatternClauseSuccess") {

	shared_ptr<PKB> pkb = make_shared<PKB>(10);
	// 3. x = x + y + 1
	// 4. y = z + a + 1
	// 5. x = count - 1;
	// 8. z = a + x shared_ptr<QueryInput> y - 10;
	REQUIRE(pkb->insertDirectModifies(3, "x"));
	REQUIRE(pkb->insertDirectModifies(4, "y"));
	REQUIRE(pkb->insertDirectModifies(5, "x"));
	REQUIRE(pkb->insertDirectModifies(8, "z"));
	REQUIRE(pkb->insertExpression(3, "x"));
	REQUIRE(pkb->insertExpression(3, "y"));
	REQUIRE(pkb->insertExpression(3, "1"));
	REQUIRE(pkb->insertExpression(4, "z"));
	REQUIRE(pkb->insertExpression(4, "z"));
	REQUIRE(pkb->insertExpression(4, "a"));
	REQUIRE(pkb->insertExpression(4, "1"));
	REQUIRE(pkb->insertExpression(5, "count"));
	REQUIRE(pkb->insertExpression(5, "1"));
	REQUIRE(pkb->insertExpression(8, "a"));
	REQUIRE(pkb->insertExpression(8, "x"));
	REQUIRE(pkb->insertExpression(8, "y"));
	REQUIRE(pkb->insertExpression(8, "10"));

	pkb->init();

	SECTION("constantExpression") {
		// select a pattern a("x", "1")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
		shared_ptr<Expression> expression = make_shared<Expression>("1");

		query->setSelectClause(declaration);
		query->addPatternClause(declaration, identX, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "3", "5" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("variableExpression") {
		// select a pattern a("x", "count")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
		shared_ptr<Expression> expression = make_shared<Expression>("count");

		query->setSelectClause(declaration);
		query->addPatternClause(declaration, identX, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "5" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}
	
	SECTION("wildcardExpression") {
		// select a pattern a("x", "_")
		/*
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
		shared_ptr<Expression> expression = make_shared<Expression>("_");

		query->setSelectClause(declaration);
		query->addPatternClause(declaration, identX, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "3", "5" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
		*/
	}
}

TEST_CASE("testSuchThatAndPatternClausesSuccess") {
	shared_ptr<PKB> pkb = make_shared<PKB>(16);
	// 1 2 {3, 4, 5 {6, 7, 8 {9, 10}} 11 {12 {13}} 14, 15} 16
	REQUIRE(pkb->insertFollow(1, 2));
	REQUIRE(pkb->insertParent(2, 3));
	REQUIRE(pkb->insertParent(2, 4));
	REQUIRE(pkb->insertFollow(3, 4));
	REQUIRE(pkb->insertParent(2, 5));
	REQUIRE(pkb->insertFollow(4, 5));
	REQUIRE(pkb->insertParent(5, 6));
	REQUIRE(pkb->insertParent(5, 7));
	REQUIRE(pkb->insertFollow(6, 7));
	REQUIRE(pkb->insertParent(5, 8));
	REQUIRE(pkb->insertFollow(7, 8));
	REQUIRE(pkb->insertParent(8, 9));
	REQUIRE(pkb->insertParent(8, 10));
	REQUIRE(pkb->insertFollow(9, 10));
	REQUIRE(pkb->insertParent(2, 11));
	REQUIRE(pkb->insertParent(11, 12));
	REQUIRE(pkb->insertParent(12, 13));
	REQUIRE(pkb->insertParent(2, 14));
	REQUIRE(pkb->insertParent(2, 15));
	REQUIRE(pkb->insertFollow(14, 15));
	
	REQUIRE(pkb->insertDirectUses(3, set<string>{ "z" }));
	REQUIRE(pkb->insertDirectModifies(3, "x"));
	REQUIRE(pkb->insertDirectUses(5, set<string>{ "a", "b" }));
	REQUIRE(pkb->insertDirectModifies(7, "x"));
	REQUIRE(pkb->insertDirectUses(7, set<string>{ "z", "y" }));
	REQUIRE(pkb->setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb->insertExpression(7, "x"));
	REQUIRE(pkb->insertExpression(7, "y"));
	REQUIRE(pkb->insertExpression(7, "z"));
	REQUIRE(pkb->setStatementType(2, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectModifies(2, "x"));
	REQUIRE(pkb->insertExpression(2, "x"));
	REQUIRE(pkb->setStatementType(15, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectUses(15, set<string> { "z", "y" }));
	REQUIRE(pkb->insertExpression(15, "y"));
	REQUIRE(pkb->insertExpression(15, "z"));
	REQUIRE(pkb->insertDirectUses(9, set<string> { "a", "c", "z" }));
	REQUIRE(pkb->insertDirectModifies(9, "t"));
	REQUIRE(pkb->insertDirectUses(12, set<string> { "z", "t" }));
	REQUIRE(pkb->insertDirectModifies(14, "count"));
	REQUIRE(pkb->insertDirectModifies(15, "y"));	
	REQUIRE(pkb->insertDirectUses(16, set<string>{ "m" }));

	pkb->init();

	SECTION("selectSynonymInClauses") {
		// x = y {op} z
		// select a uses(a, "y") pattern a("x", "z")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
		shared_ptr<QueryInput> identY =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("y"));
		shared_ptr<Expression> expression = make_shared<Expression>("z");

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, declaration, identY);
		query->addPatternClause(declaration, identX, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "7" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	/*
	SECTION("selectSynonymNotInClauses") {
		// select v follows(1, s) pattern a("x", "z")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> stmtnum =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(1));
		shared_ptr<Declaration> stmt =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
		shared_ptr<Expression> expression = make_shared<Expression>("z");

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::FOLLOWS, stmtnum, stmt);
		query->addPatternClause(declaration, identX, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "a", "b", "c", "count", "m", "t", "x", "y", "z" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}
	*/
}
