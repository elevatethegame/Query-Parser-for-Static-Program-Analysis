#include "Declaration.h"

Declaration::Declaration(QueryInputType queryInputType, EntityType entityType, string value) {
	aQueryInputType = queryInputType;
	aEntityType = entityType;
	aValue = value;
}

EntityType Declaration::getEntityType() {
	return aEntityType;
}