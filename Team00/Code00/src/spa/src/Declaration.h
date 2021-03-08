#pragma once
#include "QueryInput.h"
#include "EntityType.h"

class Declaration: public QueryInput {
private:
	EntityType aEntityType;

public:
	Declaration(EntityType entityType, string value);
	EntityType getEntityType();
};