#include "QueryParser.h"

#include <stdexcept>
#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end
#include <iostream>

// QE APIs, placed here temporarily for convenience of reference and stubbing ==============================================

void setSelectClause(std::shared_ptr<Declaration> declaration) {

}

void setRelationshipClause(RelationshipType relationshipType,
    std::shared_ptr<QueryInput> leftQueryInput, std::shared_ptr<QueryInput> rightQueryInput) {

}

void setPatternClause(std::shared_ptr<QueryInput> synonym,
    std::shared_ptr<QueryInput> queryInput, std::shared_ptr<Expression> expression) {

}

// =========================================================================================================================

QueryParser::QueryParser(const std::string givenInput) : tokenizer(givenInput)
{

}

void QueryParser::getNextToken()
{
    currToken = std::move(tokenizer.readNext());
}

bool QueryParser::canTreatAsIdent(TokenTypes type) {
    // Keyword TokenTypes which can also be treated as identifiers when accepting tokens
    TokenTypes keywordTypes[] = { TokenTypes::DesignEntity, TokenTypes::Modifies, TokenTypes::Uses, TokenTypes::Parent,
                TokenTypes::Follows, TokenTypes::Select, TokenTypes::Such, TokenTypes::That, TokenTypes::Pattern };
    return std::find(std::begin(keywordTypes), std::end(keywordTypes), type) != std::end(keywordTypes);
}

std::unique_ptr<Token> QueryParser::accept(TokenTypes type)
{
    if (!currToken) return std::unique_ptr<Token>();  // End of query reached
    if (currToken->getType() == type) {
        std::unique_ptr<Token> prevToken = std::move(currToken);
        getNextToken();
        return prevToken;
    }
    // Check if the current token is a keyword token (if so, it can be treated as identifier if we are accepting identifiers)
    if (type == TokenTypes::Identifier && canTreatAsIdent(currToken->getType())) {
        std::unique_ptr<Token> prevToken = std::move(currToken);
        getNextToken();
        return prevToken;
    }
    return std::unique_ptr<Token>();
}

std::unique_ptr<Token> QueryParser::expect(TokenTypes type)
{
    std::unique_ptr<Token> token = std::move(accept(type));
    if (token) {
        return token;
    }
    if (currToken) throw std::invalid_argument("Unexpected token encountered: " + currToken->toString());
    throw std::exception("End of query reached when a token was expected");
}

void QueryParser::selectClause()
{
    // Can have zero or more declarations
    while (declaration()) {

    }
    expect(TokenTypes::Select);
    std::unique_ptr<Token> selectedSynToken = std::move(expect(TokenTypes::Identifier));
    if (synonyms.find(selectedSynToken->getValue()) == synonyms.end()) {
        std::string errorMsg = "Undeclared synonym encountered in Select clause: " + selectedSynToken->getValue();
        throw std::exception(errorMsg.c_str());
    }
    auto declaration = std::make_shared<Declaration>(QueryInputType::Declaration,
        selectedSynToken->getValue(), synonyms[selectedSynToken->getValue()]);
    selectClauseDeclaration = declaration;
    setSelectClause(declaration);
    // Can have zero or one such that clause
    suchThatClause();
    // Can have zero or one pattern clause
    patternClause();
}

bool QueryParser::declaration()
{
    std::unique_ptr<Token> designEntity = std::move(accept(TokenTypes::DesignEntity));
    if (designEntity) {
        std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));
        synonyms[synToken->getValue()] = designEntity->getEntityType();
        std::cout << synToken->getValue() << "\n";
        while (accept(TokenTypes::Comma)) {
            synToken = std::move(expect(TokenTypes::Identifier));
            synonyms[synToken->getValue()] = designEntity->getEntityType();
            std::cout << synToken->getValue() << "\n";
            std::cout << Token::EntityTypeToString(designEntity->getEntityType()) << "\n";
        }
        expect(TokenTypes::Semicolon);
        return true;
    }
    return false;
}

bool QueryParser::suchThatClause()
{
    if (accept(TokenTypes::Such)) {
        expect(TokenTypes::That);
        relRef();
        return true;
    }
    return false;
}

bool QueryParser::patternClause()
{
    if (accept(TokenTypes::Pattern)) {
        std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));
        auto it = synonyms.find(synToken->getValue());
        if (it == synonyms.end()) {
            std::string errorMsg = "Undeclared synonym encountered in Pattern clause: " + synToken->getValue();
            throw std::exception(errorMsg.c_str());
        }
        auto synonym = std::make_shared<Declaration>(QueryInputType::Declaration, synToken->getValue(), synonyms[synToken->getValue()]);
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> queryInput = entRef();
        expect(TokenTypes::Comma);
        std::shared_ptr<Expression> expression = expressionSpec();
        expect(TokenTypes::RightParen);
        patternDeclaration = synonym;
        patternQueryInput = queryInput;
        patternExpression = expression;
        setPatternClause(synonym, queryInput, expression);
        return true;
    }
    return false;
}

void QueryParser::relRef()
{
    if (Modifies()) {

    }
    else if (Uses()) {

    }
    else if (Follows()) {

    }
    else if (Parent()) {

    }
    else {
        throw std::invalid_argument("Unexpected token encountered when parsing relRef: " + currToken->toString());
    }
}

std::shared_ptr<QueryInput> QueryParser::stmtRef()
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        auto it = synonyms.find(token->getValue());
        if (it == synonyms.end()) {
            std::string errorMsg = "Undeclared synonym encountered in StmtRef: " + token->getValue();
            throw std::exception(errorMsg.c_str());
        }
        return std::make_shared<Declaration>(QueryInputType::Declaration, token->getValue(), synonyms[token->getValue()]);
    }
    if (accept(TokenTypes::Underscore)) {
        return std::make_shared<Any>(QueryInputType::Any, token->getValue());
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        return std::make_shared<StmtNum>(QueryInputType::StmtNum, token->getValue());
    }
    throw std::invalid_argument("Unexpected token encountered when parsing stmtRef: " + currToken->toString());
}

std::shared_ptr<QueryInput> QueryParser::entRef()
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        auto it = synonyms.find(token->getValue());
        if (it == synonyms.end()) {
            std::string errorMsg = "Undeclared synonym encountered in EntRef: " + token->getValue();
            throw std::exception(errorMsg.c_str());
        }
        return std::make_shared<Declaration>(QueryInputType::Declaration, token->getValue(), synonyms[token->getValue()]);
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        return std::make_shared<Any>(QueryInputType::Any, token->getValue());
    }
    else if (accept(TokenTypes::DoubleQuote)) {
        token = std::move(expect(TokenTypes::Identifier));
        expect(TokenTypes::DoubleQuote);
        return std::make_shared<Ident>(QueryInputType::Ident, token->getValue());
    }
    else {
        throw std::invalid_argument("Unexpected token encountered when parsing entRef: " + currToken->toString());
    }
}

bool QueryParser::Modifies()
{
    if (accept(TokenTypes::Modifies)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef();
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef();
        expect(TokenTypes::RightParen);
        suchThatRelType = RelationshipType::Modifies;
        suchThatLeftQueryInput = leftQueryInput;
        suchThatRightQueryInput = rightQueryInput;
        setRelationshipClause(RelationshipType::Modifies, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::Uses()
{
    if (accept(TokenTypes::Uses)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef();
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef();
        expect(TokenTypes::RightParen);
        suchThatRelType = RelationshipType::Uses;
        suchThatLeftQueryInput = leftQueryInput;
        suchThatRightQueryInput = rightQueryInput;
        setRelationshipClause(RelationshipType::Uses, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::Parent()
{
    if (accept(TokenTypes::Parent)) {
        if (accept(TokenTypes::Asterisk)) {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = stmtRef();
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = stmtRef();
            expect(TokenTypes::RightParen);
            suchThatRelType = RelationshipType::ParentT;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            setRelationshipClause(RelationshipType::ParentT, leftQueryInput, rightQueryInput);
        }
        else {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = stmtRef();
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = stmtRef();
            expect(TokenTypes::RightParen);
            suchThatRelType = RelationshipType::Parent;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            setRelationshipClause(RelationshipType::Parent, leftQueryInput, rightQueryInput);
        }
        return true;
    }
    return false;
}

bool QueryParser::Follows()
{
    if (accept(TokenTypes::Follows)) {
        if (accept(TokenTypes::Asterisk)) {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = stmtRef();
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = stmtRef();
            expect(TokenTypes::RightParen);
            suchThatRelType = RelationshipType::FollowsT;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            setRelationshipClause(RelationshipType::FollowsT, leftQueryInput, rightQueryInput);
        }
        else {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = stmtRef();
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = stmtRef();
            expect(TokenTypes::RightParen);
            suchThatRelType = RelationshipType::Follows;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            setRelationshipClause(RelationshipType::Follows, leftQueryInput, rightQueryInput);
        }
        return true;
    }
    return false;
}

std::shared_ptr<Expression> QueryParser::expressionSpec()
{
    expect(TokenTypes::Underscore);
    std::unique_ptr<Token> token = std::move(subExpression());
    if (token) {
        expect(TokenTypes::Underscore);
        return std::make_shared<Expression>("_" + token->getValue() + "_");
    }
    return std::make_shared<Expression>("_");
}

std::unique_ptr<Token> QueryParser::subExpression()
{
    if (accept(TokenTypes::DoubleQuote)) {
        std::unique_ptr<Token> fctr = std::move(factor());
        if (!fctr) throw std::exception("Unexpected token found while parsing factor in subexpression of pattern");
        expect(TokenTypes::DoubleQuote);
        return fctr;
    }
    return NULL;
}

std::unique_ptr<Token> QueryParser::factor()
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        return token;
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        return token;
    }
    return NULL;
}

void QueryParser::parse()
{
    getNextToken();
    selectClause();
}

// Getters for testing

std::unordered_map<std::string, EntityType> QueryParser::getSynonyms()
{
    return synonyms;
}

std::shared_ptr<Declaration> QueryParser::getSelectClauseDeclaration()
{
    return selectClauseDeclaration;
}
std::shared_ptr<Declaration> QueryParser::getPatternDeclaration()
{
    return patternDeclaration;
}
std::shared_ptr<QueryInput> QueryParser::getPatternQueryInput()
{
    return patternQueryInput;
}
std::shared_ptr<Expression> QueryParser::getPatternExpression()
{
    return patternExpression;
}
RelationshipType QueryParser::getSuchThatRelType()
{
    return suchThatRelType;
}
std::shared_ptr<QueryInput> QueryParser::getSuchThatLeftQueryInput()
{
    return suchThatLeftQueryInput;
}
std::shared_ptr<QueryInput> QueryParser::getSuchThatRightQueryInput()
{
    return suchThatRightQueryInput;
}