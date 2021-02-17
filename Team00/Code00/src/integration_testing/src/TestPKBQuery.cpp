#include <memory>
#include <list>

#include "PKB.h"
#include "Query.h"
#include "QueryEvaluator.h"
#include "QueryInput.h"
#include "QueryInputType.h"
#include "StmtNum.h"
#include "ResultsProjector.h"
#include "TestResultsTableUtil.h"
#include "catch.hpp"

TEST_CASE("testSingleClause") {
	
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

