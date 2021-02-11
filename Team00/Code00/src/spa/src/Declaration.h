#pragma once
#ifndef DECLARATION_H
#define DECLARATION_H

#include "QueryInput.h"
#include "EntityType.h"
#include <string>

class Declaration : public QueryInput {
private:
	EntityType m_entityType;

public:
	Declaration(QueryInputType queryInputType, std::string value, EntityType entityType);
	EntityType getEntityType();
};

#endif
