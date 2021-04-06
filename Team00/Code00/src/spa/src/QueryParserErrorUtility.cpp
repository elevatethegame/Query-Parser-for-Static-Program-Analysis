#include "QueryParserErrorUtility.h"
#include "EntitiesTable.h";

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

void QueryParserErrorUtility::semanticCheckInvalidAttrForSynonym(std::unordered_map<std::string, EntityType> synonyms, std::string attrName,
        std::string synonym)
{
    std::set<std::string> validSynonymAttrs = EntitiesTable::getValidSynonymAttrs(synonyms[synonym]);
    EntityType entityType = synonyms[synonym];
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

void QueryParserErrorUtility::semanticCheckValidAttrCompare(std::shared_ptr<QueryInput> leftRef, std::shared_ptr<QueryInput> rightRef)
{
    QueryInputType leftRefType;
    if (leftRef->getQueryInputType() == QueryInputType::DECLARATION) {
        leftRefType = EntitiesTable::getAttrRefType(std::dynamic_pointer_cast<Declaration>(leftRef));
    }
    else {
        leftRefType = leftRef->getQueryInputType();
    }

    QueryInputType rightRefType;
    if (rightRef->getQueryInputType() == QueryInputType::DECLARATION) {
        rightRefType = EntitiesTable::getAttrRefType(std::dynamic_pointer_cast<Declaration>(rightRef));
    }
    else {
        rightRefType = rightRef->getQueryInputType();
    }

    if (leftRefType != rightRefType) {
        throw SemanticException("Invalid Attribute comparison in with clause (leftRef has different type from rightRef): " + leftRef->getValue() + " compared to " + rightRef->getValue());
    }

    // Also throw semantic exception for any queries that are guaranteed to have 0 results
    // such as print.stmt# = read.stmt#
    if (leftRef->getQueryInputType() == QueryInputType::DECLARATION && rightRef->getQueryInputType() == QueryInputType::DECLARATION
        && leftRefType == QueryInputType::STMT_NUM) {
        std::shared_ptr<Declaration> leftDeclaration = std::dynamic_pointer_cast<Declaration>(leftRef);
        std::shared_ptr<Declaration> rightDeclaration = std::dynamic_pointer_cast<Declaration>(rightRef);
        std::set<EntityType> validAttrCompareEntities = EntitiesTable::getEntityTypeValidAttrCompareEntities(leftDeclaration->getEntityType());
        if (validAttrCompareEntities.find(rightDeclaration->getEntityType()) == validAttrCompareEntities.end())
            throw SemanticException("Attribute comparison of 2 synonym attributes guaranteed to be false/empty in with clause: " 
                    + leftRef->getValue() + " compared to " + rightRef->getValue());
    }

    // Also throw semantic exception for any queries that are guaranteed to have 0 results
    // such as 10 = 12
    if (leftRef->getQueryInputType() == QueryInputType::STMT_NUM && rightRef->getQueryInputType() == QueryInputType::STMT_NUM
        && leftRef->getValue() != rightRef->getValue() ) {
        throw SemanticException("Attribute comparison of 2 integers guaranteed to be false/empty in with clause: " 
                + leftRef->getValue() + " compared to " + rightRef->getValue());
    }
}

bool QueryParserErrorUtility::isMeaninglessAttrCompare(std::shared_ptr<QueryInput> leftRef, std::shared_ptr<QueryInput> rightRef)
{
    return leftRef->getQueryInputType() == QueryInputType::STMT_NUM && rightRef->getQueryInputType() == QueryInputType::STMT_NUM
        && leftRef->getValue() == rightRef->getValue();
}