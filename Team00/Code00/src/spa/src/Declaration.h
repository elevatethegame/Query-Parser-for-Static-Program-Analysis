#pragma once

#include "QueryInput.h"
#include "EntityType.h"

class Declaration : public QueryInput {
private:
	EntityType aEntityType;

public:
	Declaration(QueryInputType queryInputType, EntityType entityType, string value);
	EntityType getEntityType();
};