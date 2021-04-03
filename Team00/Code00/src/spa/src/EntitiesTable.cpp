#include "EntitiesTable.h"

std::unordered_map<RelationshipType, std::set<EntityType>> EntitiesTable::relRefLeftArgTable = 
{
    {RelationshipType::MODIFIES, {EntityType::ASSIGN, EntityType::STMT, EntityType::READ, EntityType::PROC, EntityType::IF, EntityType::WHILE }},
    {RelationshipType::USES, { EntityType::ASSIGN, EntityType::STMT, EntityType::PRINT, EntityType::PROC, EntityType::IF, EntityType::WHILE }},
    {RelationshipType::PARENT, { EntityType::STMT, EntityType::WHILE, EntityType::IF }},
    {RelationshipType::FOLLOWS, { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ }},
    {RelationshipType::CALLS, { EntityType::PROC }},
    {RelationshipType::NEXT, { EntityType::PROGLINE, EntityType::ASSIGN, EntityType::STMT, EntityType::CALL, EntityType::PRINT, EntityType::READ,
            EntityType::IF, EntityType::WHILE }}, 
    {RelationshipType::AFFECTS, { EntityType::STMT, EntityType::ASSIGN, EntityType::PROGLINE }},
};

std::unordered_map<RelationshipType, std::set<EntityType>> EntitiesTable::relRefRightArgTable =
{
    {RelationshipType::MODIFIES, { EntityType::VAR } },
    { RelationshipType::USES, { EntityType::VAR } },
    { RelationshipType::PARENT, { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ } },
    { RelationshipType::FOLLOWS, { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ } },
    { RelationshipType::CALLS, { EntityType::PROC } },
    { RelationshipType::NEXT, { EntityType::PROGLINE, EntityType::ASSIGN, EntityType::STMT, EntityType::CALL, EntityType::PRINT, EntityType::READ,
                EntityType::IF, EntityType::WHILE } },
    {RelationshipType::AFFECTS, { EntityType::STMT, EntityType::ASSIGN, EntityType::PROGLINE }},
};

std::unordered_map<PatternType, std::set<EntityType>> EntitiesTable::patternArgTable =
{
    {PatternType::PATTERN_ASSIGN, { EntityType::VAR }},
    {PatternType::PATTERN_IF, { EntityType::VAR }},
    {PatternType::PATTERN_WHILE, { EntityType::VAR }},
};

const std::string STMTNUM_ATTR = "stmt#";
const std::string VALUE_ATTR = "value";
const std::string PROCNAME_ATTR = "procName";
const std::string VARNAME_ATTR = "varName";

std::unordered_map<EntityType, std::set<std::string>> EntitiesTable::entityTypeToValidAttrTable =
{
    {EntityType::ASSIGN, {STMTNUM_ATTR}},
    {EntityType::STMT, {STMTNUM_ATTR}},
    {EntityType::IF, {STMTNUM_ATTR}},
    {EntityType::WHILE, {STMTNUM_ATTR}},
    {EntityType::PROGLINE, {STMTNUM_ATTR}},
    {EntityType::CONST, {VALUE_ATTR}},
    {EntityType::PROC, {PROCNAME_ATTR}},
    {EntityType::VAR, {VARNAME_ATTR}},
    {EntityType::PRINT, {STMTNUM_ATTR, VARNAME_ATTR}},
    {EntityType::READ, {STMTNUM_ATTR, VARNAME_ATTR}},
    {EntityType::CALL, {STMTNUM_ATTR, PROCNAME_ATTR}},
};

std::unordered_map<EntityType, bool> EntitiesTable::entityTypeHasMultipleAttrTable =
{
    {EntityType::ASSIGN, false},
    {EntityType::STMT, false},
    {EntityType::IF, false},
    {EntityType::WHILE, false},
    {EntityType::PROGLINE, false},
    {EntityType::CONST, false},
    {EntityType::PROC, false},
    {EntityType::VAR, false},
    {EntityType::PRINT, true},
    {EntityType::READ, true},
    {EntityType::CALL, true},
};

std::unordered_map<EntityType, std::string> EntitiesTable::entityTypeSecondaryAttrTable =
{
    {EntityType::PRINT, VARNAME_ATTR},
    {EntityType::READ, VARNAME_ATTR},
    {EntityType::CALL, PROCNAME_ATTR},
};


std::set<EntityType> EntitiesTable::getRelAllowedLeftEntities(RelationshipType relationshipType)
{
    return relRefLeftArgTable.at(relationshipType);
}

std::set<EntityType> EntitiesTable::getRelAllowedRightEntities(RelationshipType relationshipType)
{
    return relRefRightArgTable.at(relationshipType);
}

std::set<EntityType> EntitiesTable::getPatternAllowedEntities(PatternType patternType)
{
    return patternArgTable.at(patternType);
}

std::set<std::string> EntitiesTable::getValidSynonymAttrs(EntityType entityType)
{
    return entityTypeToValidAttrTable.at(entityType);
}

bool EntitiesTable::isSecondaryAttr(EntityType entityType, std::string attrName)
{
    return entityTypeHasMultipleAttrTable.at(entityType) && attrName == entityTypeSecondaryAttrTable.at(entityType);
}