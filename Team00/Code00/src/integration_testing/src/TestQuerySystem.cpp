#include "catch.hpp"
#include "QueryEvaluator.h"
#include "Query.h"
#include "QueryInterface.h"
#include "QueryParser.h"
#include "ResultsProjector.h"
#include "PKBStub.h"

void require(bool b) {
    REQUIRE(b);
}

TEST_CASE("1st Test") {
    require(1 == 1);

}

