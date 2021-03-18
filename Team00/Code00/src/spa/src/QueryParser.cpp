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

void QueryParser::relRef()
{
    if (modifies()) {

    }
    else if (uses()) {

    }
    else if (follows()) {

    }
    else if (parent()) {

    }
    else if (calls()) {

    }
    else if (next()) {

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

bool QueryParser::modifies()
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
        query->addRelationshipClause(RelationshipType::MODIFIES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::uses()
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
        query->addRelationshipClause(RelationshipType::USES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::parent()
{
    if (accept(TokenTypes::Parent)) {
        RelationshipType relType;

        if (accept(TokenTypes::Asterisk))
            relType = RelationshipType::PARENT_T;
        else
            relType = RelationshipType::PARENT;

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
        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::follows()
{
    if (accept(TokenTypes::Follows)) {
        RelationshipType relType;

        if (accept(TokenTypes::Asterisk))
            relType = RelationshipType::FOLLOWS_T;
        else
            relType = RelationshipType::FOLLOWS;

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
        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::calls()
{
    if (accept(TokenTypes::Calls)) {
        RelationshipType relType;

        if (accept(TokenTypes::Asterisk))
            relType = RelationshipType::CALLS_T;
        else
            relType = RelationshipType::CALLS;

        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = expect(entRef(std::set<EntityType>(
            { EntityType::PROC })
            , true)
            , false);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = expect(entRef(std::set<EntityType>(
            { EntityType::PROC })
            , true)
            , false);
        expect(TokenTypes::RightParen);
        // Semantic checks for Calls
        // Cannot have same synonym on both sides
        if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION
            && rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
            if (leftQueryInput->getValue() == rightQueryInput->getValue())
                throw std::runtime_error("Same synonym detected on both sides");
        }
        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::next()
{
    if (accept(TokenTypes::Next)) {
        RelationshipType relType;

        if (accept(TokenTypes::Asterisk))
            relType = RelationshipType::NEXT_T;
        else
            relType = RelationshipType::NEXT;

        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = expect(stmtRef(std::set<EntityType>(
            { EntityType::PROGLINE })
            , true)
            , true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = expect(stmtRef(std::set<EntityType>(
            { EntityType::PROGLINE })
            , true)
            , true);
        expect(TokenTypes::RightParen);

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::patternClause()
{
    if (accept(TokenTypes::Pattern)) {
        if (currToken->getType() != TokenTypes::Identifier) {
            std::string errorMsg = "Expected identifier in Pattern Clause, instead found: " + currToken->getValue();
            throw std::runtime_error(errorMsg.c_str());
        }

        // Semantic check if synonym has been declared before being used
        auto it = synonyms.find(currToken->getValue());
        if (it == synonyms.end()) {
            std::string errorMsg = "Undeclared synonym encountered in Pattern clause: " + currToken->getValue();
            throw std::runtime_error(errorMsg.c_str());
        }

        // Find the Entity Type of the synonym
        EntityType synonymType = it->second;

        // Compulsory to match at least one pattern
        if (synonymType == EntityType::ASSIGN) {
            patternAssign();
        }
        else if (synonymType == EntityType::WHILE) {
            patternWhile();
        }
        else if (synonymType == EntityType::IF) {
            patternIf();
        }
        else {
            std::string errorMsg = "None of pattern type synAssign, synWhile or synIf could be matched. Unexpected token: " + currToken->getValue();
            throw std::runtime_error(errorMsg.c_str());
        }

        // Can be followed by zero or more ('and' pattern)
        while (accept(TokenTypes::And)) {
            if (currToken->getType() != TokenTypes::Identifier) {
                std::string errorMsg = "Expected identifier in Pattern Clause, instead found: " + currToken->getValue();
                throw std::runtime_error(errorMsg.c_str());
            }

            // Semantic check if synonym has been declared before being used
            auto it = synonyms.find(currToken->getValue());
            if (it == synonyms.end()) {
                std::string errorMsg = "Undeclared synonym encountered in Pattern clause: " + currToken->getValue();
                throw std::runtime_error(errorMsg.c_str());
            }

            // Find the Entity Type of the synonym
            EntityType synonymType = it->second;

            // Compulsory to match at least one pattern
            if (synonymType == EntityType::ASSIGN) {
                patternAssign();
            }
            else if (synonymType == EntityType::WHILE) {
                patternWhile();
            }
            else if (synonymType == EntityType::IF) {
                patternIf();
            }
            else {
                std::string errorMsg = "None of pattern type synAssign, synWhile or synIf could be matched. Unexpected token: " + currToken->getValue();
                throw std::runtime_error(errorMsg.c_str());
            }
        }
        return true;


    }
    return false;
}

void QueryParser::patternAssign()
{
    std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));
    auto synonym = std::make_shared<Declaration>(synonyms[synToken->getValue()], synToken->getValue());
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = expect(entRef(std::set<EntityType>({ EntityType::VAR }), true), false);
    expect(TokenTypes::Comma);
    std::shared_ptr<Expression> expression = expressionSpec();
    expect(TokenTypes::RightParen);
    query->addAssignPatternClause(synonym, queryInput, expression);
}

void QueryParser::patternWhile()
{
    std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));
    auto synonym = std::make_shared<Declaration>(synonyms[synToken->getValue()], synToken->getValue());
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = expect(entRef(std::set<EntityType>({ EntityType::VAR }), true), false);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::RightParen);
    query->addContainerPatternClause(synonym, queryInput);
}

void QueryParser::patternIf()
{
    std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));
    auto synonym = std::make_shared<Declaration>(synonyms[synToken->getValue()], synToken->getValue());
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = expect(entRef(std::set<EntityType>({ EntityType::VAR }), true), false);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::RightParen);
    query->addContainerPatternClause(synonym, queryInput);
}

std::shared_ptr<Expression> QueryParser::expressionSpec()
{
    if (accept(TokenTypes::Underscore)) {
        // Parse _"exp"_ expressionSpec
        if (accept(TokenTypes::DoubleQuote)) {
            Expression result("", ExpressionType::PARTIAL);
            expression(result);
            expect(TokenTypes::DoubleQuote);
            expect(TokenTypes::Underscore);
            return std::make_shared<Expression>(result);
        }
        // Parse _ expressionSpec
        return std::make_shared<Expression>("_", ExpressionType::EMPTY);
    }
    // Parse "exp" expressionSpec
    expect(TokenTypes::DoubleQuote);
    Expression result("", ExpressionType::EXACT);
    expression(result);
    expect(TokenTypes::DoubleQuote);
    return std::make_shared<Expression>(result);
}

void QueryParser::expression(Expression& result)
{
    term(result);
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::ExprSymbol));
    while (token) {
        Expression subResult("", result.getType());
        term(subResult);
        result.combineExpression(token->getValue(), subResult, result.getType());
        // Try to accept another ExprSymbol to loop again
        token = std::move(accept(TokenTypes::ExprSymbol));
    }
}

std::unique_ptr<Token> QueryParser::acceptTermSymbol()
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::TermSymbol));
    if (!token)
        token = std::move(accept(TokenTypes::Asterisk));  // Asterisk Token Type can also be Multiplication operator
    return token;
}

void QueryParser::term(Expression& result)
{
    factor(result);
    std::unique_ptr<Token> token = std::move(acceptTermSymbol());
    while (token) {
        Expression subResult("", result.getType());
        factor(subResult);
        result.combineExpression(token->getValue(), subResult, result.getType());
        // Try to accept another TermSymbol to loop again
        token = std::move(acceptTermSymbol());
    }
}

void QueryParser::factor(Expression& result)
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        result = Expression(token->getValue(), result.getType());
        return;
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        result = Expression(token->getValue(), result.getType());
        return;
    }
    if (accept(TokenTypes::LeftParen)) {
        expression(result);
        expect(TokenTypes::RightParen);
        return;
    }
    // Factor could not be parsed correctly
    std::string errorMsg = "Factor could not be parsed correctly. Unexpected token: " + currToken->getValue();
    throw std::runtime_error(errorMsg.c_str());
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

std::shared_ptr<QueryInterface> QueryParser::getQuery()
{
    return query;
}