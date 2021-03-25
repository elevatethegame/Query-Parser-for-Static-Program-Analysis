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
};

std::unordered_map<PatternType, std::set<EntityType>> EntitiesTable::patternArgTable =
{
    {PatternType::PATTERN_ASSIGN, { EntityType::VAR }},
    {PatternType::PATTERN_IF, { EntityType::VAR }},
    {PatternType::PATTERN_WHILE, { EntityType::VAR }},
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