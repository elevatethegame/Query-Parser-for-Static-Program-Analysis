#include "QueryParser.h"
#include "Tokenizer.h"
#include "EntitiesTable.h"
#include "QueryParserErrorUtility.h"

#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end
#include <iostream>

// =========================================================================================================================

QueryParser::QueryParser(std::shared_ptr<TokenizerInterface> tokenizer, std::shared_ptr<QueryInterface> query) : tokenizer(tokenizer), query(query)
{

}

void QueryParser::getNextToken()
{
    currToken = std::move(tokenizer->readNext());
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
    if (type == TokenTypes::Identifier && Tokenizer::canTreatAsIdent(currToken->getType(), currToken->getValue())) {
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
    // Could not match the expected token type: either no tokens left or currToken does not match
    if (currToken) {  // If there are still tokens left
        QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString());
    }
    else {  // No more tokens left
        QueryParserErrorUtility::unexpectedQueryEndSyntacticException();
    }
}

void QueryParser::selectClause()
{
    // Can have zero or more declarations
    while (declaration());

    expect(TokenTypes::Select);
    
    resultClause();

    // Can have any number of such-that and pattern clauses in any order
    while (suchThatClause() || withClause() || patternClause());
}

void QueryParser::resultClause()
{
    if (accept(TokenTypes::Boolean)) {
        return;
    }
    if (tuple()) {
        return;
    }
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), RES_CLAUSE_STR);
}

bool QueryParser::tuple()
{
    if (elem()) {
        return true;
    }
    else if (accept(TokenTypes::LeftAngleBracket)) {
        if (!elem()) return false;  // must have at least one elem in tuple
        while (accept(TokenTypes::Comma)) {
            elem();
        }
        expect(TokenTypes::RightAngleBracket);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::elem()
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::AttrRef));
    if (token) {
        std::string synonymValue = Tokenizer::getAttrRefSynonym(token->getValue());
        std::string attrName = Tokenizer::getAttrRefAttrName(token->getValue());

        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, RES_CLAUSE_STR, synonymValue);

        // TODO: add checks for semantically incorrect attribute names for certain synonyms (e.g. constant.procName is invalid)

        auto queryInput = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
        queryInput->setIsAttribute();

        query->addDeclarationToSelectClause(queryInput);

        return true;
    }
    token = std::move(accept(TokenTypes::Identifier));
    if (token) {

        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, RES_CLAUSE_STR, token->getValue());

        auto queryInput = std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());

        query->addDeclarationToSelectClause(queryInput);

        return true;
    }
    return false;
}

bool QueryParser::declaration()
{
    std::unique_ptr<Token> designEntity = std::move(accept(TokenTypes::DesignEntity));
    if (designEntity) {
        std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));

        // Throw an error if synonyms are redeclared as a different entity type
        QueryParserErrorUtility::semanticCheckRedeclaredSynonym(synonyms, synToken->getValue(), designEntity->getEntityType());

        synonyms[synToken->getValue()] = designEntity->getEntityType();
        // std::cout << synToken->getValue() << "\n";
        while (accept(TokenTypes::Comma)) {
            synToken = std::move(expect(TokenTypes::Identifier));

            // Throw an error if synonyms are redeclared as a different entity type
            QueryParserErrorUtility::semanticCheckRedeclaredSynonym(synonyms, synToken->getValue(), designEntity->getEntityType());

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
    if (modifies() || uses() || follows() || parent() || calls() || next()) {
        return;
    }
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), RELREF_STR);
}

std::shared_ptr<QueryInput> QueryParser::stmtRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore)
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, STMTREF_STR, token->getValue());

        // Check that synonym has entity that is allowed
        QueryParserErrorUtility::semanticCheckValidSynonymEntityType(synonyms, token->getValue(), allowedDesignEntities);

        return std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        QueryParserErrorUtility::semanticCheckWildcardAllowed(acceptsUnderscore, token->getValue(), STMTREF_STR);
        return std::make_shared<Any>(token->getValue());
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        return std::make_shared<StmtNum>(token->getValue());
    }
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), STMTREF_STR);
}

std::shared_ptr<QueryInput> QueryParser::entRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore)
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, STMTREF_STR, token->getValue());

        // Check that synonym has entity that is allowed
        QueryParserErrorUtility::semanticCheckValidSynonymEntityType(synonyms, token->getValue(), allowedDesignEntities);

        return std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        QueryParserErrorUtility::semanticCheckWildcardAllowed(acceptsUnderscore, token->getValue(), ENTREF_STR);
        return std::make_shared<Any>(token->getValue());
    }
    else if (accept(TokenTypes::DoubleQuote)) {
        token = std::move(expect(TokenTypes::Identifier));
        expect(TokenTypes::DoubleQuote);
        return std::make_shared<Ident>(token->getValue());
    }
    else {
        QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), ENTREF_STR);
    }
}

bool QueryParser::modifies()
{
    if (accept(TokenTypes::Modifies)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput;
        if (currToken->getType() == TokenTypes::DoubleQuote) {
            leftQueryInput = entRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::MODIFIES), false);
        }
        else {
            leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::MODIFIES), false);
        }
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::MODIFIES), true);
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
        std::shared_ptr<QueryInput> leftQueryInput;
        if (currToken->getType() == TokenTypes::DoubleQuote) {
            leftQueryInput = entRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::USES), false);
        }
        else {
            leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::USES), false);
        }
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::USES), true);
        expect(TokenTypes::RightParen);
        query->addRelationshipClause(RelationshipType::USES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::parent()
{
    RelationshipType relType;
    std::string relRef;
    if (accept(TokenTypes::Parent)) {
        relType = RelationshipType::PARENT;
        relRef = PARENT;
    }
    else if (accept(TokenTypes::ParentT)) {
        relType = RelationshipType::PARENT_T;
        relRef = PARENT_T;
    }
    else {
        return false;
    }
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::PARENT), true);
    expect(TokenTypes::Comma);
    std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::PARENT), true);
    expect(TokenTypes::RightParen);
    // Semantic checks for Parent
    // Cannot have same synonym on both sides
    QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

    // Cannot have a statement after being a parent of a statement before
    QueryParserErrorUtility::semanticCheckLeftStmtNumGtrEqualsRightStmtNum(leftQueryInput, rightQueryInput, relRef);

    query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
    return true;
}

bool QueryParser::follows()
{
    RelationshipType relType;
    std::string relRef;
    if (accept(TokenTypes::Follows)) {
        relType = RelationshipType::FOLLOWS;
        relRef = FOLLOWS;
    }
    else if (accept(TokenTypes::FollowsT)) {
        relType = RelationshipType::FOLLOWS_T;
        relRef = FOLLOWS_T;
    }
    else {
        return false;
    }

    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::FOLLOWS), true);
    expect(TokenTypes::Comma);
    std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::FOLLOWS), true);
    expect(TokenTypes::RightParen);
    // Semantic checks for Follows
    // Cannot have same synonym on both sides
    QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

    // Cannot have a statement coming after following one that comes before
    QueryParserErrorUtility::semanticCheckLeftStmtNumGtrEqualsRightStmtNum(leftQueryInput, rightQueryInput, relRef);

    query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
    return true;
}

bool QueryParser::calls()
{
    RelationshipType relType;
    std::string relRef;
    if (accept(TokenTypes::Calls)) {
        relType = RelationshipType::CALLS;
        relRef = CALLS;
    }
    else if (accept(TokenTypes::CallsT)) {
        relType = RelationshipType::CALLS_T;
        relRef = CALLS_T;
    }
    else {
        return false;
    }

    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> leftQueryInput = entRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::CALLS), true);
    expect(TokenTypes::Comma);
    std::shared_ptr<QueryInput> rightQueryInput = entRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::CALLS), true);
    expect(TokenTypes::RightParen);
    // Semantic checks for Calls
    // Cannot have same synonym on both sides
    QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

    query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
    return true;
}

bool QueryParser::next()
{
    RelationshipType relType;
    std::string relRef;
    if (accept(TokenTypes::Next)) {
        relType = RelationshipType::NEXT;
        relRef = NEXT;
    }
    else if (accept(TokenTypes::NextT)) {
        relType = RelationshipType::NEXT_T;
        relRef = NEXT_T;
    }
    else {
        return false;
    }
    
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::NEXT), true);
    expect(TokenTypes::Comma);
    std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::NEXT), true);
    expect(TokenTypes::RightParen);

    // Semantic checks for Next
    // Cannot have same synonym on both sides
    QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

    query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
    return true;
}

bool QueryParser::withClause()
{
    if (accept(TokenTypes::With)) {
        attrCompare();
        while (accept(TokenTypes::And)) {  // Can have any number of 'and' attrCompare
            attrCompare();
        }
        return true;
    }
    return false;
}

void QueryParser::attrCompare()
{
    std::shared_ptr<QueryInput> leftRef = ref();
    expect(TokenTypes::Equals);
    std::shared_ptr<QueryInput> rightRef = ref();
    query->addWithClause(leftRef, rightRef);
}

std::shared_ptr<QueryInput> QueryParser::ref()
{
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::AttrRef));
    if (token) {
        std::string synonymValue = Tokenizer::getAttrRefSynonym(token->getValue());
        std::string attrName = Tokenizer::getAttrRefAttrName(token->getValue());

        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, REF_STR, synonymValue);

        // TODO: add checks for semantically incorrect attribute names for certain synonyms (e.g. constant.procName is invalid)

        auto queryInput = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
        queryInput->setIsAttribute();

        return queryInput;
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        auto queryInput = std::make_shared<StmtNum>(token->getValue());
        return queryInput;
    }
    token = std::move(accept(TokenTypes::Identifier));
    if (token) {

        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, REF_STR, token->getValue());

        auto queryInput = std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());

        return queryInput;
    }
    if (accept(TokenTypes::DoubleQuote)) {
        token = std::move(expect(TokenTypes::Identifier));
        expect(TokenTypes::DoubleQuote);

        auto queryInput = std::make_shared<Ident>(token->getValue());

        return queryInput;
    }
    // Ref could not be parsed correctly
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), REF_STR);
}

bool QueryParser::patternClause()
{
    if (accept(TokenTypes::Pattern)) {

        bool patternFound = false;

        while (!patternFound || accept(TokenTypes::And)) {
            std::unique_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));

            // Semantic check if synonym has been declared before being used
            QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, PATTERN_CLAUSE_STR, synToken->getValue());

            // Find the Entity Type of the synonym
            auto it = synonyms.find(synToken->getValue());
            EntityType synonymType = it->second;

            // Compulsory to match at least one pattern
            if (synonymType == EntityType::ASSIGN) {
                patternAssign(synToken->getValue());
            }
            else if (synonymType == EntityType::WHILE) {
                patternWhile(synToken->getValue());
            }
            else if (synonymType == EntityType::IF) {
                patternIf(synToken->getValue());
            }
            else {
                QueryParserErrorUtility::unexpectedTokenSyntacticException(synToken->toString(), PATTERN_CLAUSE_STR);
            }

            patternFound = true;
        }
        return true;
    }
    return false;
}

void QueryParser::patternAssign(std::string synonymValue)
{
    auto synonym = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = entRef(EntitiesTable::getPatternAllowedEntities(PatternType::PATTERN_ASSIGN), true);
    expect(TokenTypes::Comma);
    std::shared_ptr<Expression> expression = expressionSpec();
    expect(TokenTypes::RightParen);
    query->addAssignPatternClause(synonym, queryInput, expression);
}

void QueryParser::patternWhile(std::string synonymValue)
{
    auto synonym = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = entRef(EntitiesTable::getPatternAllowedEntities(PatternType::PATTERN_WHILE), true);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::RightParen);
    query->addContainerPatternClause(synonym, queryInput);
}

void QueryParser::patternIf(std::string synonymValue)
{
    auto synonym = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = entRef(EntitiesTable::getPatternAllowedEntities(PatternType::PATTERN_IF), true);
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
        result = result.combineExpression(token->getValue(), subResult, result.getType());
        // Try to accept another ExprSymbol to loop again
        token = std::move(accept(TokenTypes::ExprSymbol));
    }
}

void QueryParser::term(Expression& result)
{
    factor(result);
    std::unique_ptr<Token> token = std::move(accept(TokenTypes::TermSymbol));
    while (token) {
        Expression subResult("", result.getType());
        factor(subResult);
        result = result.combineExpression(token->getValue(), subResult, result.getType());
        // Try to accept another TermSymbol to loop again
        token = std::move(accept(TokenTypes::TermSymbol));
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
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), FACTOR_STR);
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