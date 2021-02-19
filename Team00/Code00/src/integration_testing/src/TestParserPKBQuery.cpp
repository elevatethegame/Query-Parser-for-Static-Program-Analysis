#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>
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
#include "Parser.h"
#include "TNode.h"
#include "SimpleParseError.h"
#include "SIMPLEToken.h"
#include "catch.hpp"

TEST_CASE("testMediumProgram") {
	vector<string> codes = {
	    "procedure test {",
		"read x;",
		"while (x == 0) {",
		"x = x + y + 1;",
		"x = t + m;",
		"while (a <= b) {",
		"y = 1;",
		"z = 1 + 2 + 3;",
		"while (1 == 2) {",
		"t = a + c * z + 1;",
		"t = 2 / 3;}}",
		"if (1 == 2) then {",
		"t = z + t + 3;",
		"} else {",
		"print y;",
		"}",
		"read t;",
		"y = a * a * a;",
		"}",
		"print m;",
		"}"
	};

	SIMPLETokenStream stream{ codes };

	SIMPLETokenStream secondStream{ stream };
	// while (secondStream.isEmpty() == false) {
	//     cerr << secondStream.getToken().toString() << endl;
	// }

	DesignExtractor extractor;
	Parser parser{ extractor };

	auto error = parser.parseProcedure(stream);
	REQUIRE_FALSE(error.hasError());
	
	shared_ptr<PKB> pkb = extractor.extractToPKB();

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
