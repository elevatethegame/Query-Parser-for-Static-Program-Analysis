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

	REQUIRE(pkb.insertFollowStar(1, 2));
	REQUIRE(pkb.insertFollowStar(1, 3));
	REQUIRE(pkb.insertFollowStar(1, 4));
	REQUIRE(pkb.insertFollowStar(1, 7));
	REQUIRE(pkb.insertFollowStar(1, 8));
	REQUIRE(pkb.insertFollowStar(1, 9));
	REQUIRE(pkb.insertFollowStar(2, 3));
	REQUIRE(pkb.insertFollowStar(2, 4));
	REQUIRE(pkb.insertFollowStar(2, 7));
	REQUIRE(pkb.insertFollowStar(2, 8));
	REQUIRE(pkb.insertFollowStar(2, 9));
	REQUIRE(pkb.insertFollowStar(3, 4));
	REQUIRE(pkb.insertFollowStar(3, 7));
	REQUIRE(pkb.insertFollowStar(3, 8));
	REQUIRE(pkb.insertFollowStar(3, 9));
	REQUIRE(pkb.insertFollowStar(4, 7));
	REQUIRE(pkb.insertFollowStar(4, 8));
	REQUIRE(pkb.insertFollowStar(4, 9));
	REQUIRE(pkb.insertFollowStar(7, 8));
	REQUIRE(pkb.insertFollowStar(7, 9));
	REQUIRE(pkb.insertFollowStar(8, 9));
	REQUIRE(pkb.insertParentStar(4, 5));
	REQUIRE(pkb.insertParentStar(4, 6));
	REQUIRE(pkb.insertParentStar(5, 6));

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

	set<string> resultFollow = pkb.getSetResultsOfRS(FOLLOWS,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")));
	string a[] = { "2", "3", "4", "7", "8", "9" };
	set<string> expectedFollow{ std::begin(a), std::end(a) };
	REQUIRE(expectedFollow == resultFollow);
	
	set<string> resultParent = pkb.getSetResultsOfRS(PARENT,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new StmtNum(5)));
	string b[] = { "4" }; 
	set<string> expectedParent{ std::begin(b), std::end(b) };
	REQUIRE(expectedParent == resultParent);

	set<string> resultFollowStar = pkb.getSetResultsOfRS(FOLLOWS_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")),
		shared_ptr<QueryInput>(new StmtNum(8)));
	string c[] = { "1", "2", "3", "4", "7" };
	set<string> expectedFollowStar{ std::begin(c), std::end(c) };
	REQUIRE(expectedFollowStar == resultFollowStar);
	
	unordered_map<string, set<string>> resultParentStar = 
		pkb.getMapResultsOfRS(PARENT_T,
			shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s1")), 
			shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));
	REQUIRE(resultParentStar.size() == 2);
	REQUIRE(set<string> { "5", "6" } == resultParentStar["4"]);
	REQUIRE(set<string> { "6" } == resultParentStar["5"]);
}

TEST_CASE("PKB usesAndModifies") {

	PKB pkb = PKB(16);
	// 1 2 {3, 4, 5 {6, 7, 8 {9, 10}} 11 {12 {13}} 14, 15} 16

	REQUIRE(pkb.insertUses(2, "x"));
	REQUIRE(pkb.insertUses(2, "y"));
	REQUIRE(pkb.insertUses(2, "z"));
	REQUIRE(pkb.insertUses(2, "a"));
	REQUIRE(pkb.insertUses(2, "b"));
	REQUIRE(pkb.insertUses(2, "c"));
	REQUIRE(pkb.insertUses(2, "t"));
	REQUIRE(pkb.insertUses(3, "x"));
	REQUIRE(pkb.insertUses(3, "y"));
	REQUIRE(pkb.insertUses(5, "a"));
	REQUIRE(pkb.insertUses(5, "b"));
	REQUIRE(pkb.insertUses(5, "a"));
	REQUIRE(pkb.insertUses(5, "c"));
	REQUIRE(pkb.insertUses(5, "z"));
	REQUIRE(pkb.insertUses(8, "a"));
	REQUIRE(pkb.insertUses(8, "c"));
	REQUIRE(pkb.insertUses(8, "z"));
	REQUIRE(pkb.insertUses(9, "a"));
	REQUIRE(pkb.insertUses(9, "c"));
	REQUIRE(pkb.insertUses(9, "z"));
	REQUIRE(pkb.insertUses(11, "z"));
	REQUIRE(pkb.insertUses(11, "t"));
	REQUIRE(pkb.insertUses(12, "z"));
	REQUIRE(pkb.insertUses(12, "t"));
	REQUIRE(pkb.insertUses(15, "a"));
	REQUIRE(pkb.insertUses(16, "m"));

	REQUIRE(pkb.insertModifies(2, "x"));
	REQUIRE(pkb.insertModifies(2, "y"));
	REQUIRE(pkb.insertModifies(2, "z"));
	REQUIRE(pkb.insertModifies(2, "t"));
	REQUIRE(pkb.insertModifies(2, "count"));
	REQUIRE(pkb.insertModifies(2, "y"));
	REQUIRE(pkb.insertModifies(3, "x"));
	REQUIRE(pkb.insertModifies(5, "y"));
	REQUIRE(pkb.insertModifies(5, "z"));
	REQUIRE(pkb.insertModifies(5, "t"));
	REQUIRE(pkb.insertModifies(6, "y"));
	REQUIRE(pkb.insertModifies(7, "z"));
	REQUIRE(pkb.insertModifies(8, "t"));
	REQUIRE(pkb.insertModifies(9, "t"));
	REQUIRE(pkb.insertModifies(14, "count"));
	REQUIRE(pkb.insertModifies(15, "y"));

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
	
	set<string> resultUses1 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("t")));
	string c1[] = { "2", "11", "12" };
	REQUIRE(set<string>{ std::begin(c1), std::end(c1) } == resultUses1);
	
	set<string> resultUses2 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("z")));
	string c2[] = { "2", "5", "8", "9", "11", "12" };
	REQUIRE(set<string>{ std::begin(c2), std::end(c2) } == resultUses2);
	
	set<string> resultUses3 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new StmtNum(2)), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")));
	string c3[] = { "a", "b", "c", "x", "y", "z", "t" };
	REQUIRE(set<string>{ std::begin(c3), std::end(c3) } == resultUses3);

	set<string> resultModifies1 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("t")));
	string d1[] = { "2", "5", "8", "9" };
	REQUIRE(set<string>{ std::begin(d1), std::end(d1) } == resultModifies1);

	set<string> resultModifies2 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("y")));
	string d2[] = { "2", "5", "6", "15" };
	REQUIRE(set<string>{ std::begin(d2), std::end(d2) } == resultModifies2);

	set<string> resultModifies3 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new StmtNum(2)), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")));
	string d3[] = {  "x", "y", "z", "t", "count" };
	REQUIRE(set<string>{ std::begin(d3), std::end(d3) } == resultModifies3);

}

TEST_CASE("PKB multiProcedures") {
	
	PKB pkb = PKB(10);

	// main {1 {2, 3 {4 {5}}} first {6 {7}} second {8, 9, 10}
	// main -> first -> second

	REQUIRE(pkb.insertProcUses("main", "x"));
	REQUIRE(pkb.insertProcUses("main", "y"));
	REQUIRE(pkb.insertProcUses("main", "z"));
	REQUIRE(pkb.insertProcUses("main", "t"));
	REQUIRE(pkb.insertProcUses("main", "count"));
	REQUIRE(pkb.insertProcUses("first", "y"));
	REQUIRE(pkb.insertProcUses("first", "z"));
	REQUIRE(pkb.insertProcUses("first", "t"));
	REQUIRE(pkb.insertProcUses("first", "count"));
	REQUIRE(pkb.insertProcUses("second", "z"));
	REQUIRE(pkb.insertProcUses("second", "count"));

	REQUIRE(pkb.insertProcModifies("main", "count"));
	REQUIRE(pkb.insertProcModifies("main", "m"));
	REQUIRE(pkb.insertProcModifies("first", "count"));
	REQUIRE(pkb.insertProcModifies("second", "count"));

	REQUIRE(pkb.insertCalls("main", "first"));
	REQUIRE(pkb.insertCalls("first", "second"));
	REQUIRE(pkb.insertCallsStar("main", "first"));
	REQUIRE(pkb.insertCallsStar("main", "second"));
	REQUIRE(pkb.insertCallsStar("first", "second"));

	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("m"))));
	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new Ident("second")), shared_ptr<QueryInput>(new Ident("count"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new Ident("second")), shared_ptr<QueryInput>(new Ident("z"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("t"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES,
		shared_ptr<QueryInput>(new Ident("first")), shared_ptr<QueryInput>(new Ident("count"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(USES,
		shared_ptr<QueryInput>(new Ident("first")), shared_ptr<QueryInput>(new Ident("x"))));
	REQUIRE(pkb.getBooleanResultOfRS(CALLS, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("first"))));
	REQUIRE(pkb.getBooleanResultOfRS(CALLS_T, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("second"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(CALLS_T, 
		shared_ptr<QueryInput>(new Ident("first")), shared_ptr<QueryInput>(new Ident("main"))));

	set<string> resultUses1 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "p")), 
		shared_ptr<QueryInput>(new Ident("t")));
	string c1[] = { "main", "first" };
	REQUIRE(set<string>{ std::begin(c1), std::end(c1) } == resultUses1);

	set<string> resultUses2 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "proc")), 
		shared_ptr<QueryInput>(new Ident("count")));
	string c2[] = { "main", "first", "second" };
	REQUIRE(set<string>{ std::begin(c2), std::end(c2) } == resultUses2);

	set<string> resultUses3 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Ident("first")), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "var")));
	string c3[] = { "y", "z", "t", "count" };
	REQUIRE(set<string>{ std::begin(c3), std::end(c3) } == resultUses3);
	
	set<string> resultUses4 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Ident("count")), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "var")));
	REQUIRE(set<string>{ } == resultUses4);
	
	set<string> resultModifies1 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "s")), 
		shared_ptr<QueryInput>(new Ident("count")));
	string d1[] = { "main", "first", "second" };
	REQUIRE(set<string>{ std::begin(d1), std::end(d1) } == resultModifies1);

	set<string> resultModifies2 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Ident("main")), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "V")));
	string d2[] = { "count", "m" };
	REQUIRE(set<string>{ std::begin(d2), std::end(d2) } == resultModifies2);

	unordered_map<string, set<string>> resultModifies3 = pkb.getMapResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "p")),
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "V")));
	REQUIRE(set<string>{ "m", "count" } == resultModifies3["main"]);
	REQUIRE(set<string>{ "count" } == resultModifies3["first"]);
	REQUIRE(set<string>{ "count" } == resultModifies3["second"]);

}

TEST_CASE("PKB factorPattern") {
	
	PKB pkb = PKB(10);
	// 1 {2, 3, 4, 5, 6 {7 {8, 9}} 10}
	// 3. x = x + y + 1
	// 4. y = z + a + 1
	// 5. x = count - 1;
	// 8. z = a + x * y - 10;
	REQUIRE(pkb.setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(4, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(5, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(8, EntityType::ASSIGN));
	REQUIRE(pkb.insertModifies(1, "x"));
	REQUIRE(pkb.insertModifies(1, "y"));
	REQUIRE(pkb.insertModifies(1, "z"));
	REQUIRE(pkb.insertModifies(3, "x"));
	REQUIRE(pkb.insertModifies(4, "y"));
	REQUIRE(pkb.insertModifies(5, "x"));
	REQUIRE(pkb.insertModifies(7, "z"));
	REQUIRE(pkb.insertModifies(8, "z"));
	REQUIRE(pkb.insertExpression(3, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("y", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("1", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("z", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("1", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("count", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("1", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("y", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("10", ExpressionType::PARTIAL))));

	unordered_map<string, set<string>> resultPattern = 
		pkb.getMapResultsOfAssignPattern(shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")), 
		*(new Expression("1", ExpressionType::PARTIAL)));

	REQUIRE(set<string>{ "x" } == resultPattern["3"]);
	REQUIRE(set<string>{ "y" } == resultPattern["4"]);
	REQUIRE(set<string>{ } == resultPattern["8"]);
	
	set<string> result1 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Any()), *(new Expression("x", ExpressionType::PARTIAL)));
	REQUIRE(set<string>{ "3", "8" } == result1);

	set<string> result2 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Any()), *(new Expression("y", ExpressionType::PARTIAL)));
	REQUIRE(set<string>{ "3", "8" } == result2);

	set<string> result3 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Ident("x")), *(new Expression("1", ExpressionType::PARTIAL)));
	REQUIRE(set<string>{ "3", "5" } == result3);

	set<string> result4 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new StmtNum(3)), *(new Expression("1", ExpressionType::PARTIAL)));
	REQUIRE(set<string>{ } == result4);

	set<string> result5 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Ident("x")), *(new Expression("_", ExpressionType::EMPTY)));
	REQUIRE(set<string>{ "3", "5" } == result5);

	unordered_map<string, set<string>> result6 = pkb.getMapResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "x")), 
		*(new Expression("1", ExpressionType::PARTIAL)));
	REQUIRE(set<string>{ "x" } == result6["3"]);
	REQUIRE(set<string>{ "y" } == result6["4"]);
	REQUIRE(set<string>{ "x" } == result6["5"]);

	unordered_map<string, set<string>> result7 = pkb.getMapResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "VAR")),
		*(new Expression("_", ExpressionType::EMPTY)));
	REQUIRE(set<string>{ "x" } == result7["3"]);
	REQUIRE(set<string>{ "y" } == result7["4"]);
	REQUIRE(set<string>{ "x" } == result7["5"]);
	REQUIRE(set<string>{ "z" } == result7["8"]);

	set<string> result8 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new StmtNum(3)), *(new Expression("1", ExpressionType::EXACT)));
	REQUIRE(set<string>{ } == result8);
}


TEST_CASE("PKB containerPattern") {
	
	PKB pkb = PKB(10);
	// 1 {2} 3 {4, 5} 6 {7 {8, 9 {10}}}
	REQUIRE(pkb.setStatementType(1, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(3, EntityType::IF));
	REQUIRE(pkb.setStatementType(6, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(7, EntityType::IF));
	REQUIRE(pkb.setStatementType(9, EntityType::WHILE));
	
	REQUIRE(pkb.setControlVariable(1, "x"));
	REQUIRE(pkb.setControlVariable(3, "y"));
	REQUIRE(pkb.setControlVariable(7, "y"));
	REQUIRE(pkb.setControlVariable(9, "count"));
	REQUIRE(set<string>{ "1", "6", "9" } == pkb.getEntities(EntityType::WHILE));
	REQUIRE(set<string>{ "3", "7" } == pkb.getEntities(EntityType::IF));
	REQUIRE_FALSE(pkb.setControlVariable(0, "t"));
	REQUIRE_FALSE(pkb.setControlVariable(2, "x"));

	set<string> result1 = pkb.getSetResultsOfContainerPattern(
		EntityType::WHILE, shared_ptr<QueryInput>(new Any()));
	REQUIRE(set<string>{ "1", "9" } == result1);

	set<string> result2 = pkb.getSetResultsOfContainerPattern(
		EntityType::IF, shared_ptr<QueryInput>(new Ident("y")));
	REQUIRE(set<string>{ "3", "7" } == result2);

	set<string> result3 = pkb.getSetResultsOfContainerPattern(
		EntityType::ASSIGN, shared_ptr<QueryInput>(new Ident("count")));
	REQUIRE(set<string>{ } == result3);

	unordered_map<string, set<string>> result4 = pkb.getMapResultsOfContainerPattern(
		EntityType::WHILE, shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "VAR")));
	REQUIRE(set<string>{ "x" } == result4["1"]);
	REQUIRE(set<string>{ "count" } == result4["9"]);
	REQUIRE(set<string>{ } == result4["6"]);
	REQUIRE(set<string>{ } == result4["7"]);
}