#include "catch.hpp"
#include "QueryParser.h"
#include <vector>

TEST_CASE("Test Invalid Query without Select Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta;";
	QueryParser queryParser = QueryParser{ input };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (std::exception const& err) {
		REQUIRE(std::string(err.what()) == "End of query reached when a token was expected");
	}
}

TEST_CASE("Test Declared Synonyms Stored Correctly")
{
	std::string input = "read re1, re2\t\n  ; variable\nv1,v2; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta1,a2,a3; Select v1;";
	QueryParser queryParser = QueryParser{ input };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
	INFO(Token::EntityTypeToString(synonyms["re1"]));
	INFO(Token::EntityTypeToString(synonyms["re2"]));
	INFO(Token::EntityTypeToString(synonyms["v1"]));
	INFO(Token::EntityTypeToString(synonyms["v2"]));
	INFO(Token::EntityTypeToString(synonyms["c"]));
	INFO(Token::EntityTypeToString(synonyms["pcd"]));
	INFO(Token::EntityTypeToString(synonyms["pn"]));
	INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["ifs"]));
	INFO(Token::EntityTypeToString(synonyms["s1"]));
	INFO(Token::EntityTypeToString(synonyms["a1"]));

	REQUIRE(synonyms["re1"] == EntityType::Read);
	REQUIRE(synonyms["re2"] == EntityType::Read);
	REQUIRE(synonyms["v1"] == EntityType::Variable);
	REQUIRE(synonyms["v2"] == EntityType::Variable);
	REQUIRE(synonyms["c"] == EntityType::Constant);
	REQUIRE(synonyms["pcd"] == EntityType::Procedure);
	REQUIRE(synonyms["pn"] == EntityType::Print);
	REQUIRE(synonyms["w"] == EntityType::While);
	REQUIRE(synonyms["ifs"] == EntityType::If);
	REQUIRE(synonyms["s1"] == EntityType::Stmt);
	REQUIRE(synonyms["a1"] == EntityType::Assign);
	REQUIRE(synonyms["a2"] == EntityType::Assign);
	REQUIRE(synonyms["a3"] == EntityType::Assign);

	std::shared_ptr<Declaration> declaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(declaration->getType() == QueryInputType::Declaration);
	REQUIRE(declaration->getEntityType() == EntityType::Variable);
	REQUIRE(declaration->getValue() == "v1");
}

TEST_CASE("Test Query with Select And Such That Clause1")
{
	std::string input = "stmt s; print pn; \nSelect s such that Follows* (pn, s) ";
	QueryParser queryParser = QueryParser{ input };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	INFO(Token::EntityTypeToString(synonyms["s"]));
	INFO(Token::EntityTypeToString(synonyms["pn"]));

	REQUIRE(synonyms["pn"] == EntityType::Print);
	REQUIRE(synonyms["s"] == EntityType::Stmt);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getType() == QueryInputType::Declaration);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::Stmt);
	REQUIRE(selectClDeclaration->getValue() == "s");

	RelationshipType relationshipType = queryParser.getSuchThatRelType();
	std::shared_ptr<QueryInput> leftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> rightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::FollowsT);
	REQUIRE(leftQueryInput->getType() == QueryInputType::Declaration);
	REQUIRE(leftQueryInput->getValue() == "pn");
	REQUIRE(rightQueryInput->getType() == QueryInputType::Declaration);
	REQUIRE(rightQueryInput->getValue() == "s");
}

TEST_CASE("Test Query with Select And Such That Clause2")
{
	std::string input = "variable v; \nSelect v such that Modifies (80, \"aVariable123\") ";
	QueryParser queryParser = QueryParser{ input };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	INFO(Token::EntityTypeToString(synonyms["v"]));

	REQUIRE(synonyms["v"] == EntityType::Variable);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getType() == QueryInputType::Declaration);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::Variable);
	REQUIRE(selectClDeclaration->getValue() == "v");

	RelationshipType relationshipType = queryParser.getSuchThatRelType();
	std::shared_ptr<QueryInput> leftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> rightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::Modifies);
	REQUIRE(leftQueryInput->getType() == QueryInputType::StmtNum);
	REQUIRE(leftQueryInput->getValue() == "80");
	REQUIRE(rightQueryInput->getType() == QueryInputType::Ident);
	REQUIRE(rightQueryInput->getValue() == "aVariable123");
}

TEST_CASE("Test Query with Select, Such That And Pattern Clause 1")
{
	std::string input = "variable var1; assign a;\nSelect var1 such that Uses(56, var1) pattern a(_, _\"9000\"_) ";
	QueryParser queryParser = QueryParser{ input };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	INFO(Token::EntityTypeToString(synonyms["var1"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["var1"] == EntityType::Variable);
	REQUIRE(synonyms["a"] == EntityType::Assign);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getType() == QueryInputType::Declaration);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::Variable);
	REQUIRE(selectClDeclaration->getValue() == "var1");

	RelationshipType relationshipType = queryParser.getSuchThatRelType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::Uses);
	REQUIRE(suchThatClLeftQueryInput->getType() == QueryInputType::StmtNum);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "56");
	REQUIRE(suchThatClRightQueryInput->getType() == QueryInputType::Declaration);
	REQUIRE(suchThatClRightQueryInput->getValue() == "var1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::Variable);

	std::shared_ptr<Declaration> patternClDeclaration = queryParser.getPatternDeclaration();
	std::shared_ptr<QueryInput> patternQueryInput = queryParser.getPatternQueryInput();
	std::shared_ptr<Expression> expression = queryParser.getPatternExpression();
	REQUIRE(patternClDeclaration->getType() == QueryInputType::Declaration);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::Assign);
	REQUIRE(patternQueryInput->getType() == QueryInputType::Any);
	REQUIRE(patternQueryInput->getValue() == "_");
	REQUIRE(expression->getValue() == "_9000_");
}

TEST_CASE("Test Query with Select, Such That And Pattern Clause 2")
{
	std::string input = "while w; assign a; \nSelect w such that Parent*(w, a) pattern a(\"xyz\", _\"xyz\"_) ";
	QueryParser queryParser = QueryParser{ input };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["w"] == EntityType::While);
	REQUIRE(synonyms["a"] == EntityType::Assign);

	std::shared_ptr<Declaration> selectClDeclaration = queryParser.getSelectClauseDeclaration();
	REQUIRE(selectClDeclaration->getType() == QueryInputType::Declaration);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::While);
	REQUIRE(selectClDeclaration->getValue() == "w");

	RelationshipType relationshipType = queryParser.getSuchThatRelType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = queryParser.getSuchThatLeftQueryInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = queryParser.getSuchThatRightQueryInput();
	REQUIRE(relationshipType == RelationshipType::ParentT);
	REQUIRE(suchThatClLeftQueryInput->getType() == QueryInputType::Declaration);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::While);
	REQUIRE(suchThatClRightQueryInput->getType() == QueryInputType::Declaration);
	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::Assign);

	std::shared_ptr<Declaration> patternClDeclaration = queryParser.getPatternDeclaration();
	std::shared_ptr<QueryInput> patternQueryInput = queryParser.getPatternQueryInput();
	std::shared_ptr<Expression> expression = queryParser.getPatternExpression();
	REQUIRE(patternClDeclaration->getType() == QueryInputType::Declaration);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::Assign);
	REQUIRE(patternQueryInput->getType() == QueryInputType::Ident);
	REQUIRE(patternQueryInput->getValue() == "xyz");
	REQUIRE(expression->getValue() == "_xyz_");
}


