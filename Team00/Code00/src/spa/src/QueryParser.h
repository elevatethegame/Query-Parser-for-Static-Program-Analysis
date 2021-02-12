#pragma once
#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "Tokenizer.h"
#include <vector>
#include <unordered_map>
#include <set>
#include "Declaration.h"
#include "Any.h"
#include "StmtNum.h"
#include "Ident.h"
#include "Expression.h"
#include "RelationshipType.h"

class QueryParser
{
private:

    std::unique_ptr<Token> currToken;
    Tokenizer tokenizer;
    // To check whether all synonyms in select, such that and pattern clauses have been declared
    std::unordered_map<std::string, EntityType> synonyms;
    // Store for testing
    std::shared_ptr<Declaration> selectClauseDeclaration;
    std::shared_ptr<Declaration> patternDeclaration;
    std::shared_ptr<QueryInput> patternQueryInput;
    std::shared_ptr<Expression> patternExpression;
    RelationshipType suchThatRelType = RelationshipType::Null;
    std::shared_ptr<QueryInput> suchThatLeftQueryInput;
    std::shared_ptr<QueryInput> suchThatRightQueryInput;


    void getNextToken();
    bool canTreatAsIdent(TokenTypes type);
    std::unique_ptr<Token> accept(TokenTypes type);
    std::unique_ptr<Token> expect(TokenTypes type);
    void selectClause();
    bool declaration();
    bool suchThatClause();
    bool patternClause();
    void relRef();
    std::shared_ptr<QueryInput> stmtRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore);
    std::shared_ptr<QueryInput> entRef(std::set<EntityType> allowedDesignEntities);
    bool Modifies();
    bool Uses();
    bool Parent();
    bool Follows();
    std::shared_ptr<Expression> expressionSpec();
    std::unique_ptr<Token> subExpression();
    std::unique_ptr<Token> factor();

public:

    QueryParser(const std::string givenInput);

    void parse();

    // Getters for testing
    std::unordered_map<std::string, EntityType> getSynonyms();
    std::shared_ptr<Declaration> getSelectClauseDeclaration();
    std::shared_ptr<Declaration> getPatternDeclaration();
    std::shared_ptr<QueryInput> getPatternQueryInput();
    std::shared_ptr<Expression> getPatternExpression();
    RelationshipType getSuchThatRelType();
    std::shared_ptr<QueryInput> getSuchThatLeftQueryInput();
    std::shared_ptr<QueryInput> getSuchThatRightQueryInput();

};

#endif