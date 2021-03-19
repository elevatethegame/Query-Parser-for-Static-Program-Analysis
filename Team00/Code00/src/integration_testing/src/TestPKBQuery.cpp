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
	REQUIRE(pkb->insertProcedure("main"));
	REQUIRE(pkb->insertFollow(1, 2));
	REQUIRE(pkb->insertFollow(2, 3));
	REQUIRE(pkb->insertFollow(3, 4));
	REQUIRE(pkb->insertParent(4, 5));
	REQUIRE(pkb->insertParent(5, 6));
	REQUIRE(pkb->insertParent(5, 7));
	REQUIRE(pkb->insertFollow(6, 7));
	REQUIRE(pkb->insertParent(4, 8));
	REQUIRE(pkb->insertFollow(5, 8));
	REQUIRE(pkb->insertFollow(4, 9));
	REQUIRE(pkb->insertFollow(9, 10));

	REQUIRE(pkb->insertFollowStar(1, 2));
	REQUIRE(pkb->insertFollowStar(1, 3));
	REQUIRE(pkb->insertFollowStar(1, 4));
	REQUIRE(pkb->insertFollowStar(1, 9));
	REQUIRE(pkb->insertFollowStar(1, 10));
	REQUIRE(pkb->insertFollowStar(2, 3));
	REQUIRE(pkb->insertFollowStar(2, 4));
	REQUIRE(pkb->insertFollowStar(2, 9));
	REQUIRE(pkb->insertFollowStar(2, 10));
	REQUIRE(pkb->insertFollowStar(3, 4));
	REQUIRE(pkb->insertFollowStar(3, 9));
	REQUIRE(pkb->insertFollowStar(3, 10));
	REQUIRE(pkb->insertFollowStar(4, 9));
	REQUIRE(pkb->insertFollowStar(4, 10));
	REQUIRE(pkb->insertFollowStar(4, 10));
	REQUIRE(pkb->insertFollowStar(6, 7));
	REQUIRE(pkb->insertFollowStar(5, 8));

	REQUIRE(pkb->insertParentStar(4, 5));
	REQUIRE(pkb->insertParentStar(4, 6));
	REQUIRE(pkb->insertParentStar(4, 7));
	REQUIRE(pkb->insertParentStar(4, 8));
	REQUIRE(pkb->insertParentStar(5, 6));
	REQUIRE(pkb->insertParentStar(5, 7));
	
	// 1 2 3 4 {5 {6, 7} 8} 9, 10
	REQUIRE(pkb->insertNext(1, 2));
	REQUIRE(pkb->insertNext(2, 3));
	REQUIRE(pkb->insertNext(3, 4));
	REQUIRE(pkb->insertNext(4, 5));
	REQUIRE(pkb->insertNext(5, 6));
	REQUIRE(pkb->insertNext(6, 7));
	REQUIRE(pkb->insertNext(7, 6));
	REQUIRE(pkb->insertNext(7, 8));
	REQUIRE(pkb->insertNext(5, 8));
	REQUIRE(pkb->insertNext(8, 5));
	REQUIRE(pkb->insertNext(8, 9));
	REQUIRE(pkb->insertNext(4, 9));
	REQUIRE(pkb->insertNext(9, 10));

	REQUIRE(pkb->insertModifies(3, "x"));
	REQUIRE(pkb->insertUses(3, "x"));
	REQUIRE(pkb->insertExpression(3, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("x+1", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("x+1", ExpressionType::EXACT))));
	REQUIRE(pkb->setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(4, EntityType::WHILE));
	REQUIRE(pkb->setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb->setStatementType(8, EntityType::ASSIGN));
	REQUIRE(pkb->insertUses(4, "t"));
	REQUIRE(pkb->insertUses(4, "v"));
	REQUIRE(pkb->insertUses(4, "x"));
	REQUIRE(pkb->insertUses(4, "y"));
	REQUIRE(pkb->insertModifies(4, "y"));
	REQUIRE(pkb->insertUses(5, "v"));
	REQUIRE(pkb->insertModifies(8, "y"));
	REQUIRE(pkb->insertUses(8, "x"));
	REQUIRE(pkb->insertUses(8, "y"));
	REQUIRE(pkb->insertExpression(8, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(8, *(new Expression("x+1", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(8, *(new Expression("(x+1)*y", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(8, *(new Expression("(x+1)*y", ExpressionType::EXACT))));

	REQUIRE(pkb->insertProcUses("main", "x"));
	REQUIRE(pkb->insertProcUses("main", "y"));
	REQUIRE(pkb->insertProcUses("main", "t"));
	REQUIRE(pkb->insertProcUses("main", "v"));
	REQUIRE(pkb->insertProcModifies("main", "x"));
	REQUIRE(pkb->insertProcModifies("main", "y"));
	REQUIRE(pkb->setControlVariable(4, "t"));
	REQUIRE(pkb->setControlVariable(5, "v"));

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

	SECTION("nonBooleanResultsNext") {
		// select n such that Next(7, n)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::PROGLINE, "n");

		shared_ptr<QueryInput> stmtNum7 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(7));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::NEXT, stmtNum7, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "6", "8" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsProcUses") {
		// select v such that Uses("main", v)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> proc =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, proc, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "v", "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsUses") {
		// select v such that Uses(4, v)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> stmtNum4 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(4));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, stmtNum4, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "v", "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsProcUses") {
		// select v such that Uses("main", v)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> proc =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));

		query->setSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, proc, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "v", "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("mapResultsAssignPattern") {
		// select v pattern a(v, _"x+1"_)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration1 =
			make_shared<Declaration>(EntityType::ASSIGN, "a");
		shared_ptr<Declaration> declaration2 =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Expression> expression = make_shared<Expression>("x+1", ExpressionType::PARTIAL);

		query->setSelectClause(declaration2);
		query->addAssignPatternClause(declaration1, declaration2, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsContainerPattern") {
		// select w pattern w("v", _)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::WHILE, "w");
		shared_ptr<QueryInput> variable =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("v"));

		query->setSelectClause(declaration);
		query->addContainerPatternClause(declaration, variable);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "5" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

}