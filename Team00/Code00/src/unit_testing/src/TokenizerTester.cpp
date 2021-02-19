#include "catch.hpp"
#include "Tokenizer.h"
#include <vector>

TEST_CASE("Test Tokenizer Ignores Whitespace")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta;";
	Tokenizer tokenizer = Tokenizer{ input };
	std::vector<std::unique_ptr<Token>> tokens;
	std::unique_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)27);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon };
	std::string tokenValues[] = { "read", "re", ";", "variable", "v", ";", "constant", "c", ";", "procedure", "pcd", ";",
		"print", "pn", ";", "while", "w", ";", "if", "ifs", ";", "stmt", "s1", ";", "assign", "a", ";" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 1")
{
	Tokenizer tokenizer = Tokenizer{ "assign a; variable v;\nSelect v such that Modifies(a, v) pattern a(_, _\"v\"_)" };
	std::vector<std::unique_ptr<Token>> tokens;
	std::unique_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)27);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Modifies, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::Underscore, TokenTypes::Comma,
		TokenTypes::Underscore, TokenTypes::DoubleQuote, TokenTypes::Identifier,
		TokenTypes::DoubleQuote, TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "assign", "a", ";", "variable", "v", ";", "Select", "v", "such", "that", "Modifies", "(",
		"a", ",", "v", ")", "pattern", "a", "(", "_", ",", "_", "\"", "v", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 2")
{
	Tokenizer tokenizer = Tokenizer{ "stmt s; print pn;\nSelect v such that Follows*(s, pn) pattern a(\"testVariable\", _\"v\"_)" };
	std::vector<std::unique_ptr<Token>> tokens;
	std::unique_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)30);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Follows, TokenTypes::Asterisk, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::DoubleQuote, TokenTypes::Identifier,
		TokenTypes::DoubleQuote, TokenTypes::Comma, TokenTypes::Underscore,
		TokenTypes::DoubleQuote, TokenTypes::Identifier, TokenTypes::DoubleQuote,
		TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "stmt", "s", ";", "print", "pn", ";", "Select", "v", "such", "that", "Follows", "*",
		"(", "s", ",", "pn", ")", "pattern", "a", "(", "\"", "testVariable", "\"", ",", "_", "\"", "v", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}

}

TEST_CASE("Test Combination 3")
{
	Tokenizer tokenizer = Tokenizer{ "read re;\nSelect re such that Parent(121, re) pattern a(\"dummyVariable\", _\"3230\"_)" };
	std::vector<std::unique_ptr<Token>> tokens;
	std::unique_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)26);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Parent, TokenTypes::LeftParen,
		TokenTypes::Integer, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::DoubleQuote, TokenTypes::Identifier,
		TokenTypes::DoubleQuote, TokenTypes::Comma, TokenTypes::Underscore, TokenTypes::DoubleQuote, TokenTypes::Integer,
		TokenTypes::DoubleQuote, TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "read", "re", ";", "Select", "re", "such", "that", "Parent", "(",
		"121", ",", "re", ")", "pattern", "a", "(", "\"", "dummyVariable", "\"", ",", "_", "\"", "3230", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 4")
{
	Tokenizer tokenizer = Tokenizer{ "variable var1;\nSelect a such that Uses(56, var1) pattern a(_, _\"9000\"_)" };
	std::vector<std::unique_ptr<Token>> tokens;
	std::unique_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)24);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Uses, TokenTypes::LeftParen,
		TokenTypes::Integer, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::Underscore, TokenTypes::Comma,
		TokenTypes::Underscore, TokenTypes::DoubleQuote, TokenTypes::Integer,
		TokenTypes::DoubleQuote, TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "variable", "var1", ";", "Select", "a", "such", "that", "Uses", "(",
		"56", ",", "var1", ")", "pattern", "a", "(", "_", ",", "_", "\"", "9000", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 5")
{
	Tokenizer tokenizer = Tokenizer{ "while w; if ifs;\nSelect w such that Parent * (w, ifs)" };
	std::vector<std::unique_ptr<Token>> tokens;
	std::unique_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)17);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Parent, TokenTypes::Asterisk, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen };
	std::string tokenValues[] = { "while", "w", ";", "if", "ifs", ";", "Select", "w", "such", "that", "Parent", "*",
		"(", "w", ",", "ifs", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}