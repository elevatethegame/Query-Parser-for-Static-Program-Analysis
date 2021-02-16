#include "catch.hpp"
#include "QueryEvaluator.h"
#include "Query.h"
#include "QueryInterface.h"
#include "QueryParser.h"
#include "ResultsProjector.h"
#include "PKBStub.h"
#include "TestResultsTableUtil.h"

TEST_CASE("First Test") {
    list<string> results;
    string input;

    SECTION("Query with only Select Clause") {
        input = "read re1, re2\t\n  ; variable\nv1,v2; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
            "stmt s1; assign\n\ta1,a2,a3; Select v1;";
        auto query = std::make_shared<Query>();
        QueryParser queryParser = QueryParser{ input, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        pkb->setGetEntitiesReturnValue({"x", "y", "z", "c", "a", "b"});
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "a", "b", "c", "x", "y", "z" });

    }

    SECTION("Query with Select Clause + Such that Clause") {
        input = "print pn; stmt s1; Select s1 such that Follows*(s1, pn);";
            
        auto query = std::make_shared<Query>();
        QueryParser queryParser = QueryParser{ input, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_map<std::string, std::set<std::string>> expectedMap = { {"2", {"3", "4"}}, { "5", {"1", "8"} }, {"13", {"14", "15"} } };
        pkb->setGetEntitiesReturnValue({ "2", "3", "5", "7", "11", "13" });
        pkb->setGetResultsOfRSReturnValue(expectedMap);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "13", "2", "5" });

    }
}

