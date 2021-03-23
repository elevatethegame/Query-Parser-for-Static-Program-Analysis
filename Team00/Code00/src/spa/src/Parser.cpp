#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>

using namespace std;

#include "Parser.h"
#include "PKB.h"
#include "TNode.h"
#include "SimpleParseError.h"
#include "SIMPLEToken.h"
#include "ParserHelper.h"
#include "ExpressionType.h"

Parser::Parser(DesignExtractor& extractor) : designExtractor(extractor) {
	this->numberOfStatements = 0;
	variables.clear();
	literals.clear();
}

void Parser::addLiterals(string literal) {
	if (literals.find(literal) != literals.end()) {
		literals.insert(literal);
	}
}

void Parser::addParentRelationship(int parent, int child) {
	if (parent != -1 && child != -1) {
		designExtractor.insertParent(parent, child);
	}
}

void Parser::addVariable(string variableName) {
	if (variables.find(variableName) == variables.end()) {
		variables.insert(variableName);
	}
}

void Parser::startAProcedure(string procName) {
	rangeProcedure[procName].first = this->expectNextStatementIndex();
	currentProcName = procName;
	procedures.insert(procName);
}

void Parser::endAProcedure(string procName) {
	rangeProcedure[procName].second = this->numberOfStatements;
}

int Parser::startNewStatement(int parentStatement, EntityType type) {
	int newIndex = ++numberOfStatements;
	this->designExtractor.increaseNumberOfStatement(type);
	this->addParentRelationship(parentStatement, newIndex);
	return newIndex;
}

void Parser::addUses(int statementId, const string& variable) {
	this->addVariable(variable);
	this->designExtractor.insertUses(statementId, variable);
}

void Parser::addModifies(int statementId, const string& variable) {
	this->addVariable(variable);
	this->designExtractor.insertModifies(statementId, variable);
}

void Parser::addFollowRelationship(int before, int after) {
	this->designExtractor.insertFollow(before, after);
}

void Parser::addExpression(int statementId, Expression expression) {
	this->designExtractor.insertExpression(statementId, expression);
}

void Parser::addCallingRelationship(int statementId, string callee) {
	this->callingRelationships[currentProcName].emplace_back(callee);
	this->designExtractor.insertStatementCall(statementId, callee);
}

ParseResult combineResult(ParseResult first, ParseResult second, SIMPLEToken operand) {
	ParseResult answer = FAILED;
	if (operand.getValue() == "(") {
		if (first != NONERESULT) {
			return FAILED;
		}
		if (second == CONDITION_N0) {
			return CONDITION_N1;
		}
		if (second == EXPRESSION) {
			return EXPRESSION;
		}
		return FAILED;
	}

	if (operand.getValue() == "!") {
		if (first != NONERESULT) {
			return FAILED;
		}
		if (second != CONDITION_N1) {
			return FAILED;
		}
		return CONDITION_N0;
	}

	if (operand.getTokenType() == TokenType::condSymbol) {
		if (first != CONDITION_N1) {
			return FAILED;
		}
		if (second != CONDITION_N1) {
			return FAILED;
		}
		return CONDITION_N0;
	}

	if (operand.getTokenType() == TokenType::relSymbol) {
		if (first != EXPRESSION) {
			return FAILED;
		}
		if (second != EXPRESSION) {
			return FAILED;
		}
		return CONDITION_N0;
	}

	if (operand.getTokenType() == TokenType::exprSymbol) {
		if (first != EXPRESSION) {
			return FAILED;
		}
		if (second != EXPRESSION) {
			return FAILED;
		}
		return EXPRESSION;
	}

	if (operand.getTokenType() == TokenType::termSymbol) {
		if (first != EXPRESSION) {
			return FAILED;
		}
		if (second != EXPRESSION) {
			return FAILED;
		}
		return EXPRESSION;
	}

	return FAILED;
}

ParseError consumeToken(const TokenType& type, SIMPLETokenStream& stream, SIMPLEToken& token) {
	///firstly, check whether the stream is empty
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "Expect more tokens");
	}

	///next, consume one token from the stream
	token = stream.getToken();
	if (token.getTokenType() != type) {
		return ParseError(token, "Invalid token");
	}
	else {
		return ParseError();
	}
}

ParseError consumeTerminal(const string& value, SIMPLETokenStream& stream) {
	//firstly, check whether the stream is empty
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "Expect more tokens");
	}

	///next, consume one token from the stream
	auto token = stream.getToken();
	if (token.getValue() != value) {
		return ParseError(token, "Invalid token");
	}
	else {
		return ParseError();
	}
}

ParseError Parser::parseProgram(SIMPLETokenStream &stream) {
	while (!stream.isEmpty()) {
		auto error = parseProcedure(stream);
		if (error.hasError()) {
			return error;
		}
	}
	if (checkCallOfNonProcedure(this->callingRelationships, this->procedures)) {
		return ParseError("There is a call of non-existance procedure");
	}
	if (checkCyclicCalls(this->callingRelationships)) {
		return ParseError("There is a cyclic call");
	}

	this->designExtractor.setCalls(callingRelationships);
	return ParseError();
}

ParseError Parser::parseProcedure(SIMPLETokenStream& stream) {
	SIMPLEToken procedureName;
	auto error = consumeTerminal("procedure", stream);
	error = error.combineWith(consumeToken(TokenType::name, stream, procedureName));
	
	///check duplication of procedure names
	if (procedures.find(procedureName.getValue()) != procedures.end())  {
		return ParseError("Two procedures have the same name");
	}

	this->startAProcedure(procedureName.getValue());
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream));
	error = error.combineWith(consumeTerminal("}", stream));
	this->endAProcedure(procedureName.getValue());
	return error;
}

ParseError Parser::parseAssignmentStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::ASSIGN);

	SIMPLEToken leftHandSide;
	Expression result("", ExpressionType::EMPTY);
	auto error = consumeToken(TokenType::name, stream, leftHandSide);
	error = error.combineWith(consumeTerminal("=", stream));
	error = error.combineWith(parseExpression(stream, result, thisStatementIndex));
	error = error.combineWith(consumeTerminal(";", stream));

	if (error.hasError()) {
		return error;
	}

	this->addModifies(thisStatementIndex, leftHandSide.getValue());
	return ParseError();
}

ParseError Parser::parseCallStatement(SIMPLETokenStream &stream, int parentStatementIndex) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::CALL);

	SIMPLEToken calleeName;
	auto error = consumeTerminal("call", stream);
	error = error.combineWith(consumeToken(TokenType::name, stream, calleeName));
	error = error.combineWith(consumeTerminal(";", stream));

	addCallingRelationship(thisStatementIndex, calleeName.getValue());

	return error;
}

ParseError Parser::parseReadStatement(SIMPLETokenStream &stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::READ);

	SIMPLEToken variable;
	auto error = consumeTerminal("read", stream);
	error = error.combineWith(consumeToken(TokenType::name, stream, variable));
	error = error.combineWith(consumeTerminal(";", stream));

	if (error.hasError()) {
		return error;
	}

	this->addModifies(thisStatementIndex, variable.getValue());
	return ParseError();
}

ParseError Parser::parsePrintStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::PRINT);
	SIMPLEToken variable;
	auto error = consumeTerminal("print", stream);
	error = error.combineWith(consumeToken(TokenType::name, stream, variable));
	error = error.combineWith(consumeTerminal(";", stream));;

	if (error.hasError()) {
		return error;
	}

	this->addUses(thisStatementIndex, variable.getValue());
	return ParseError();
}

ParseError Parser::parseWhileStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::WHILE);

	auto error = consumeTerminal("while", stream);
	error = error.combineWith(consumeTerminal("(", stream));
	error = error.combineWith(parseConditionExpression(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal(")", stream));
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal("}", stream));


	if (error.hasError()) {
		return error;
	}


	return ParseError();
}

ParseError Parser::parseIfStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::IF);

	auto error = consumeTerminal("if", stream);
	error = error.combineWith(consumeTerminal("(", stream));
	error = error.combineWith(parseConditionExpression(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal(")", stream));
	error = error.combineWith(consumeTerminal("then", stream));
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal("}", stream));
	error = error.combineWith(consumeTerminal("else", stream));
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal("}", stream));

	if (error.hasError()) {
		return error;
	}
	return ParseError();
}

ParseError Parser::parseStatementList(SIMPLETokenStream& stream, int parentStatementIndex) {
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "unexpected end of file");
	}

	vector<int> listIndices(0);
	while (!stream.isEmpty()) {
		vector<SIMPLEToken> nextTwos = stream.lookAhead(2);

		if (nextTwos.size() < 2) {
			break;
		} else
		if (nextTwos[0].getValue() == "while" && nextTwos[1].getValue() == "(") {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseWhileStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}
		} else
		if (nextTwos[0].getValue() == "if" && nextTwos[1].getValue() == "(") {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseIfStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else 
		if (nextTwos[0].getTokenType() == TokenType::name && nextTwos[1].getValue() == "=") {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseAssignmentStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else
		if (nextTwos[0].getValue() == "read" && nextTwos[1].getTokenType() == TokenType::name) {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseReadStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else 
		if (nextTwos[0].getValue() == "print" && nextTwos[1].getTokenType() == TokenType::name) {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parsePrintStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else 
		if (nextTwos[0].getValue() == "call" && nextTwos[1].getTokenType() == TokenType::name) {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseCallStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}
		} else {
			break;
		}
	}
	
	//TODO
	if (listIndices.size() == 0) {
		return ParseError(stream.getLastConsumedToken(), "statement list should not be empty");
	}

	for (int i = 0; i + 1 < listIndices.size(); i++) {
		this->addFollowRelationship(listIndices[i], listIndices[i + 1]);
	}

	return ParseError();
}

ParseError Parser::parseFactor(SIMPLETokenStream& stream, Expression& result, int userStatement) {
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "unexpected end of file");
	}
	SIMPLEToken nextToken = stream.getToken();

	if (nextToken.getTokenType() == TokenType::name) {
		this->addUses(userStatement, nextToken.getValue());
		result = Expression(nextToken.getValue(), ExpressionType::PARTIAL);
		this->addExpression(userStatement, result);
		return ParseError();
	}

	if (nextToken.getTokenType() == TokenType::integer) {
		result = Expression(nextToken.getValue(), ExpressionType::PARTIAL);
		this->addExpression(userStatement, result);
		this->designExtractor.insertConstant(nextToken.getValue());
		return ParseError();
	}

	if (nextToken.getValue() == "(") {
		auto error = parseExpression(stream, result, userStatement);
		error = error.combineWith(consumeTerminal(")", stream));
		return error;
	}

	return ParseError(nextToken, "unexpected token");
}

ParseError Parser::parseTerm(SIMPLETokenStream& stream, Expression& result, int userStatement) {
	auto error = parseFactor(stream, result, userStatement);

	while (!stream.isEmpty()) {
		SIMPLEToken nextToken = stream.lookAheadSingle();
		if (nextToken.getTokenType() == TokenType::termSymbol) {
			Expression subResult("", ExpressionType::EMPTY);
			error = error.combineWith(consumeToken(TokenType::termSymbol, stream, nextToken));
			error = error.combineWith(parseFactor(stream, subResult, userStatement));
			result = result.combineExpression(nextToken.getValue(), subResult, ExpressionType::PARTIAL);
		}
		else {
			return error;
		}
	}

	this->addExpression(userStatement, result);
	return error;
}

ParseError Parser::parseExpression(SIMPLETokenStream& stream, Expression& result, int userStatement) {
	auto error = parseTerm(stream, result, userStatement);

	while (!stream.isEmpty()) {
		SIMPLEToken nextToken = stream.lookAheadSingle();
		if (nextToken.getTokenType() == TokenType::exprSymbol) {
			Expression subResult("", ExpressionType::EMPTY);
			error = error.combineWith(consumeToken(TokenType::exprSymbol, stream, nextToken));
			error = error.combineWith(parseTerm(stream, subResult, userStatement));
			result = result.combineExpression(nextToken.getValue(), subResult, ExpressionType::PARTIAL);
		}
		else {
			break;
		}
	}

	this->addExpression(userStatement, result);
	return error;
}

ParseError Parser::parseConditionExpression(SIMPLETokenStream& stream, int userStatement) {
	ParseResult result = ParseResult::FAILED;
	auto error = evaluateConditionOrExpression(0, stream, result, NONERESULT, userStatement);
	if (result != ParseResult::CONDITION_N0) {
		return ParseError(SIMPLEToken(), "wrong condition expression");
	}
	return error;
}

ParseError Parser::evaluateConditionOrExpression(int level, SIMPLETokenStream& stream, ParseResult& result, ParseResult currentResult, int userStatement) {
	if (stream.isEmpty()) {
		//stop
		result = currentResult;
		return ParseError();
	}

	auto error = ParseError();
	result = FAILED;

	if (currentResult == ParseResult::NONERESULT) {
		SIMPLEToken nextToken = stream.lookAheadSingle();

		bool receiveTerm = false;
		if (nextToken.getValue() == "(") {
			receiveTerm = true;
			auto error = consumeTerminal("(", stream);
			ParseResult subResult = FAILED;
			error = error.combineWith(evaluateConditionOrExpression(0, stream, subResult, NONERESULT, userStatement));
			error = error.combineWith(consumeTerminal(")", stream));
			switch (subResult) {
			case ParseResult::EXPRESSION:
				currentResult = EXPRESSION;
				break;
			case ParseResult::CONDITION_N0:
				currentResult = ParseResult::CONDITION_N1;
				break;
			default:
				return ParseError(nextToken, "wrong expression");
			}
		}
		else
			if (nextToken.getTokenType() == TokenType::name || nextToken.getTokenType() == TokenType::integer) {
				receiveTerm = true;
				error = consumeToken(nextToken.getTokenType(), stream, nextToken);
				if (nextToken.getTokenType() == TokenType::name) {
					this->addUses(userStatement, nextToken.getValue());
				}
				if (nextToken.getTokenType() == TokenType::integer) {
					this->designExtractor.insertConstant(nextToken.getValue());
				}
				currentResult = EXPRESSION;
			}

		if (receiveTerm) {
			error = error.combineWith(evaluateConditionOrExpression(level, stream, result, currentResult, userStatement));
			return error;
		}
	}

	auto nextToken = stream.lookAheadSingle();

	int currentRank = static_cast<int>(nextToken.getTokenType());
	if (currentRank <= 0 || currentRank >= 6) {
		//stop
		result = currentResult;
		return ParseError();
	}

	if (currentRank > level) {
		ParseResult subResult = FAILED;
		auto error = consumeToken(nextToken.getTokenType(), stream, nextToken);
		error = error.combineWith(evaluateConditionOrExpression(currentRank, stream, subResult, NONERESULT, userStatement));
		currentResult = combineResult(currentResult, subResult, nextToken);

		///TODO if currentResult is a FAILED 
		if (currentResult == FAILED) {
			return ParseError(nextToken, "wrong condition");
		}
		error = error.combineWith(evaluateConditionOrExpression(level, stream, result, currentResult, userStatement));
		return error;
	}
	else {
		result = currentResult;
		return error;
	}
}