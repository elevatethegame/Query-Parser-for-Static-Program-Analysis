#include "Declaration.h"

using namespace std;

Declaration::Declaration(EntityType entityType, string value) {
	this->aEntityType = entityType;
	this->aValue = value;
	this->aQueryInputType = QueryInputType::DECLARATION;
}

EntityType Declaration::getEntityType() {
	return this->aEntityType;
}