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
#include "Parser.h"
#include "DesignExtractor.h"
#include "DesignExtractorHelper.h"


TEST_CASE("second test") {
    vector<string> codes = {
    "procedure main {",
        "while (1 == 2) {",
        "m = 1;",
        "while (x == 0) {",
        "while (2 > 3) {",
        "call first;",
        "}}}}",
    "procedure first {",
        "while (y + 1 == t) {",
        "call second;",
        "}}",
    "procedure second {",
        "count = 0;",
        "count = count + z;",
        "a = a * b - c / d + e - f * g % h;",
        "}"
    };
     
    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    cerr << error.getErrorMessage() << endl;
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();

    // auto expressions = extractor.getExpression(10);
    // cerr << "All the expressions of 10" << endl;
    // for (auto ex: expressions) {
    //     cerr << ex.getValue() << endl;
    // }
    // cerr << "End of getting expressions" << endl;
 
    SECTION("assignPattern1") {
        // select a pattern a("count", _"count"_)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::ASSIGN, "a");
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("a"));
        shared_ptr<Expression> expression = 
            make_shared<Expression>("(a*b)", ExpressionType::PARTIAL);

        query->setSelectClause(declaration);
        query->addAssignPatternClause(declaration, variable, expression);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    } 
}

TEST_CASE("First test") {
    vector<string> codes = {
    "procedure main {",
        "while (1 == 2) {",
        "m = 1;",
        "while (x == 0) {",
        "while (2 > 3) {",
        "call first;",
        "}}}}",
    "procedure first {",
        "while (y + 1 == t) {",
        "call second;",
        "}}",
    "procedure second {",
        "count = 0;",
        "count = count + z;",
        "count = count + (10 * 2);",
        "}"
    };
     
    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();

    // auto expressions = extractor.getExpression(10);
    // cerr << "All the expressions of 10" << endl;
    // for (auto ex: expressions) {
    //     cerr << ex.getValue() << endl;
    // }
    // cerr << "End of getting expressions" << endl;
 
    SECTION("assignPattern1") {
        // select a pattern a("count", _"count"_)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::ASSIGN, "a");
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));
        shared_ptr<Expression> expression = 
            make_shared<Expression>("(10*2)", ExpressionType::PARTIAL);

        query->setSelectClause(declaration);
        query->addAssignPatternClause(declaration, variable, expression);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    } 
}

TEST_CASE("Test multipleProcedures") {
    vector<string> codes = {
        "procedure main {",
            "while (1 == 2) {",
            "m = 1;",
            "while (x == 0) {",
            "while (2 > 3) {",
            "call first;",
            "}}}}",
        "procedure first {",
            "while (y + 1 == t) {",
            "call second;",
            "}}",
        "procedure second {",
            "count = 0;",
            "count = count + z;",
            "count = count + 10;",
            "}"
    };

    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();


    
    SECTION("followsT") {
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

        list<string> expected{ };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }


    SECTION("call Star") {
        // select p such that Call*(p, "second")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::PROC, "p");

        shared_ptr<QueryInput> caller =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, "p"));
        shared_ptr<QueryInput> callee =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("second"));

        query->setSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::CALLS_T, caller, callee);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{"main", "first" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    } 
    
    SECTION("stmtUses") {
        // select s such that uses(s, "count")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::STMT, "s");

        shared_ptr<QueryInput> stmt =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));

        query->setSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::USES, stmt, variable);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "1", "3", "4", "5", "6", "7", "9", "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        // cerr << "using " << endl;
        // for (auto x: result) {
        //     cerr << x << " ";
        // }
        // cerr << endl;
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("stmtModifies") {
        // select s such that modifies(s, "count")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::STMT, "s");

        shared_ptr<QueryInput> stmt =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));

        query->setSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::MODIFIES, stmt, variable);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "1", "3", "4", "5", "6", "7", "8", "9", "10"};
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        // cerr << "using " << endl;
        // for (auto x: result) {
        //     cerr << x << " ";
        // }
        // cerr << endl;
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("procUses") {
        // select p such that uses(p, "count")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::PROC, "p");

        shared_ptr<QueryInput> proc =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, "p"));
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));

        query->setSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::USES, proc, variable);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "main", "first", "second" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("procCallsT") {
        // select p such that calls*("main", p)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::PROC, "p");

        shared_ptr<QueryInput> proc =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, "p"));
        shared_ptr<QueryInput> procName =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));

        query->setSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::CALLS_T, procName, proc);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "first", "second" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("assignPattern") {
        // select a pattern a("count", _"count"_)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::ASSIGN, "a");
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));
        shared_ptr<Expression> expression = 
            make_shared<Expression>("count", ExpressionType::PARTIAL);

        query->setSelectClause(declaration);
        query->addAssignPatternClause(declaration, variable, expression);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "9", "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("containerPattern") {
        // select w pattern w(_, _)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::WHILE, "w");
        shared_ptr<QueryInput> wildcard = make_shared<Any>();

        query->setSelectClause(declaration);
        query->addContainerPatternClause(declaration, wildcard);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "3", "6" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

}
