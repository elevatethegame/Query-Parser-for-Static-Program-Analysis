#include "PKB.h"
#include "StmtNum.h"
#include "Ident.h"
#include "Any.h"

#include "catch.hpp"
using namespace std;


TEST_CASE("PKB setAndGetTypes") {

	PKB pkb = PKB(10);

	REQUIRE_FALSE(pkb.setStatementType(1, EntityType::PROC));
	REQUIRE_FALSE(pkb.setStatementType(2, EntityType::VAR));
	REQUIRE_FALSE(pkb.setStatementType(3, EntityType::CONST));
	REQUIRE_FALSE(pkb.setStatementType(4, EntityType::STMT));
	
	REQUIRE(pkb.insertProcedure("main"));
	REQUIRE(pkb.setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(6, EntityType::IF));
	REQUIRE(pkb.setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(8, EntityType::CALL));
	REQUIRE(pkb.setStatementType(9, EntityType::READ));
	REQUIRE(pkb.setStatementType(10, EntityType::READ));
	REQUIRE(pkb.insertConst("-100"));
	REQUIRE(pkb.insertConst("3"));

	REQUIRE_FALSE(pkb.setStatementType(5, EntityType::PRINT));
	REQUIRE_FALSE(pkb.setStatementType(11, EntityType::CALL));
	REQUIRE_FALSE(pkb.setStatementType(0, EntityType::WHILE));
	REQUIRE_FALSE(pkb.setStatementType(-1e9+7, EntityType::IF));

	REQUIRE(pkb.getEntities(EntityType::PRINT).empty());
	set<string> readResult;
	readResult.insert("9");
	readResult.insert("10");
	REQUIRE(pkb.getEntities(EntityType::READ) == readResult);
	set<string> whileResult;
	whileResult.insert("5");
	REQUIRE(pkb.getEntities(EntityType::WHILE) == whileResult);
	set<string> procResult;
	procResult.insert("main");
	REQUIRE(pkb.getEntities(EntityType::PROC) == procResult);
	set<string> constResult;
	constResult.insert("-100");
	constResult.insert("3");
	REQUIRE(pkb.getEntities(EntityType::CONST) == constResult);

	REQUIRE(pkb.getEntities(EntityType::VAR).empty());
	REQUIRE(pkb.getEntities(EntityType::STMT).size() == 10);
}

TEST_CASE("PKB parentAndFollow") {
	
	PKB pkb = PKB(10);
	// 1 2 3 4 {5 {6}} 7 8 9
	REQUIRE(pkb.insertFollow(1, 2));
	REQUIRE(pkb.insertFollow(2, 3));
	REQUIRE(pkb.insertFollow(3, 4));
	REQUIRE(pkb.insertParent(4, 5));
	REQUIRE(pkb.insertParent(5, 6));
	REQUIRE(pkb.insertFollow(4, 7));
	REQUIRE(pkb.insertFollow(7, 8));
	REQUIRE(pkb.insertFollow(8, 9));

	REQUIRE_FALSE(pkb.insertFollow(0, 1));
	REQUIRE_FALSE(pkb.insertFollow(3, 1));
	REQUIRE_FALSE(pkb.insertParent(0, -1));
	REQUIRE_FALSE(pkb.insertParent(1, 1));

	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(1)), shared_ptr<QueryInput>(new StmtNum(2))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(3)), shared_ptr<QueryInput>(new StmtNum(4))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(8)), shared_ptr<QueryInput>(new StmtNum(9))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new Any()), shared_ptr<QueryInput>(new StmtNum(3))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(7)), shared_ptr<QueryInput>(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new Any()), shared_ptr<QueryInput>(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(PARENT, 
		shared_ptr<QueryInput>(new StmtNum(4)), shared_ptr<QueryInput>(new StmtNum(5))));
	REQUIRE(pkb.getBooleanResultOfRS(PARENT, 
		shared_ptr<QueryInput>(new StmtNum(4)), shared_ptr<QueryInput>(new Any())));

	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(9)), 
		shared_ptr<QueryInput>(new StmtNum(10))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(4)), 
		shared_ptr<QueryInput>(new StmtNum(8))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(0)), 
		shared_ptr<QueryInput>(new StmtNum(1))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT, 
		shared_ptr<QueryInput>(new StmtNum(11)), 
		shared_ptr<QueryInput>(new Any())));
	
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new Declaration(EntityType::ASSIGN,"a")), 
		shared_ptr<QueryInput>(new StmtNum(1))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Declaration(EntityType::CALL, "c"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Ident("dummyIdent"))));

	set<string> resultFollow = pkb.getResultsOfRS(FOLLOWS,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")))[""];
	string a[] = { "2", "3", "4", "7", "8", "9" };
	set<string> expectedFollow{ std::begin(a), std::end(a) };
	REQUIRE(expectedFollow == resultFollow);
	
	set<string> resultParent = pkb.getResultsOfRS(PARENT,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new StmtNum(5)))[""];
	string b[] = { "4" }; 
	set<string> expectedParent{ std::begin(b), std::end(b) };
	REQUIRE(expectedParent == resultParent);
}

TEST_CASE("PKB extarctStar 1") {
	
	PKB pkb = PKB(10);
	// 1 2 3 4 {5 {6, 7} 8} 9, 10
	REQUIRE(pkb.insertFollow(1, 2));
	REQUIRE(pkb.insertFollow(2, 3));
	REQUIRE(pkb.insertFollow(3, 4));
	REQUIRE(pkb.insertParent(4, 5));
	REQUIRE(pkb.insertParent(5, 6));
	REQUIRE(pkb.insertParent(5, 7));
	REQUIRE(pkb.insertFollow(6, 7));
	REQUIRE(pkb.insertParent(4, 8));
	REQUIRE(pkb.insertFollow(5, 8));
	REQUIRE(pkb.insertFollow(4, 9));
	REQUIRE(pkb.insertFollow(9, 10));

	pkb.init();

	unordered_map<string, set<string>> resultFollow = pkb.getResultsOfRS(
		RelationshipType::FOLLOWS_T, 
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s1")),
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));
	
	string a1[] = { "2", "3", "4", "9", "10" };
	set<string> expected1{ std::begin(a1), std::end(a1) };
	REQUIRE(expected1 == resultFollow["1"]);

	string a2[] = { "3", "4", "9", "10" };
	set<string> expected2{ std::begin(a2), std::end(a2) };
	REQUIRE(expected2 == resultFollow["2"]);

	string a3[] = { "4", "9", "10" };
	set<string> expected3{ std::begin(a3), std::end(a3) };
	REQUIRE(expected3 == resultFollow["3"]);

	string a5[] = { "8" };
	set<string> expected5{ std::begin(a5), std::end(a5) };
	REQUIRE(expected5 == resultFollow["5"]);

	string a6[] = { "7" };
	set<string> expected6{ std::begin(a6), std::end(a6) };
	REQUIRE(expected6 == resultFollow["6"]);

	set<string> expected10{};
	REQUIRE(expected10 == resultFollow["10"]);

	unordered_map<string, set<string>> resultParent = pkb.getResultsOfRS(
		RelationshipType::PARENT_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s1")),
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));

	string b4[] = { "5", "6", "7", "8" };
	REQUIRE(set<string>{ std::begin(b4), std::end(b4) } == resultParent["4"]);

	string b5[] = { "6", "7" };
	REQUIRE(set<string>{ std::begin(b5), std::end(b5) } == resultParent["5"]);

	REQUIRE(set<string>{} == resultParent["1"]);
	REQUIRE(set<string>{} == resultParent["6"]);
	REQUIRE(set<string>{} == resultParent["10"]);
}

TEST_CASE("PKB extarctStar 2") {

	PKB pkb = PKB(16);
	// 1 2 {3, 4, 5 {6, 7, 8 {9, 10}} 11 {12 {13}} 14, 15} 16
	REQUIRE(pkb.insertFollow(1, 2));
	REQUIRE(pkb.insertParent(2, 3));
	REQUIRE(pkb.insertParent(2, 4));
	REQUIRE(pkb.insertFollow(3, 4));
	REQUIRE(pkb.insertParent(2, 5));
	REQUIRE(pkb.insertFollow(4, 5));
	REQUIRE(pkb.insertParent(5, 6));
	REQUIRE(pkb.insertParent(5, 7));
	REQUIRE(pkb.insertFollow(6, 7));
	REQUIRE(pkb.insertParent(5, 8));
	REQUIRE(pkb.insertFollow(7, 8));
	REQUIRE(pkb.insertParent(8, 9));
	REQUIRE(pkb.insertParent(8, 10));
	REQUIRE(pkb.insertFollow(9, 10));
	REQUIRE(pkb.insertParent(2, 11));
	REQUIRE(pkb.insertFollow(5, 11));
	REQUIRE(pkb.insertParent(11, 12));
	REQUIRE(pkb.insertParent(12, 13));
	REQUIRE(pkb.insertParent(2, 14));
	REQUIRE(pkb.insertFollow(11, 14));
	REQUIRE(pkb.insertParent(2, 15));
	REQUIRE(pkb.insertFollow(14, 15));
	REQUIRE(pkb.insertFollow(2, 16));

	REQUIRE(pkb.insertDirectUses(2, set<string>{ "x" }));
	REQUIRE(pkb.insertDirectUses(3, set<string>{ "x", "y" }));
	REQUIRE(pkb.insertDirectModifies(3, "x"));
	REQUIRE(pkb.insertDirectUses(5, set<string>{ "a", "b" }));
	REQUIRE(pkb.insertDirectModifies(6, "y"));
	REQUIRE(pkb.insertDirectModifies(7, "z"));
	REQUIRE(pkb.insertDirectUses(9, set<string> { "a", "c", "z" }));
	REQUIRE(pkb.insertDirectModifies(9, "t"));
	REQUIRE(pkb.insertDirectUses(12, set<string> { "z", "t" }));
	REQUIRE(pkb.insertDirectModifies(14, "count"));
	REQUIRE(pkb.insertDirectUses(15, set<string> { "a" }));
	REQUIRE(pkb.insertDirectModifies(15, "y"));
	REQUIRE(pkb.insertDirectUses(16, set<string>{ "m" }));

	pkb.init();

	unordered_map<string, set<string>> resultFollow = pkb.getResultsOfRS(
		RelationshipType::FOLLOWS_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s1")),
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));

	string a1[] = { "2", "16" };
	REQUIRE(set<string> { std::begin(a1), std::end(a1) } == resultFollow["1"]);

	string a3[] = { "4", "5", "11", "14", "15" };
	REQUIRE(set<string> { std::begin(a3), std::end(a3) } == resultFollow["3"]);

	string a6[] = { "7", "8" };
	REQUIRE(set<string> { std::begin(a6), std::end(a6) } == resultFollow["6"]);

	string a11[] = { "14", "15" };
	REQUIRE(set<string> { std::begin(a11), std::end(a11) } == resultFollow["11"]);

	set<string> expected12{};
	REQUIRE(expected12 == resultFollow["12"]);

	unordered_map<string, set<string>> resultParent = pkb.getResultsOfRS(
		RelationshipType::PARENT_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s1")),
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));

	string b2[] = { "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15" };
	REQUIRE(set<string>{ std::begin(b2), std::end(b2) } == resultParent["2"]);

	string b5[] = { "6", "7", "8", "9", "10" };
	REQUIRE(set<string>{ std::begin(b5), std::end(b5) } == resultParent["5"]);

	string b11[] = { "12", "13" };
	REQUIRE(set<string>{ std::begin(b11), std::end(b11) } == resultParent["11"]);

	string b12[] = { "13" };
	REQUIRE(set<string>{ std::begin(b12), std::end(b12) } == resultParent["12"]);

	REQUIRE(set<string>{} == resultParent["1"]);
	REQUIRE(set<string>{} == resultParent["6"]);
	REQUIRE(set<string>{} == resultParent["13"]);

	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new StmtNum(3)), shared_ptr<QueryInput>(new Ident("x"))));
	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new StmtNum(5)), shared_ptr<QueryInput>(new Ident("t"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new StmtNum(8)), shared_ptr<QueryInput>(new Ident("c"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new StmtNum(11)), shared_ptr<QueryInput>(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new Any()), shared_ptr<QueryInput>(new Ident("z"))));
	
	set<string> resultUses1 = pkb.getResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("t")))[""];
	string c1[] = { "2", "11", "12" };
	REQUIRE(set<string>{ std::begin(c1), std::end(c1) } == resultUses1);
	
	set<string> resultUses2 = pkb.getResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("z")))[""];
	string c2[] = { "2", "5", "8", "9", "11", "12" };
	REQUIRE(set<string>{ std::begin(c2), std::end(c2) } == resultUses2);
	
	set<string> resultUses3 = pkb.getResultsOfRS(USES,
		shared_ptr<QueryInput>(new StmtNum(2)), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")))[""];
	string c3[] = { "a", "b", "c", "x", "y", "z", "t" };
	REQUIRE(set<string>{ std::begin(c3), std::end(c3) } == resultUses3);

	set<string> resultModifies1 = pkb.getResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("t")))[""];
	string d1[] = { "2", "5", "8", "9" };
	REQUIRE(set<string>{ std::begin(d1), std::end(d1) } == resultModifies1);

	set<string> resultModifies2 = pkb.getResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("y")))[""];
	string d2[] = { "2", "5", "6", "15" };
	REQUIRE(set<string>{ std::begin(d2), std::end(d2) } == resultModifies2);

	set<string> resultModifies3 = pkb.getResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new StmtNum(2)), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")))[""];
	string d3[] = {  "x", "y", "z", "t", "count" };
	REQUIRE(set<string>{ std::begin(d3), std::end(d3) } == resultModifies3);

}

TEST_CASE("PKB factorPattern") {
	
	PKB pkb = PKB(10);
	// 3. x = x + y + 1
	// 4. y = z + a + 1
	// 5. x = count - 1;
	// 8. z = a + x shared_ptr<QueryInput> y - 10;
	REQUIRE(pkb.insertDirectModifies(3, "x"));
	REQUIRE(pkb.insertDirectModifies(4, "y"));
	REQUIRE(pkb.insertDirectModifies(5, "x"));
	REQUIRE(pkb.insertDirectModifies(8, "z"));
	REQUIRE(pkb.insertExpression(3, "x"));
	REQUIRE(pkb.insertExpression(3, "y"));
	REQUIRE(pkb.insertExpression(3, "1"));
	REQUIRE(pkb.insertExpression(4, "z"));
	REQUIRE(pkb.insertExpression(4, "z"));
	REQUIRE(pkb.insertExpression(4, "a"));
	REQUIRE(pkb.insertExpression(4, "1"));
	REQUIRE(pkb.insertExpression(5, "count"));
	REQUIRE(pkb.insertExpression(5, "1"));
	REQUIRE(pkb.insertExpression(8, "a"));
	REQUIRE(pkb.insertExpression(8, "x"));
	REQUIRE(pkb.insertExpression(8, "y"));
	REQUIRE(pkb.insertExpression(8, "10"));

	unordered_map<string, set<string>> resultPattern = 
		pkb.getResultsOfPattern(EntityType::ASSIGN,
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")), *(new Expression("1")));

	REQUIRE(set<string>{ "x" } == resultPattern["3"]);
	REQUIRE(set<string>{ "y" } == resultPattern["4"]);
	REQUIRE(set<string>{ } == resultPattern["8"]);
	
	set<string> result1 = pkb.getResultsOfPattern(EntityType::ASSIGN,
			shared_ptr<QueryInput>(new Any()), *(new Expression("x")))[""];
	REQUIRE(set<string>{ "3", "8" } == result1);

	set<string> result2 = pkb.getResultsOfPattern(EntityType::ASSIGN,
		shared_ptr<QueryInput>(new Any()), *(new Expression("y")))[""];
	REQUIRE(set<string>{ "3", "8" } == result2);

	set<string> result3 = pkb.getResultsOfPattern(EntityType::ASSIGN,
		shared_ptr<QueryInput>(new Ident("x")), *(new Expression("1")))[""];
	REQUIRE(set<string>{ "3", "5" } == result3);

	set<string> result4 = pkb.getResultsOfPattern(EntityType::ASSIGN,
		shared_ptr<QueryInput>(new StmtNum(3)), *(new Expression("1")))[""];
	REQUIRE(set<string>{ } == result4);

}


TEST_CASE("PKB iter1SimTest") {

	PKB pkb = PKB(16);
	// 1 2 {3, 4, 5 {6, 7, 8 {9, 10}} 11 {12, 13} 14, 15} 16
	// 1 read x
	REQUIRE(pkb.setStatementType(1, EntityType::READ));
	REQUIRE(pkb.insertDirectModifies(1, "x"));
	// 2 while (x == 0)
	REQUIRE(pkb.insertFollow(1, 2));
	REQUIRE(pkb.setStatementType(2, EntityType::WHILE));
	REQUIRE(pkb.insertDirectUses(2, set<string>{ "x" }));
	// 3 x = x + y + 1
	REQUIRE(pkb.insertParent(2, 3));
	REQUIRE(pkb.setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectUses(3, set<string>{ "x", "y" }));
	REQUIRE(pkb.insertDirectModifies(3, "x"));
	REQUIRE(pkb.insertExpression(3, "x"));
	REQUIRE(pkb.insertExpression(3, "y"));
	REQUIRE(pkb.insertExpression(3, "1"));
	// 4 call ...
	REQUIRE(pkb.insertParent(2, 4));
	REQUIRE(pkb.insertFollow(3, 4));
	REQUIRE(pkb.setStatementType(4, EntityType::CALL));
	// 5 while (a <= b) {6...10}
	REQUIRE(pkb.insertParent(2, 5));
	REQUIRE(pkb.insertFollow(4, 5));
	REQUIRE(pkb.setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb.insertDirectUses(5, set<string>{ "a", "b" }));
	// 6 y = 1
	REQUIRE(pkb.insertParent(5, 6));
	REQUIRE(pkb.setStatementType(6, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectModifies(6, "y"));
	REQUIRE(pkb.insertExpression(6, "1"));
	// 7 z = 1 + 2 + 3
	REQUIRE(pkb.insertParent(5, 7));
	REQUIRE(pkb.insertFollow(6, 7));
	REQUIRE(pkb.setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectModifies(7, "z"));
	REQUIRE(pkb.insertExpression(7, "1"));
	REQUIRE(pkb.insertExpression(7, "2"));
	REQUIRE(pkb.insertExpression(7, "3"));
	// 8 while (1 == 2) {9, 10}
	REQUIRE(pkb.insertParent(5, 8));
	REQUIRE(pkb.insertFollow(7, 8));
	REQUIRE(pkb.setStatementType(8, EntityType::WHILE));
	// 9 t = a + c * z + 1
	REQUIRE(pkb.insertParent(8, 9));
	REQUIRE(pkb.setStatementType(9, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectUses(9, set<string> { "a", "c", "z" }));
	REQUIRE(pkb.insertDirectModifies(9, "t"));
	REQUIRE(pkb.insertExpression(9, "a"));
	REQUIRE(pkb.insertExpression(9, "c"));
	REQUIRE(pkb.insertExpression(9, "z"));
	REQUIRE(pkb.insertExpression(9, "1"));
	// 10 t = 2 / 3
	REQUIRE(pkb.insertParent(8, 10));
	REQUIRE(pkb.insertFollow(9, 10));
	REQUIRE(pkb.setStatementType(10, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectModifies(10, "t"));
	REQUIRE(pkb.insertExpression(10, "2"));
	REQUIRE(pkb.insertExpression(10, "3"));
	// 11 if (1 == 2) {12, 13}
	REQUIRE(pkb.insertParent(2, 11));
	REQUIRE(pkb.insertFollow(5, 11));
	REQUIRE(pkb.setStatementType(11, EntityType::IF));
	// 12 t = z + t + 3
	REQUIRE(pkb.insertParent(11, 12));
	REQUIRE(pkb.setStatementType(12, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectUses(12, set<string> { "z", "t" }));
	REQUIRE(pkb.insertDirectModifies(12, "t"));
	REQUIRE(pkb.insertExpression(12, "z"));
	REQUIRE(pkb.insertExpression(12, "t"));
	REQUIRE(pkb.insertExpression(12, "3"));
	// 13 print y
	REQUIRE(pkb.insertFollow(12, 13));
	REQUIRE(pkb.insertParent(11, 13));
	REQUIRE(pkb.setStatementType(13, EntityType::PRINT));
	REQUIRE(pkb.insertDirectUses(13, set<string> { "y" }));
	// 14 read t
	REQUIRE(pkb.insertParent(2, 14));
	REQUIRE(pkb.insertFollow(11, 14));
	REQUIRE(pkb.setStatementType(14, EntityType::READ));
	REQUIRE(pkb.insertDirectModifies(14, "t"));
	// 15 y = a * a * a
	REQUIRE(pkb.insertParent(2, 15));
	REQUIRE(pkb.insertFollow(14, 15));
	REQUIRE(pkb.setStatementType(15, EntityType::ASSIGN));
	REQUIRE(pkb.insertDirectUses(15, set<string> { "a" }));
	REQUIRE(pkb.insertDirectModifies(15, "y"));
	REQUIRE(pkb.insertExpression(15, "a"));
	// 16 print m
	REQUIRE(pkb.insertFollow(2, 16));
	REQUIRE(pkb.setStatementType(16, EntityType::PRINT));
	REQUIRE(pkb.insertDirectUses(16, set<string>{ "m" }));

	pkb.init();

	set<string> resultAssigns = pkb.getEntities(EntityType::ASSIGN);
	string resAssign[] = { "3", "6", "7", "9", "10", "12", "15" };
	REQUIRE(set<string> { std::begin(resAssign), std::end(resAssign)} == resultAssigns);

	set<string> resultVars = pkb.getEntities(EntityType::VAR);
	string resVar[] = { "x", "y", "z", "a", "b", "c", "t", "m" };
	REQUIRE(set<string> { std::begin(resVar), std::end(resVar)} == resultVars);

	unordered_map<string, set<string>> resultFollow = pkb.getResultsOfRS(
		RelationshipType::FOLLOWS_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::ASSIGN, "s1")),
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));

	string a3[] = { "4", "5", "11", "14", "15" };
	REQUIRE(set<string> { std::begin(a3), std::end(a3) } == resultFollow["3"]);

	string a6[] = { "7", "8" };
	REQUIRE(set<string> { std::begin(a6), std::end(a6) } == resultFollow["6"]);

	REQUIRE(set<string> { "10" } == resultFollow["9"]);

	REQUIRE(set<string> { "13" } == resultFollow["12"]);

	REQUIRE(set<string>{} == resultFollow["1"]);
	REQUIRE(set<string>{} == resultFollow["2"]);
	REQUIRE(set<string>{} == resultFollow["14"]);
	REQUIRE(set<string>{} == resultFollow["15"]);

	unordered_map<string, set<string>> resultParent = pkb.getResultsOfRS(
		RelationshipType::PARENT_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::WHILE, "s1")),
		shared_ptr<QueryInput>(new Declaration(EntityType::ASSIGN, "s2")));

	string b2[] = { "3", "6", "7", "9", "10", "12", "15" };
	REQUIRE(set<string>{ std::begin(b2), std::end(b2) } == resultParent["2"]);

	string b5[] = { "6", "7", "9", "10" };
	REQUIRE(set<string>{ std::begin(b5), std::end(b5) } == resultParent["5"]);

	string b8[] = { "9", "10" };
	REQUIRE(set<string>{ std::begin(b8), std::end(b8) } == resultParent["8"]);

	REQUIRE(set<string>{} == resultParent["1"]);
	REQUIRE(set<string>{} == resultParent["6"]);
	REQUIRE(set<string>{} == resultParent["11"]);
	REQUIRE(set<string>{} == resultParent["13"]);

	set<string> resultUses = pkb.getResultsOfRS(
		RelationshipType::USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::ASSIGN, "s")),
		shared_ptr<QueryInput>(new Ident("x")))[""];
	REQUIRE(set<string>{ "3" } == resultUses);

	set<string> resultModifies = pkb.getResultsOfRS(
		RelationshipType::MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::WHILE, "s")),
		shared_ptr<QueryInput>(new Ident("t")))[""];
	set<string> expected = { "2", "5", "8" };
	REQUIRE(set<string>{ std::begin(expected), std::end(expected) } == resultModifies);

	set<string> result1 = pkb.getResultsOfPattern(EntityType::STMT,
		shared_ptr<QueryInput>(new Ident("t")), *(new Expression("t")))[""];
	REQUIRE(set<string>{ "12" } == result1);

	set<string> result2 = pkb.getResultsOfPattern(EntityType::ASSIGN,
		shared_ptr<QueryInput>(new Any()), *(new Expression("1")))[""];
	REQUIRE(set<string>{ "3", "6", "7", "9" } == result2);

}