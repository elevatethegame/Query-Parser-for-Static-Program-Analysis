#pragma once
#ifndef ENTITIESTABLE_H
#define ENTITIESTABLE_H

#include "RelationshipType.h"
#include "EntityType.h"
#include "PatternType.h"
#include "Declaration.h"
#include <unordered_map>
#include <set>
#include <memory>

class EntitiesTable 
{
private:

    static std::unordered_map<RelationshipType, std::set<EntityType>> relRefLeftArgTable;

    static std::unordered_map<RelationshipType, std::set<EntityType>> relRefRightArgTable;

    static std::unordered_map<PatternType, std::set<EntityType>> patternArgTable;

    static std::unordered_map<EntityType, std::set<std::string>> entityTypeToValidAttrTable;

    static std::unordered_map<EntityType, std::string> entityTypeSecondaryAttrTable;

    static std::unordered_map<EntityType, QueryInputType> primaryAttributeToQueryInputTypeTable;

    static std::unordered_map<EntityType, QueryInputType> secondaryAttributeToQueryInputTypeTable;

    static std::unordered_map<EntityType, std::set<EntityType>> entityTypeToValidAttrCompareEntities;

public:
    static std::set<EntityType> getRelAllowedLeftEntities(RelationshipType);
    static std::set<EntityType> getRelAllowedRightEntities(RelationshipType);
    static std::set<EntityType> getPatternAllowedEntities(PatternType);
    static std::set<std::string> getValidSynonymAttrs(EntityType);
    static bool isSecondaryAttr(EntityType entityType, std::string attrName);
    static QueryInputType getAttrRefType(std::shared_ptr<Declaration> declaration);
    static std::set<EntityType> getEntityTypeValidAttrCompareEntities(EntityType);
};

#endif