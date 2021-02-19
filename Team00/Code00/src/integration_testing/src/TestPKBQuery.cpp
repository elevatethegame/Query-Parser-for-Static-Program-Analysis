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
	REQUIRE(pkb->setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(4, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(5, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(8, EntityType::ASSIGN));
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
	}
}

TEST_CASE("testSuchThatAndPatternClausesSuccess") {
	shared_ptr<PKB> pkb = make_shared<PKB>(16);
	// 1 2 {3, 4, 5 {6, 7, 8 {9, 10}} 11 {12, 13} 14, 15} 16
	// 1 read x
	REQUIRE(pkb->setStatementType(1, EntityType::READ));
	REQUIRE(pkb->insertDirectModifies(1, "x"));
	// 2 while (x == 0)
	REQUIRE(pkb->insertFollow(1, 2));
	REQUIRE(pkb->setStatementType(2, EntityType::WHILE));
	REQUIRE(pkb->insertDirectUses(2, set<string>{ "x" }));
	// 3 x = x + y + 1
	REQUIRE(pkb->insertParent(2, 3));
	REQUIRE(pkb->setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectUses(3, set<string>{ "x", "y" }));
	REQUIRE(pkb->insertDirectModifies(3, "x"));
	REQUIRE(pkb->insertExpression(3, "x"));
	REQUIRE(pkb->insertExpression(3, "y"));
	REQUIRE(pkb->insertExpression(3, "1"));
	// 4 call ...
	REQUIRE(pkb->insertParent(2, 4));
	REQUIRE(pkb->insertFollow(3, 4));
	REQUIRE(pkb->setStatementType(4, EntityType::CALL));
	// 5 while (a <= b) {6...10}
	REQUIRE(pkb->insertParent(2, 5));
	REQUIRE(pkb->insertFollow(4, 5));
	REQUIRE(pkb->setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb->insertDirectUses(5, set<string>{ "a", "b" }));
	// 6 y = 1
	REQUIRE(pkb->insertParent(5, 6));
	REQUIRE(pkb->setStatementType(6, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectModifies(6, "y"));
	REQUIRE(pkb->insertExpression(6, "1"));
	// 7 z = 1 + 2 + 3
	REQUIRE(pkb->insertParent(5, 7));
	REQUIRE(pkb->insertFollow(6, 7));
	REQUIRE(pkb->setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectModifies(7, "z"));
	REQUIRE(pkb->insertExpression(7, "1"));
	REQUIRE(pkb->insertExpression(7, "2"));
	REQUIRE(pkb->insertExpression(7, "3"));
	// 8 while (1 == 2) {9, 10}
	REQUIRE(pkb->insertParent(5, 8));
	REQUIRE(pkb->insertFollow(7, 8));
	REQUIRE(pkb->setStatementType(8, EntityType::WHILE));
	// 9 t = a + c * z + 1
	REQUIRE(pkb->insertParent(8, 9));
	REQUIRE(pkb->setStatementType(9, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectUses(9, set<string> { "a", "c", "z" }));
	REQUIRE(pkb->insertDirectModifies(9, "t"));
	REQUIRE(pkb->insertExpression(9, "a"));
	REQUIRE(pkb->insertExpression(9, "c"));
	REQUIRE(pkb->insertExpression(9, "z"));
	REQUIRE(pkb->insertExpression(9, "1"));
	// 10 t = 2 / 3
	REQUIRE(pkb->insertParent(8, 10));
	REQUIRE(pkb->insertFollow(9, 10));
	REQUIRE(pkb->setStatementType(10, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectModifies(10, "t"));
	REQUIRE(pkb->insertExpression(10, "2"));
	REQUIRE(pkb->insertExpression(10, "3"));
	// 11 if (1 == 2) {12, 13}
	REQUIRE(pkb->insertParent(2, 11));
	REQUIRE(pkb->setStatementType(11, EntityType::IF));
	// 12 t = z + t + 3
	REQUIRE(pkb->insertParent(11, 12));
	REQUIRE(pkb->setStatementType(12, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectUses(12, set<string> { "z", "t" }));
	REQUIRE(pkb->insertDirectModifies(12, "t"));
	REQUIRE(pkb->insertExpression(12, "z"));
	REQUIRE(pkb->insertExpression(12, "t"));
	REQUIRE(pkb->insertExpression(12, "3"));
	// 13 print y
	REQUIRE(pkb->insertFollow(12, 13));
	REQUIRE(pkb->insertParent(11, 13));
	REQUIRE(pkb->setStatementType(13, EntityType::PRINT));
	REQUIRE(pkb->insertDirectUses(13, set<string> { "y" }));
	// 14 read t
	REQUIRE(pkb->insertParent(2, 14));
	REQUIRE(pkb->setStatementType(14, EntityType::READ));
	REQUIRE(pkb->insertDirectModifies(14, "t"));
	// 15 y = a * a * a
	REQUIRE(pkb->insertParent(2, 15));
	REQUIRE(pkb->insertFollow(14, 15));
	REQUIRE(pkb->setStatementType(15, EntityType::ASSIGN));
	REQUIRE(pkb->insertDirectUses(15, set<string> { "a" }));
	REQUIRE(pkb->insertDirectModifies(15, "y"));
	REQUIRE(pkb->insertExpression(15, "a"));
	// 16 print m
	REQUIRE(pkb->setStatementType(16, EntityType::PRINT));
	REQUIRE(pkb->insertDirectUses(16, set<string>{ "m" }));

	pkb->init();

	SECTION("selectSynonymInClauses") {
		// x = ... t ... z ...
		// select a uses(a, "t") pattern a("t", "z")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identT =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("t"));
		shared_ptr<Expression> expression = make_shared<Expression>("z");

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, declaration, identT);
		query->addPatternClause(declaration, identT, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "12" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("selectSynonymNotInClauses") {
		// select v follows*(2, s) pattern a("x", "y")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declarationV =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Declaration> declarationA =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> stmtnum =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(2));
		shared_ptr<Declaration> stmt =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> identX =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
		shared_ptr<Expression> expression = make_shared<Expression>("y");

		query->setSelectClause(declarationV);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmtnum, stmt);
		query->addPatternClause(declarationA, identX, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "a", "b", "c", "m", "t", "x", "y", "z" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("selectTwoRelatedSynonyms") {
		// select v Uses(a, v) pattern a("t", "z")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declarationV =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Declaration> declarationA =
			make_shared<Declaration>(EntityType::ASSIGN, "a");

		shared_ptr<QueryInput> identT =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("t"));
		shared_ptr<Expression> expression = make_shared<Expression>("z");

		query->setSelectClause(declarationV);
		query->addRelationshipClause(RelationshipType::USES, declarationA, declarationV);
		query->addPatternClause(declarationA, identT, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "z", "a", "c" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("selectThreeRelatedSynonyms") {
		// select s follows*(a, s) pattern a(v, "1")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declarationV =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Declaration> declarationA =
			make_shared<Declaration>(EntityType::ASSIGN, "a");
		shared_ptr<Declaration> declarationS =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<Expression> expression = make_shared<Expression>("1");

		query->setSelectClause(declarationS);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, declarationA, declarationS);
		query->addPatternClause(declarationA, declarationV, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "11", "14", "15", "4", "5", "7", "8", "10" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("selectSynonymWithWildcards") {
		// select w follows*(w, _) pattern a(v, "_")
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declarationV =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Declaration> declarationA =
			make_shared<Declaration>(EntityType::ASSIGN, "a");
		shared_ptr<Declaration> declarationW =
			make_shared<Declaration>(EntityType::WHILE, "s");
		shared_ptr<Any> wildcard = make_shared<Any>();

		shared_ptr<Expression> expression = make_shared<Expression>("_");

		query->setSelectClause(declarationW);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, declarationW, wildcard);
		query->addPatternClause(declarationA, declarationV, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "2", "5" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}
	
}
