#include "PKB.h"
#include "StmtNum.h"
#include "Any.h"
#include "SimpleParseError.h"
#include "SIMPLEToken.h"
#include "SIMPLETokenStream.h"
#include "Parser.h"
#include "SimpleParseError.h"

#include "catch.hpp"
#include <memory>
#include <algorithm>
#include <vector>


using namespace std;

bool equals(vector<string> a, vector<string> b) {
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());
    if (a.size() != b.size()) return false;
    auto n = a.size();
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

TEST_CASE("Positive condition very very very very very very very complicated") {
    string condition = "((!(x - 3 < 2)) && (!(a / 2 * 3 + 6 < ((10) * (4 - x - 1) / 8 - 6 / x)))) || (x == 1)";

    SIMPLETokenStream stream{{condition}};

    DesignExtractor extractor;
    extractor.increaseNumberOfStatement(EntityType::IF);
    Parser parser{extractor};
    
    auto error = parser.parseConditionExpression(stream, 1);
    REQUIRE_FALSE(error.hasError());
    REQUIRE(stream.isEmpty());
}

TEST_CASE("Negative condition very very very very complicated") {
    string condition = "(!(a + b + c < 3 * 2)) && (3 * a - 4 / b < ((10 * x)) - (2)) }";

    SIMPLETokenStream stream{{condition}};

    DesignExtractor extractor;
    extractor.increaseNumberOfStatement(EntityType::IF);
    Parser parser{extractor};
    
    auto error = parser.parseConditionExpression(stream, 1);
    REQUIRE_FALSE(error.hasError());
    REQUIRE_FALSE(stream.isEmpty());
    REQUIRE(stream.getToken().getValue() == "}");
    REQUIRE(stream.isEmpty());
}

TEST_CASE("Negative condition very very very very very very complicated") {
    string condition = "(b > c)";

    SIMPLETokenStream stream{{condition}};



    DesignExtractor extractor;
    Parser parser{extractor};
    extractor.increaseNumberOfStatement(EntityType::IF);
    
    auto error = parser.parseConditionExpression(stream, 1);    
    REQUIRE(error.hasError());
}

TEST_CASE("Source code parsing very simple") {
    vector<string> codes = {
        "procedure Tuan {",
        "read x;",
        "print y;",
        "}",
    };

    SIMPLETokenStream stream{codes};

    SIMPLETokenStream secondStream{stream};
    // while (secondStream.isEmpty() == false) {
    //     cerr << secondStream.getToken().toString() << endl;
    // }

    DesignExtractor extractor;
    Parser parser{extractor};
    extractor.increaseNumberOfStatement(EntityType::IF);

    auto error = parser.parseProcedure(stream);
    REQUIRE_FALSE(error.hasError());
}



TEST_CASE("Source code parsing more difficult ... ") {
    vector<string> codes = {
        "procedure Tuan {",
        "while (x > 5) {",
        "a = b; if ((a + b) > 2) then { a = a - a; } else { b = b + b; } a = b;",
        "}",
        "print y;",
        "}",
    };

    SIMPLETokenStream stream{codes};
    SIMPLETokenStream secondStream{stream};

    DesignExtractor extractor;
    Parser parser{extractor};

    auto error = parser.parseProcedure(stream);
    REQUIRE_FALSE(error.hasError());
    REQUIRE(equals(extractor.getUses(1), {"x"}));
    REQUIRE(equals(extractor.getModifies(2), {"a"}));
}


TEST_CASE("Negative cyclic calls") {
    vector<string> codes = {
        "procedure Tuan {",
        "call Chau;",
        "}",
        "procedure Chau {",
        "call Tuan;",
        "}"
    };

    SIMPLETokenStream stream{codes};
    SIMPLETokenStream secondStream{stream};

    DesignExtractor extractor;
    Parser parser{extractor};
    
    auto error = parser.parseProgram(stream);
    REQUIRE(error.hasError());
}

TEST_CASE("Two procedures with same name") {
    vector<string> codes = {
        "procedure Tuan {",
        "a = b;",
        "}",
        "procedure Tuan {",
        "c = d;",
        "}"
    };

    SIMPLETokenStream stream{codes};
    SIMPLETokenStream secondStream{stream};

    DesignExtractor extractor;
    Parser parser{extractor};
    
    auto error = parser.parseProgram(stream);
    REQUIRE(error.hasError());
}

TEST_CASE("Calling non existing procedure") {
    vector<string> codes = {
        "procedure Tuan {",
        "call Chau;",
        "}",
        "procedure Chau {",
        "c = d;",
        "}"
    };

    SIMPLETokenStream stream{codes};
    SIMPLETokenStream secondStream{stream};

    DesignExtractor extractor;
    Parser parser{extractor};
    
    auto error = parser.parseProgram(stream);
    REQUIRE(error.hasError());
    cerr << error.getErrorMessage() << endl;
}

