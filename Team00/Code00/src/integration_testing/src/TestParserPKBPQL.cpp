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

    auto error = parser.parseProcedure(stream);
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

        list<string> expected{ "1", "3", "4", "5", "6", "7", "8", "9", "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

}
