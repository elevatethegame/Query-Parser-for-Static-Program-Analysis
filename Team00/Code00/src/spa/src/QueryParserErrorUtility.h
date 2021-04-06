#pragma once

#include "SyntacticException.h"
#include "SemanticException.h"
#include "QueryParserErrorMessages.h"
#include "Token.h"
#include "QueryInput.h"
#include <unordered_map>
#include <set>
#include <memory>

class QueryParserErrorUtility {
public:
    static void unexpectedQueryEndSyntacticException();
    static void unexpectedTokenSyntacticException(std::string tokenValue);
    static void unexpectedTokenSyntacticException(std::string tokenValue, std::string clause);

    static void semanticCheckUndeclaredSynonym(std::unordered_map<std::string, EntityType> synonyms, std::string clause,
            std::string synonymValue);

    static void semanticCheckRedeclaredSynonym(std::unordered_map<std::string, EntityType> synonyms,
            std::string synonymValue, EntityType entityType);

    static void semanticCheckValidSynonymEntityType(std::unordered_map<std::string, EntityType> synonyms,
            std::string tokenValue, std::set<EntityType> allowedDesignEntities);

    static void semanticCheckWildcardAllowed(bool acceptsUnderscore, std::string tokenValue, std::string clause);

    static void semanticCheckSameSynonymBothSides(std::shared_ptr<QueryInput> leftQueryInput,
            std::shared_ptr<QueryInput> rightQueryInput, std::string relRef);

    static void semanticCheckLeftStmtNumGtrEqualsRightStmtNum(std::shared_ptr<QueryInput> leftQueryInput,
            std::shared_ptr<QueryInput> rightQueryInput, std::string relRef);

    static void semanticCheckInvalidAttrForSynonym(std::unordered_map<std::string, EntityType> synonyms, std::string attrName,
            std::string synonym);

    static void semanticCheckWithClauseSynonym(EntityType entityType, std::string synonym);

    static void semanticCheckValidAttrCompare(std::shared_ptr<QueryInput> leftRef, std::shared_ptr<QueryInput> rightRef);

    static bool isMeaninglessAttrCompare(std::shared_ptr<QueryInput> leftRef, std::shared_ptr<QueryInput> rightRef);
};