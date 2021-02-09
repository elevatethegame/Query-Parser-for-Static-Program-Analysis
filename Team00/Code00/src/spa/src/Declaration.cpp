#include "Declaration.h"

Declaration::Declaration(EntityType entityType, string value) {
	aQueryInputType = QueryInputType::DECLARATION;
	aEntityType = entityType;
	aValue = value;
}

EntityType Declaration::getEntityType() {
	return aEntityType;
}