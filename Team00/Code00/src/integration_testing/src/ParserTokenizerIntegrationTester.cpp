#include "catch.hpp"
#include "QueryParser.h"
#include "Query.h"
#include <vector>
#include <memory>

// ================= Positive Test Cases (No exceptions thrown) =================

TEST_CASE("Test whether Declared Synonyms are Stored Correctly")
{
	std::string input = "read re1, re2\t\n  ; variable\nv1,v2; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta1,a2,a3; Select v1;";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
	/*INFO(Token::EntityTypeToString(synonyms["re1"]));
	INFO(Token::EntityTypeToString(synonyms["re2"]));
	INFO(Token::EntityTypeToString(synonyms["v1"]));
	INFO(Token::EntityTypeToString(synonyms["v2"]));
	INFO(Token::EntityTypeToString(synonyms["c"]));
	INFO(Token::EntityTypeToString(synonyms["pcd"]));
	INFO(Token::EntityTypeToString(synonyms["pn"]));
	INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["ifs"]));
	INFO(Token::EntityTypeToString(synonyms["s1"]));
	INFO(Token::EntityTypeToString(synonyms["a1"]));*/

	REQUIRE(synonyms["re1"] == EntityType::READ);
	REQUIRE(synonyms["re2"] == EntityType::READ);
	REQUIRE(synonyms["v1"] == EntityType::VAR);
	REQUIRE(synonyms["v2"] == EntityType::VAR);
	REQUIRE(synonyms["c"] == EntityType::CONST);
	REQUIRE(synonyms["pcd"] == EntityType::PROC);
	REQUIRE(synonyms["pn"] == EntityType::PRINT);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["s1"] == EntityType::STMT);
	REQUIRE(synonyms["a1"] == EntityType::ASSIGN);
	REQUIRE(synonyms["a2"] == EntityType::ASSIGN);
	REQUIRE(synonyms["a3"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> declaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(declaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(declaration->getEntityType() == EntityType::VAR);
	REQUIRE(declaration->getValue() == "v1");
}

// ----------------- SelectCl + PatternCl -----------------

TEST_CASE("Test Query with Select And Pattern Clause1")
{
	std::string input = "assign a; \nSelect a pattern a(_, _\"9\"_) ";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	//INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<Declaration> patternClDeclaration = queryParser.getPatternDeclaration();
	std::shared_ptr<QueryInput> patternQueryInput = queryParser.getPatternQueryInput();
	std::shared_ptr<Expression> expression = queryParser.getPatternExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");
	REQUIRE(expression->getValue() == "9");
}

// ----------------- SelectCl + SuchThatCl -----------------

TEST_CASE("Test Query with Select And Such That Clause1")
{
	std::string input = "stmt s; print pn; \nSelect s such that Follows* (pn, s) ";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["s"]));
	INFO(Token::EntityTypeToString(synonyms["pn"]));*/

	REQUIRE(synonyms["pn"] == EntityType::PRINT);
	REQUIRE(synonyms["s"] == EntityType::STMT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::STMT);
	REQUIRE(selectClDeclaration->getValue() == "s");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> rightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "s");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::STMT);
}

TEST_CASE("Test Query with Select And Such That Clause2")
{
	std::string input = "variable v; \nSelect v such that Modifies (80, \"aVariable123\") ";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["v"]));*/

	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "v");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> rightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "80");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "aVariable123");
}

// ----------------- Test Follows -----------------

TEST_CASE("Test Follows(Any, Any)")
{
	std::string input = "assign a; \nSelect a such that Follows(_, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	//INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "_");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Follows(Any, Synonym)")
{
	std::string input = "assign a; if ifs; \nSelect a such that Follows(_, ifs)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "_");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "ifs");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::IF);
}

TEST_CASE("Test Follows(Any, Integer)")
{
	std::string input = "print pn;\nSelect pn such that Follows(_, 999)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "_");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "999");
}

TEST_CASE("Test Follows(Synonym, Any)")
{
	std::string input = "read re;\nSelect re such that Follows(re, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Follows(Synonym, Synonym)")
{
	std::string input = "read re; stmt s1;\nSelect re such that Follows(re, s1)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);
	REQUIRE(synonyms["s1"] == EntityType::STMT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::STMT);
}

TEST_CASE("Test Follows(Synonym, Integer)")
{
	std::string input = "read re;\nSelect re such that Follows(re, 999)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "999");
}

TEST_CASE("Test Follows(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Follows(3203, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Follows(Integer, Synonym)")
{
	std::string input = "while w;\nSelect w such that Follows(3203, w)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::WHILE);
}

TEST_CASE("Test Follows(Integer, Integer)")
{
	std::string input = "while w;\nSelect w such that Follows(3203, 4000)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "4000");
}

// ----------------- Test Parent -----------------

TEST_CASE("Test Parent(Any, Any)")
{
	std::string input = "assign a; \nSelect a such that Parent(_, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	//INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "_");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Parent(Any, Synonym)")
{
	std::string input = "assign a; if ifs; \nSelect a such that Parent(_, ifs)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "_");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "ifs");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::IF);
}

TEST_CASE("Test Parent(Any, Integer)")
{
	std::string input = "print pn;\nSelect pn such that Parent(_, 999)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "_");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "999");
}

TEST_CASE("Test Parent(Synonym, Any)")
{
	std::string input = "read re;\nSelect re such that Parent(re, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Parent(Synonym, Synonym)")
{
	std::string input = "read re; stmt s1;\nSelect re such that Parent(re, s1)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);
	REQUIRE(synonyms["s1"] == EntityType::STMT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::STMT);
}

TEST_CASE("Test Parent(Synonym, Integer)")
{
	std::string input = "read re;\nSelect re such that Parent(re, 999)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "999");
}

TEST_CASE("Test Parent(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Parent(3203, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Parent(Integer, Synonym)")
{
	std::string input = "while w;\nSelect w such that Parent(3203, w)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::WHILE);
}

TEST_CASE("Test Parent(Integer, Integer)")
{
	std::string input = "while w;\nSelect w such that Parent(3203, 4000)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "4000");
}

// ----------------- Test Modifies -----------------

TEST_CASE("Test Modifies(Synonym, Any)")
{
	std::string input = "read re;\nSelect re such that Modifies(re, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Modifies(Synonym, Synonym)")
{
	std::string input = "read re; variable v;\nSelect re such that Modifies(re, v)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Modifies(Synonym, \"Ident\")")
{
	std::string input = "read re;\nSelect re such that Modifies(re, \"hello1\")";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "hello1");
}

TEST_CASE("Test Modifies(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Modifies(3203, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Modifies(Integer, Synonym)")
{
	std::string input = "variable v;\nSelect v such that Modifies(3203, v)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "v");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Modifies(Integer, Ident)")
{
	std::string input = "assign a;\nSelect a such that Modifies(3203, \"aVariable\")";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "aVariable");
}

// ----------------- Test Uses -----------------

TEST_CASE("Test Uses(Synonym, Any)")
{
	std::string input = "print pn;\nSelect pn such that Uses(pn, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Uses(Synonym, Synonym)")
{
	std::string input = "print pn; constant c;\nSelect pn such that Uses(pn, c)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);
	REQUIRE(synonyms["c"] == EntityType::CONST);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "c");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::CONST);
}

TEST_CASE("Test Uses(Synonym, \"Ident\")")
{
	std::string input = "print pn;\nSelect pn such that Uses(pn, \"hello1\")";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "hello1");
}

TEST_CASE("Test Uses(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Uses(3203, _)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
}

TEST_CASE("Test Uses(Integer, Synonym)")
{
	std::string input = "variable v;\nSelect v such that Uses(3203, v)";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "v");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Uses(Integer, Ident)")
{
	std::string input = "assign a;\nSelect a such that Uses(3203, \"aVariable\")";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "3203");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "aVariable");
}

// ----------------- SelectCl + SuchThatCl + PatternCl -----------------

TEST_CASE("Test Query with Select, Such That And Pattern Clause 1")
{
	std::string input = "variable var1; assign a;\nSelect var1 such that Uses(56, var1) pattern a(_, _\"9000\"_) ";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["var1"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["var1"] == EntityType::VAR);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "var1");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "56");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "var1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	std::shared_ptr<Declaration> patternClDeclaration = queryParser.getPatternDeclaration();
	std::shared_ptr<QueryInput> patternQueryInput = queryParser.getPatternQueryInput();
	std::shared_ptr<Expression> expression = queryParser.getPatternExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");
	REQUIRE(expression->getValue() == "9000");
}

TEST_CASE("Test Query with Select, Such That And Pattern Clause 2")
{
	std::string input = "while w; assign a; \nSelect w such that Parent*(w, a) pattern a(\"xyz\", _\"xyz\"_) ";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::PARENT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);

	std::shared_ptr<Declaration> patternClDeclaration = queryParser.getPatternDeclaration();
	std::shared_ptr<QueryInput> patternQueryInput = queryParser.getPatternQueryInput();
	std::shared_ptr<Expression> expression = queryParser.getPatternExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "xyz");
	REQUIRE(expression->getValue() == "xyz");
}

// ----------------- Negative Test Cases (Appropriate Exceptions thrown) -----------------

TEST_CASE("Test Invalid Query without Select Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta;";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "End of query reached when a token was expected");
	}
}

TEST_CASE("Test synonym being redeclared as different synonym type")
{
	std::string input = "read re\t\n  ; variable\nv, re; Select re;";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym re with Entity Type of Read being redeclared as Entity Type of Variable");
	}
}

TEST_CASE("Test undeclared synonym in Select Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re1;";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in Select clause: re1");
	}
}

TEST_CASE("Test undeclared synonym in Pattern Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re pattern a1(v, _);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in Pattern clause: a1");
	}
}

TEST_CASE("Test synonym not of assignment type in Pattern Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re pattern pcd(v, _);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pcd not allowed, has Entity Type of Procedure");
	}
}

TEST_CASE("Test invalid expression spec in Pattern Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re pattern a(v, _\";\"_);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Unexpected token found while parsing factor in subexpression of pattern");
	}
}

TEST_CASE("Test undeclared synonym in Such That Clause Left Argument")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re1, ifs);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in StmtRef: re1");
	}
}

TEST_CASE("Test undeclared synonym in Such That Clause Right Argument")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(ifs, re1);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in StmtRef: re1");
	}
}

TEST_CASE("Test disallowed variable synonym in left argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(v, re);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed constant synonym in left argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(c, re);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed procedure synonym in left argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(pcd, re);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pcd not allowed, has Entity Type of Procedure");
	}
}

TEST_CASE("Test disallowed variable synonym in right argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed constant synonym in right argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re, c);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed procedure synonym in right argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re, pcd);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pcd not allowed, has Entity Type of Procedure");
	}
}

TEST_CASE("Test disallowed \"_\" in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Modifies(_, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Unexpected token encountered when parsing stmtRef: Token Type of Underscore with value: _");
	}
}

TEST_CASE("Test disallowed print synonym in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(pn, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pn not allowed, has Entity Type of Print");
	}
}

TEST_CASE("Test disallowed constant synonym in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(c, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed variable synonym in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(v, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed constant synonym in right argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(re, c);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed \"_\" in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(_, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Unexpected token encountered when parsing stmtRef: Token Type of Underscore with value: _");
	}
}

TEST_CASE("Test disallowed read synonym in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(re, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym re not allowed, has Entity Type of Read");
	}
}

TEST_CASE("Test disallowed constant synonym in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(c, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed variable synonym in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Uses(v, v);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed while synonym in right argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(pn, w);";
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "Synonym w not allowed, has Entity Type of While");
	}
}





