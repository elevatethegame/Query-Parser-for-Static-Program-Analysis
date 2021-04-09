#include "QueryParserErrorUtility.h"

void QueryParserErrorUtility::unexpectedQueryEndSyntacticException()
{
    throw SyntacticException(UNEXPECTED_END_OF_QUERY);
}

void QueryParserErrorUtility::unexpectedTokenSyntacticException(std::string tokenValue)
{
    throw SyntacticException(UNEXPECTED_TOKEN + tokenValue);
}

void QueryParserErrorUtility::unexpectedTokenSyntacticException(std::string tokenValue, std::string clause)
{
    throw SyntacticException("Unexpected token encountered when parsing " + clause + ": " + tokenValue);
}

void QueryParserErrorUtility::semanticCheckUndeclaredSynonym(std::unordered_map<std::string, EntityType> synonyms, 
        std::string clause, std::string synonymValue)
{
    // Semantic check: Undeclared Synonym used in Clause
    if (synonyms.find(synonymValue) == synonyms.end()) {
        throw SemanticException("Undeclared synonym encountered in " + clause + ": " + synonymValue);
    }
    
}

void QueryParserErrorUtility::semanticCheckRedeclaredSynonym(std::unordered_map<std::string, EntityType> synonyms,
        std::string synonymValue, EntityType entityType)
{
    auto it = synonyms.find(synonymValue);
    // Throw an error if synonyms are redeclared as a different entity type
    if (it != synonyms.end() && it->second != entityType) {
        std::string errorMsg = "Synonym " + synonymValue + " with " + Token::EntityTypeToString(it->second)
            + " being redeclared as " + Token::EntityTypeToString(entityType);
        throw SemanticException(errorMsg);
    }
}

void QueryParserErrorUtility::semanticCheckValidSynonymEntityType(std::unordered_map<std::string, EntityType> synonyms,
        std::string tokenValue, std::set<EntityType> allowedDesignEntities)
{
    auto it = synonyms.find(tokenValue);
    EntityType synonymType = it->second;
    if (allowedDesignEntities.find(synonymType) == allowedDesignEntities.end()) {
        std::string errorMsg = "Synonym " + tokenValue + " not allowed, has " + Token::EntityTypeToString(synonymType);
        throw SemanticException(errorMsg);
    }
}

void QueryParserErrorUtility::semanticCheckWildcardAllowed(bool acceptsUnderscore, std::string tokenValue, std::string clause)
{
    if (!acceptsUnderscore)
        throw SemanticException(tokenValue + " is not allowed as first argument in " + clause);

}

void QueryParserErrorUtility::semanticCheckSameSynonymBothSides(std::shared_ptr<QueryInput> leftQueryInput, 
        std::shared_ptr<QueryInput> rightQueryInput, std::string relRef)
{
    if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION
        && rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
        if (leftQueryInput->getValue() == rightQueryInput->getValue())
            throw SemanticException(SAME_SYNONYM_BOTH_SIDES + " in " + relRef);
    }
}

void QueryParserErrorUtility::semanticCheckLeftStmtNumGtrEqualsRightStmtNum(std::shared_ptr<QueryInput> leftQueryInput,
        std::shared_ptr<QueryInput> rightQueryInput, std::string relRef)
{
    if (leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM &&
        rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM) {
        if (leftQueryInput->getValue() >= rightQueryInput->getValue()) {
            std::string errorMsg = "Left Stmt# " + leftQueryInput->getValue() + " cannot be greater or equals to right stmt# " + rightQueryInput->getValue()
                    + " in " + relRef;
            throw SemanticException(errorMsg);
        }
    }
}

void QueryParserErrorUtility::semanticCheckInvalidAttrForSynonym(std::set<std::string> validSynonymAttrs, std::string attrName, 
        std::string synonym, EntityType entityType)
{
    if (validSynonymAttrs.find(attrName) == validSynonymAttrs.end()) {
        throw SemanticException("Invalid attrName " + attrName + " for synonym:  " + synonym + " with " + Token::EntityTypeToString(entityType));
    }
}

void QueryParserErrorUtility::semanticCheckWithClauseSynonym(EntityType entityType, std::string synonym)
{
    if (entityType != EntityType::PROGLINE) {
        throw SemanticException("Synonym in With Clause: " + synonym + " not of prog_line entity type");
    }
}