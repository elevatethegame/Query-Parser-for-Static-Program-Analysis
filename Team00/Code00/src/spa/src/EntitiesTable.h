#pragma once
#ifndef ENTITIESTABLE_H
#define ENTITIESTABLE_H

#include "RelationshipType.h"
#include "EntityType.h"
#include "PatternType.h"
#include <unordered_map>
#include <set>

class EntitiesTable 
{
private:

    static std::unordered_map<RelationshipType, std::set<EntityType>> relRefLeftArgTable;

    static std::unordered_map<RelationshipType, std::set<EntityType>> relRefRightArgTable;

    static std::unordered_map<PatternType, std::set<EntityType>> patternArgTable;

    static std::unordered_map<EntityType, std::set<std::string>> entityTypeToValidAttrTable;

    static std::unordered_map<EntityType, bool> entityTypeHasMultipleAttrTable;

    static std::unordered_map<EntityType, std::string> entityTypeSecondaryAttrTable;

public:
    static std::set<EntityType> getRelAllowedLeftEntities(RelationshipType);
    static std::set<EntityType> getRelAllowedRightEntities(RelationshipType);
    static std::set<EntityType> getPatternAllowedEntities(PatternType);
    static std::set<std::string> getValidSynonymAttrs(EntityType);
    static bool isSecondaryAttr(EntityType entityType, std::string attrName);
};

#endif