#include "catch.hpp"
#include "QueryParser.h"
#include "TokenizerStub.h"
#include "Query.h"
#include <vector>
#include <memory>

// ----------------- SelectCl + SuchThatCl + PatternCl -----------------

TEST_CASE("Test Query with Select, Such That And Pattern Clause 1")
{
	std::string input = "variable var1; assign a;\nSelect var1 such that Uses(56, var1) pattern a(_, _\"9000\"_) ";
	auto query = std::make_shared<Query>();
	std::vector<Token> tokens{Token(TokenTypes::DesignEntity, "variable"), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Semicolon, ";"),
			Token(TokenTypes::DesignEntity, "assign"), Token(TokenTypes::Identifier, "a"), Token(TokenTypes::Semicolon, ";"), 
			Token(TokenTypes::Select, "Select"), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Such, "such"),
			Token(TokenTypes::That, "that"), Token(TokenTypes::Uses, "Uses"), Token(TokenTypes::LeftParen, "("),
			Token(TokenTypes::Integer, "56"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Identifier, "var1"),
			Token(TokenTypes::RightParen, ")"), Token(TokenTypes::Pattern, "pattern"), Token(TokenTypes::Identifier, "a"), 
			Token(TokenTypes::LeftParen, "("), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","),
			Token(TokenTypes::Underscore, "_"), Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Integer, "9000"),
			Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::RightParen, ")")
	};
	auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["var1"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["var1"] == EntityType::VAR);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "var1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "56");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "var1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");
	REQUIRE(expression->getValue() == "9000");
	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
}

//TEST_CASE("Test Query with Select, Such That And Pattern Clause 2")
//{
//	std::string input = "while w; assign a; \nSelect w such that Parent*(w, a) pattern a(\"xyz\", _\"xyz\"_) ";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	/*INFO(Token::EntityTypeToString(synonyms["w"]));
//	INFO(Token::EntityTypeToString(synonyms["a"]));*/
//
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(selectClDeclaration->getValue() == "w");
//
//	std::shared_ptr<RelationshipClause> relationshipCl = query->getRelationshipClauses().at(0);
//	RelationshipType relationshipType = relationshipCl->getRelationshipType();
//	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::PARENT_T);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);
//
//	std::shared_ptr<PatternClause> patternCl = query->getPatternClauses().at(0);
//	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
//	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
//	std::shared_ptr<Expression> expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "xyz");
//	REQUIRE(expression->getValue() == "xyz");
//	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
//}
//
//// ----------------- SelectCl + Multiple PatternCl / Single PatternCl with multiple 'and' -----------------
//
//TEST_CASE("Test Query with Select, And Multiple Pattern Clause 1")
//{
//	std::string input = "assign a; if ifs; while w; variable v; \nSelect w pattern a(\"xyz\", \"xyz\") "
//		" pattern ifs(v,_,_) pattern w(_, _) pattern w(\"x\", _)";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["ifs"] == EntityType::IF);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(selectClDeclaration->getValue() == "w");
//
//	std::shared_ptr<PatternClause> patternCl = query->getPatternClauses().at(0);
//	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
//	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
//	std::shared_ptr<Expression> expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "xyz");
//	REQUIRE(expression->getValue() == "xyz");
//	REQUIRE(expression->getType() == ExpressionType::EXACT);
//
//	patternCl = query->getPatternClauses().at(1);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "ifs");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//
//	patternCl = query->getPatternClauses().at(2);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "w");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
//	REQUIRE(patternQueryInput->getValue() == "_");
//
//	patternCl = query->getPatternClauses().at(3);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "w");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "x");
//}
//
//TEST_CASE("Test Query with Select, And Single Pattern Clause with Multiple 'and' ")
//{
//	std::string input = "assign a; if ifs; while w; variable v; \nSelect w pattern a(\"xyz\", _\"xyz\"_) and"
//		" ifs(v,_,_) and w(_, _) and w(\"x\", _) and a(v, \"5 % 2 + 7 - var1 * var2 + var3 / var4 \")";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["ifs"] == EntityType::IF);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(selectClDeclaration->getValue() == "w");
//
//	std::shared_ptr<PatternClause> patternCl = query->getPatternClauses().at(0);
//	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
//	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
//	std::shared_ptr<Expression> expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "xyz");
//	REQUIRE(expression->getValue() == "xyz");
//	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
//
//	patternCl = query->getPatternClauses().at(1);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "ifs");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//
//	patternCl = query->getPatternClauses().at(2);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "w");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
//	REQUIRE(patternQueryInput->getValue() == "_");
//
//	patternCl = query->getPatternClauses().at(3);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "w");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "x");
//
//	patternCl = query->getPatternClauses().at(4);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//	REQUIRE(expression->getValue() == "((((5%2)+7)-(var1*var2))+(var3/var4))");
//	REQUIRE(expression->getType() == ExpressionType::EXACT);
//}
//
//// ----------------- SelectCl + Multiple SuchThatCl / Single SuchThatCl with multiple 'and' -----------------
//
//TEST_CASE("Test Query with Select, And Multiple Such That Clauses 1")
//{
//	std::string input = "assign a; if ifs; while w; variable v; \nSelect a such that Modifies(a, v) "
//		" such that Uses(ifs, v) such that Parent*(w, a) such that Calls (\"x\", \"y\")";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	/*INFO(Token::EntityTypeToString(synonyms["w"]));
//	INFO(Token::EntityTypeToString(synonyms["a"]));*/
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["ifs"] == EntityType::IF);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(selectClDeclaration->getValue() == "a");
//
//	std::shared_ptr<RelationshipClause> relationshipCl = query->getRelationshipClauses().at(0);
//	RelationshipType relationshipType = relationshipCl->getRelationshipType();
//	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::MODIFIES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
//
//	relationshipCl = query->getRelationshipClauses().at(1);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::USES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "ifs");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::IF);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
//
//	relationshipCl = query->getRelationshipClauses().at(2);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::PARENT_T);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);
//
//	relationshipCl = query->getRelationshipClauses().at(3);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::CALLS);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "x");
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "y");
//
//}
//
//TEST_CASE("Test Query with Select, And Single Such That Clause with Multiple 'and' ")
//{
//	std::string input = "assign a; prog_line pgl1, pgl2; while w; variable v; \nSelect a such that Follows(a, w) "
//		" and Next(5, 6) and Next*(pgl1, pgl2) and Modifies (7, \"y\")";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	/*INFO(Token::EntityTypeToString(synonyms["w"]));
//	INFO(Token::EntityTypeToString(synonyms["a"]));*/
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
//	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(selectClDeclaration->getValue() == "a");
//
//	std::shared_ptr<RelationshipClause> relationshipCl = query->getRelationshipClauses().at(0);
//	RelationshipType relationshipType = relationshipCl->getRelationshipType();
//	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "w");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::WHILE);
//
//	relationshipCl = query->getRelationshipClauses().at(1);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::NEXT);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "5");
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "6");
//
//	relationshipCl = query->getRelationshipClauses().at(2);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::NEXT_T);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "pgl2");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::PROGLINE);
//
//	relationshipCl = query->getRelationshipClauses().at(3);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::MODIFIES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "7");
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "y");
//}
//
//TEST_CASE("Test Query with Select and Multiple Such That Clause each having Multiple 'and' ")
//{
//	std::string input = "assign a; prog_line pgl1, pgl2; procedure pcd; while w; variable v; \nSelect a such that Follows(a, w) "
//		" and Next(5, 6) such that Next*(pgl1, pgl2) and Modifies (7, \"y\") such that Uses(a, v) and Calls(pcd, \"helloWorldProcedure\")";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
//	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
//	REQUIRE(synonyms["pcd"] == EntityType::PROC);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(selectClDeclaration->getValue() == "a");
//
//	std::shared_ptr<RelationshipClause> relationshipCl = query->getRelationshipClauses().at(0);
//	RelationshipType relationshipType = relationshipCl->getRelationshipType();
//	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "w");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::WHILE);
//
//	relationshipCl = query->getRelationshipClauses().at(1);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::NEXT);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "5");
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "6");
//
//	relationshipCl = query->getRelationshipClauses().at(2);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::NEXT_T);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "pgl2");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::PROGLINE);
//
//	relationshipCl = query->getRelationshipClauses().at(3);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::MODIFIES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "7");
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "y");
//
//	relationshipCl = query->getRelationshipClauses().at(4);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::USES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
//
//	relationshipCl = query->getRelationshipClauses().at(5);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::CALLS);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "pcd");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROC);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "helloWorldProcedure");
//}
//
//// ----------------- SelectCl + Multiple SuchThatCl + Multiple pattern interleaved -----------------
//
//TEST_CASE("Test Query with Select, And Multiple Such That Clauses, And Multiple pattern clauses interleaved 1")
//{
//	std::string input = "assign a; if ifs; while w; variable v; \nSelect a pattern a(\"x\", \"7 * (3 + 2) / 4 - 3\") such that Modifies(a, v) "
//		" pattern ifs(v,_,_) such that Uses(ifs, v) pattern w(v,_) such that Parent*(w, a) and Calls (\"x\", \"y\")";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	/*INFO(Token::EntityTypeToString(synonyms["w"]));
//	INFO(Token::EntityTypeToString(synonyms["a"]));*/
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["ifs"] == EntityType::IF);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(selectClDeclaration->getValue() == "a");
//
//	std::shared_ptr<RelationshipClause> relationshipCl = query->getRelationshipClauses().at(0);
//	RelationshipType relationshipType = relationshipCl->getRelationshipType();
//	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::MODIFIES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
//
//	relationshipCl = query->getRelationshipClauses().at(1);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::USES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "ifs");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::IF);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
//
//	relationshipCl = query->getRelationshipClauses().at(2);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::PARENT_T);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);
//
//	relationshipCl = query->getRelationshipClauses().at(3);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::CALLS);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "x");
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "y");
//
//	std::shared_ptr<PatternClause> patternCl = query->getPatternClauses().at(0);
//	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
//	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
//	std::shared_ptr<Expression> expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "x");
//	REQUIRE(expression->getValue() == "(((7*(3+2))/4)-3)");
//	REQUIRE(expression->getType() == ExpressionType::EXACT);
//
//	patternCl = query->getPatternClauses().at(1);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "ifs");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//
//	patternCl = query->getPatternClauses().at(2);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "w");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//}
//
//TEST_CASE("Test Query with Select, And Multiple Such That Clauses, And Multiple pattern clauses interleaved 2")
//{
//	std::string input = "assign a; if ifs; while w; variable v; stmt s1; prog_line pgl1, pgl2; procedure pcd1;"
//		"\nSelect s1 pattern a(v,_) and ifs(v,_,_)"
//		"such that Next(pgl1, pgl2) and Calls*(pcd1, _) pattern w(\"controlVar\" , _) and a(\"x\", _\"y\"_)"
//		"such that Modifies(s1, v) and Parent(s1, a)";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	queryParser.parse();
//	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();
//
//	/*INFO(Token::EntityTypeToString(synonyms["w"]));
//	INFO(Token::EntityTypeToString(synonyms["a"]));*/
//
//	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
//	REQUIRE(synonyms["ifs"] == EntityType::IF);
//	REQUIRE(synonyms["w"] == EntityType::WHILE);
//	REQUIRE(synonyms["v"] == EntityType::VAR);
//	REQUIRE(synonyms["s1"] == EntityType::STMT);
//	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
//	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
//	REQUIRE(synonyms["pcd1"] == EntityType::PROC);
//
//	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
//	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(selectClDeclaration->getEntityType() == EntityType::STMT);
//	REQUIRE(selectClDeclaration->getValue() == "s1");
//
//	std::shared_ptr<RelationshipClause> relationshipCl = query->getRelationshipClauses().at(0);
//	RelationshipType relationshipType = relationshipCl->getRelationshipType();
//	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::NEXT);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "pgl2");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::PROGLINE);
//
//	relationshipCl = query->getRelationshipClauses().at(1);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::CALLS_T);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "pcd1");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROC);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "_");
//
//	relationshipCl = query->getRelationshipClauses().at(2);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::MODIFIES);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "s1");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::STMT);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
//
//	relationshipCl = query->getRelationshipClauses().at(3);
//	relationshipType = relationshipCl->getRelationshipType();
//	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
//	suchThatClRightQueryInput = relationshipCl->getRightInput();
//	REQUIRE(relationshipType == RelationshipType::PARENT);
//	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClLeftQueryInput->getValue() == "s1");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::STMT);
//	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);
//
//	std::shared_ptr<PatternClause> patternCl = query->getPatternClauses().at(0);
//	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
//	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
//	std::shared_ptr<Expression> expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//	REQUIRE(expression->getValue() == "_");
//	REQUIRE(expression->getType() == ExpressionType::EMPTY);
//
//	patternCl = query->getPatternClauses().at(1);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "ifs");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternQueryInput->getValue() == "v");
//	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
//
//	patternCl = query->getPatternClauses().at(2);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "w");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "controlVar");
//
//	patternCl = query->getPatternClauses().at(3);
//	patternClDeclaration = patternCl->getSynonym();
//	patternQueryInput = patternCl->getQueryInput();
//	expression = patternCl->getExpression();
//	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
//	REQUIRE(patternClDeclaration->getValue() == "a");
//	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
//	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
//	REQUIRE(patternQueryInput->getValue() == "x");
//	REQUIRE(expression->getValue() == "y");
//	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
//}
//
//// ----------------- Negative Test Cases (Appropriate Exceptions thrown) -----------------
//
//TEST_CASE("Test Invalid Query without Select Clause")
//{
//	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
//		"stmt s1; assign\n\ta;";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	try {
//		queryParser.parse();
//		REQUIRE(false);
//	}
//	catch (std::exception const& err) {
//		REQUIRE(std::string(err.what()) == "End of query reached when a token was expected");
//	}
//}
//
//TEST_CASE("Test synonym being redeclared as different synonym type")
//{
//	std::string input = "read re\t\n  ; variable\nv, re; Select re;";
//	auto query = std::make_shared<Query>();
//	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
//	QueryParser queryParser = QueryParser{ tokenizer, query };
//	try {
//		queryParser.parse();
//		REQUIRE(false);
//	}
//	catch (std::exception const& err) {
//		REQUIRE(std::string(err.what()) == "Synonym re with Entity Type of Read being redeclared as Entity Type of Variable");
//	}
//}