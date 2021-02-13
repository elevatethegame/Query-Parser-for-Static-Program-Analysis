#include "PKB.h"
#include "StmtNum.h"
#include "Indent.h"
#include "Any.h"

#include "catch.hpp"
using namespace std;


TEST_CASE("PKB setAndGetTypes") {

	PKB pkb = PKB(10);

	REQUIRE_FALSE(pkb.setStatementType(1, EntityType::PROC));
	REQUIRE_FALSE(pkb.setStatementType(2, EntityType::VAR));
	REQUIRE_FALSE(pkb.setStatementType(3, EntityType::CONST));
	REQUIRE_FALSE(pkb.setStatementType(4, EntityType::STMT));
	
	REQUIRE(pkb.setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(6, EntityType::IF));
	REQUIRE(pkb.setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(8, EntityType::CALL));
	REQUIRE(pkb.setStatementType(9, EntityType::READ));
	REQUIRE(pkb.setStatementType(10, EntityType::READ));

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
	
	REQUIRE(pkb.getEntities(EntityType::PROC).empty());
	REQUIRE(pkb.getEntities(EntityType::VAR).empty());
	REQUIRE(pkb.getEntities(EntityType::CONST).empty());
	REQUIRE(pkb.getEntities(EntityType::STMT).size() == 10);
}

TEST_CASE("PKB parentAndFollow") {
	
	PKB pkb = PKB(10);

	REQUIRE(pkb.insertFollow(1, 2));
	REQUIRE(pkb.insertFollow(2, 3));
	REQUIRE(pkb.insertFollow(3, 4));
	REQUIRE(pkb.insertParent(4, 5));
	REQUIRE(pkb.insertParent(5, 6));
	REQUIRE(pkb.insertFollow(7, 8));
	REQUIRE(pkb.insertFollow(8, 9));

	REQUIRE_FALSE(pkb.insertFollow(0, 1));
	REQUIRE_FALSE(pkb.insertFollow(3, 1));
	REQUIRE_FALSE(pkb.insertParent(0, -1));
	REQUIRE_FALSE(pkb.insertParent(1, 1));

	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, *(new StmtNum(1)), *(new StmtNum(2))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, *(new StmtNum(3)), *(new StmtNum(4))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, *(new StmtNum(8)), *(new StmtNum(9))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, *(new Any()), *(new StmtNum(3))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, *(new StmtNum(7)), *(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, *(new Any()), *(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(PARENT, *(new StmtNum(4)), *(new StmtNum(5))));
	REQUIRE(pkb.getBooleanResultOfRS(PARENT, *(new StmtNum(4)), *(new Any())));

	REQUIRE_FALSE(pkb.getBooleanResultOfRS(
		RelationshipType::FOLLOWS, *(new StmtNum(9)), *(new StmtNum(10))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(
		RelationshipType::FOLLOWS, *(new StmtNum(4)), *(new Any())));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(
		RelationshipType::FOLLOWS, *(new StmtNum(0)), *(new StmtNum(1))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(
		RelationshipType::PARENT, *(new StmtNum(11)), *(new Any())));
	
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		*(new Declaration(EntityType::ASSIGN,"a")), *(new StmtNum(1))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT,
		*(new Any()), *(new Declaration(EntityType::CALL, "c"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT,
		*(new Any()), *(new Indent("dummyIndent"))));

	set<string> resultFollow = pkb.getResultsOfRS(FOLLOWS,
		*(new Any()), *(new Declaration(EntityType::STMT, "s")))[""];
	string a[] = { "2", "3", "4", "8", "9" };
	set<string> expectedFollow{ std::begin(a), std::end(a) };
	REQUIRE(expectedFollow == resultFollow);
	
	set<string> resultParent = pkb.getResultsOfRS(PARENT,
		*(new Declaration(EntityType::STMT, "s")), *(new StmtNum(5)))[""];
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
	REQUIRE(pkb.insertFollow(9, 10));

	pkb.init();

	unordered_map<string, set<string>> resultFollow = pkb.getResultsOfRS(
		RelationshipType::FOLLOWS_S, 
		*(new Declaration(EntityType::STMT, "s1")), 
		*(new Declaration(EntityType::STMT, "s2")));
	
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
		RelationshipType::PARENT_S,
		*(new Declaration(EntityType::STMT, "s1")),
		*(new Declaration(EntityType::STMT, "s2")));

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
	REQUIRE(pkb.insertParent(11, 12));
	REQUIRE(pkb.insertParent(12, 13));
	REQUIRE(pkb.insertParent(2, 14));
	REQUIRE(pkb.insertParent(2, 15));
	REQUIRE(pkb.insertFollow(14, 15));

	pkb.init();

	unordered_map<string, set<string>> resultFollow = pkb.getResultsOfRS(
		RelationshipType::FOLLOWS_S,
		*(new Declaration(EntityType::STMT, "s1")),
		*(new Declaration(EntityType::STMT, "s2")));

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
		RelationshipType::PARENT_S,
		*(new Declaration(EntityType::STMT, "s1")),
		*(new Declaration(EntityType::STMT, "s2")));

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
}