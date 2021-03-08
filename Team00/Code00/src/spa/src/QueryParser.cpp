#include "QueryParser.h"

#include <stdexcept>
#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end
#include <iostream>

// =========================================================================================================================

QueryParser::QueryParser(const std::string givenInput, std::shared_ptr<QueryInterface> query) : tokenizer(givenInput), query(query)
{

}

void QueryParser::getNextToken()
{
    currToken = std::move(tokenizer.readNext());
}

bool QueryParser::canTreatAsIdent(TokenTypes type) {
    // Keyword TokenTypes which can also be treated as identifiers when accepting tokens
    TokenTypes keywordTypes[] = { TokenTypes::DesignEntity, TokenTypes::Modifies, TokenTypes::Uses, TokenTypes::Parent,
                TokenTypes::Follows, TokenTypes::Select, TokenTypes::Such, TokenTypes::That, TokenTypes::Pattern,
                TokenTypes::And };
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
    throw std::runtime_error("End of query reached when a token was expected");
}

// Expect a stmtRef or an entRef
std::shared_ptr<QueryInput> QueryParser::expect(std::shared_ptr<QueryInput> queryInput, bool isStmtRef)
{
    if (!queryInput) {
        if (isStmtRef)
            throw std::invalid_argument("Unexpected token encountered when parsing stmtRef: " + currToken->toString());
        else
            throw std::invalid_argument("Unexpected token encountered when parsing entRef: " + currToken->toString());
    }
    return queryInput;
}

void QueryParser::selectClause()
{
    // Can have zero or more declarations
    while (declaration()) {

    }
    expect(TokenTypes::Select);
    std::unique_ptr<Token> selectedSynToken = std::move(expect(TokenTypes::Identifier));

    // Semantic check: Undeclared Synonym used in Select Clause
    if (synonyms.find(selectedSynToken->getValue()) == synonyms.end()) {
        std::string errorMsg = "Undeclared synonym encountered in Select clause: " + selectedSynToken->getValue();
        throw std::runtime_error(errorMsg.c_str());
    }

    auto declaration = std::make_shared<Declaration>(synonyms[selectedSynToken->getValue()], selectedSynToken->getValue());
    selectClauseDeclaration = declaration;
    query->setSelectClause(declaration);

    // Can have any number of such-that and pattern clauses in any order
    while (suchThatClause() || patternClause()) {

    }

}

bool QueryParser::declaration()
{
    std::unique_ptr<Token> designEntity = std::move(accept(TokenTypes::DesignEntity));
    if (designEntity) {
        std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));
        auto it = synonyms.find(synToken->getValue());
        // Throw an error if synonyms are redeclared as a different entity type
        if (it != synonyms.end() && it->second != designEntity->getEntityType()) {
            std::string errorMsg = "Synonym " + synToken->getValue() + " with " + Token::EntityTypeToString(it->second) 
                + " being redeclared as " + Token::EntityTypeToString(designEntity->getEntityType());
            throw std::runtime_error(errorMsg.c_str());
        }
        synonyms[synToken->getValue()] = designEntity->getEntityType();
        // std::cout << synToken->getValue() << "\n";
        while (accept(TokenTypes::Comma)) {
            synToken = std::move(expect(TokenTypes::Identifier));
            auto it = synonyms.find(synToken->getValue());
            // Throw an error if synonyms are redeclared as a different entity type
            if (it != synonyms.end() && it->second != designEntity->getEntityType()) {
                std::string errorMsg = "Synonym " + synToken->getValue() + " with " + Token::EntityTypeToString(it->second)
                    + " being redeclared as " + Token::EntityTypeToString(designEntity->getEntityType());
                throw std::runtime_error(errorMsg.c_str());
            }
            synonyms[synToken->getValue()] = designEntity->getEntityType();
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
        while (accept(TokenTypes::And)) {  // Can have any number of 'and' relRef
            relRef();
        }
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
            throw std::runtime_error(errorMsg.c_str());
        }
        EntityType synonymType = it->second;
        if (synonymType != EntityType::ASSIGN) {  // Only allow entity type of Assignment for pattern clause
            std::string errorMsg = "Synonym " + synToken->getValue() + " not allowed, has " + Token::EntityTypeToString(synonymType);
            throw std::runtime_error(errorMsg.c_str());
        }
        auto synonym = std::make_shared<Declaration>(synonyms[synToken->getValue()], synToken->getValue());
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> queryInput = expect(entRef(std::set<EntityType>({ EntityType::VAR }), true), false);
        expect(TokenTypes::Comma);
        std::shared_ptr<Expression> expression = expressionSpec();
        expect(TokenTypes::RightParen);
        patternDeclaration = synonym;
        patternQueryInput = queryInput;
        patternExpression = expression;
        query->addPatternClause(synonym, queryInput, expression);
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

std::shared_ptr<QueryInput> QueryParser::stmtRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore)
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        auto it = synonyms.find(token->getValue());
        if (it == synonyms.end()) {
            std::string errorMsg = "Undeclared synonym encountered in StmtRef: " + token->getValue();
            throw std::runtime_error(errorMsg.c_str());
        }
        EntityType synonymType = it->second;
        if (allowedDesignEntities.find(synonymType) == allowedDesignEntities.end()) {
            std::string errorMsg = "Synonym " + token->getValue() + " not allowed, has " + Token::EntityTypeToString(synonymType);
            throw std::runtime_error(errorMsg.c_str());
        }
        return std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        if (acceptsUnderscore) {
            return std::make_shared<Any>(token->getValue());
        }
        throw std::invalid_argument(token->toString() + " is not allowed as first argument in stmtRef");
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        return std::make_shared<StmtNum>(token->getValue());
    }
    return NULL;
}

std::shared_ptr<QueryInput> QueryParser::entRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore)
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        auto it = synonyms.find(token->getValue());
        if (it == synonyms.end()) {
            std::string errorMsg = "Undeclared synonym encountered in EntRef: " + token->getValue();
            throw std::runtime_error(errorMsg.c_str());
        }
        EntityType synonymType = it->second;
        if (allowedDesignEntities.find(synonymType) == allowedDesignEntities.end()) {
            std::string errorMsg = "Synonym " + token->getValue() + " not allowed, has " + Token::EntityTypeToString(synonymType);
            throw std::runtime_error(errorMsg.c_str());
        }
        return std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        if (acceptsUnderscore) {
            return std::make_shared<Any>(token->getValue());
        }
        throw std::invalid_argument(token->toString() + " is not allowed as first argument in entRef");
    }
    else if (accept(TokenTypes::DoubleQuote)) {
        token = std::move(expect(TokenTypes::Identifier));
        expect(TokenTypes::DoubleQuote);
        return std::make_shared<Ident>(token->getValue());
    }
    else {
        return NULL;
    }
}

bool QueryParser::Modifies()
{
    if (accept(TokenTypes::Modifies)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(std::set<EntityType>(
            {EntityType::ASSIGN, EntityType::STMT, EntityType::READ, EntityType::PROC, EntityType::IF, EntityType::WHILE }) 
            , false);
        if (!leftQueryInput) {
            leftQueryInput = expect(entRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::READ, EntityType::PROC, EntityType::IF, EntityType::WHILE })
                , false)
                , false);
        }
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = expect(entRef(std::set<EntityType>({ EntityType::VAR }), true), false);
        expect(TokenTypes::RightParen);
        suchThatRelationshipType = RelationshipType::MODIFIES;
        suchThatLeftQueryInput = leftQueryInput;
        suchThatRightQueryInput = rightQueryInput;
        query->addRelationshipClause(RelationshipType::MODIFIES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::Uses()
{
    if (accept(TokenTypes::Uses)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(std::set<EntityType>(
            { EntityType::ASSIGN, EntityType::STMT, EntityType::PRINT, EntityType::PROC, EntityType::IF, EntityType::WHILE })
            , false);
        if (!leftQueryInput) {
            leftQueryInput = expect(entRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::PRINT, EntityType::PROC, EntityType::IF, EntityType::WHILE })
                , false)
                , false);
        }
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = expect(entRef(std::set<EntityType>({ EntityType::VAR }), true), false);
        expect(TokenTypes::RightParen);
        suchThatRelationshipType = RelationshipType::USES;
        suchThatLeftQueryInput = leftQueryInput;
        suchThatRightQueryInput = rightQueryInput;
        query->addRelationshipClause(RelationshipType::USES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::Parent()
{
    if (accept(TokenTypes::Parent)) {
        if (accept(TokenTypes::Asterisk)) {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = expect(
                stmtRef(std::set<EntityType>({ EntityType::STMT, EntityType::WHILE, EntityType::IF }) , true)
                    , true);
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = expect(stmtRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ }), true)
                    , true);
            expect(TokenTypes::RightParen);
            // Semantic checks for Parent
            // Cannot have same synonym on both sides
            if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION
                && rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
                if (leftQueryInput->getValue() == rightQueryInput->getValue())
                    throw std::runtime_error("Same synonym detected on both sides");
            }
            // Cannot have a statement after being a parent of a statement before
            if (leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM &&
                rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM) {
                if (leftQueryInput->getValue() >= rightQueryInput->getValue()) {
                    std::string errorMsg = "Statement " + leftQueryInput->getValue() + " cannot be parent of " + rightQueryInput->getValue();
                    throw std::runtime_error(errorMsg.c_str());
                }
            }
            suchThatRelationshipType = RelationshipType::PARENT_T;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            query->addRelationshipClause(RelationshipType::PARENT_T, leftQueryInput, rightQueryInput);
        }
        else {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = expect(
                stmtRef(std::set<EntityType>({ EntityType::STMT, EntityType::WHILE, EntityType::IF }), true)
                , true);
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = expect(stmtRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ }), true)
                , true);
            expect(TokenTypes::RightParen);
            // Semantic checks for Parent
            // Cannot have same synonym on both sides
            if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION
                && rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
                if (leftQueryInput->getValue() == rightQueryInput->getValue())
                    throw std::runtime_error("Same synonym detected on both sides");
            }
            // Cannot have a statement after being a parent of a statement before
            if (leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM &&
                rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM) {
                if (leftQueryInput->getValue() >= rightQueryInput->getValue()) {
                    std::string errorMsg = "Statement " + leftQueryInput->getValue() + " cannot be parent of " + rightQueryInput->getValue();
                    throw std::runtime_error(errorMsg.c_str());
                }
            }
            suchThatRelationshipType = RelationshipType::PARENT;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            query->addRelationshipClause(RelationshipType::PARENT, leftQueryInput, rightQueryInput);
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
            std::shared_ptr<QueryInput> leftQueryInput = expect(stmtRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ })
                , true)
                    , true);
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = expect(stmtRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ })
                , true)
                    , true);
            expect(TokenTypes::RightParen);
            // Semantic checks for Follows
            // Cannot have same synonym on both sides
            if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION
                && rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
                if (leftQueryInput->getValue() == rightQueryInput->getValue())
                    throw std::runtime_error("Same synonym detected on both sides");
            }
            // Cannot have a statement coming after following one that comes before
            if (leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM &&
                rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM) {
                if (leftQueryInput->getValue() >= rightQueryInput->getValue()) {
                    std::string errorMsg = "Statement " + leftQueryInput->getValue() + " cannot follow " + rightQueryInput->getValue();
                    throw std::runtime_error(errorMsg.c_str());
                }
            }
            suchThatRelationshipType = RelationshipType::FOLLOWS_T;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            query->addRelationshipClause(RelationshipType::FOLLOWS_T, leftQueryInput, rightQueryInput);
        }
        else {
            expect(TokenTypes::LeftParen);
            std::shared_ptr<QueryInput> leftQueryInput = expect(stmtRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ })
                , true)
                    , true);
            expect(TokenTypes::Comma);
            std::shared_ptr<QueryInput> rightQueryInput = expect(stmtRef(std::set<EntityType>(
                { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ })
                , true)
                    , true);
            expect(TokenTypes::RightParen);
            // Semantic checks for Follows*
            // Cannot have same synonym on both sides
            if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION
                && rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
                if (leftQueryInput->getValue() == rightQueryInput->getValue())
                    throw std::runtime_error("Same synonym detected on both sides");
            }
            // Cannot have a statement coming after following one that comes before
            if (leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM &&
                rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM) {
                if (leftQueryInput->getValue() >= rightQueryInput->getValue()) {
                    std::string errorMsg = "Statement " + leftQueryInput->getValue() + " cannot follow " + rightQueryInput->getValue();
                    throw std::runtime_error(errorMsg.c_str());
                }
            }
            suchThatRelationshipType = RelationshipType::FOLLOWS;
            suchThatLeftQueryInput = leftQueryInput;
            suchThatRightQueryInput = rightQueryInput;
            query->addRelationshipClause(RelationshipType::FOLLOWS, leftQueryInput, rightQueryInput);
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
        return std::make_shared<Expression>(token->getValue());
    }
    return std::make_shared<Expression>("_");
}

std::unique_ptr<Token> QueryParser::subExpression()
{
    if (accept(TokenTypes::DoubleQuote)) {
        std::unique_ptr<Token> fctr = std::move(factor());
        if (!fctr) throw std::runtime_error("Unexpected token found while parsing factor in subexpression of pattern");
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
RelationshipType QueryParser::getSuchThatRelationshipType()
{
    return suchThatRelationshipType;
}
std::shared_ptr<QueryInput> QueryParser::getSuchThatLeftQueryInput()
{
    return suchThatLeftQueryInput;
}
std::shared_ptr<QueryInput> QueryParser::getSuchThatRightQueryInput()
{
    return suchThatRightQueryInput;
}