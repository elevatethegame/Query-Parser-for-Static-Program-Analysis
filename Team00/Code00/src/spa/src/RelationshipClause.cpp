#include "RelationshipClause.h"

RelationshipClause::RelationshipClause(RelationshipType relationshipType, QueryInput* leftInput, QueryInput* rightInput) {
	aRelationshipType = relationshipType;
	aLeftInput = leftInput;
	aRightInput = rightInput;
}

RelationshipType RelationshipClause::getRelationshipType() {
	return aRelationshipType;
}

QueryInput* RelationshipClause::getLeftInput() {
	return aLeftInput;
}

QueryInput* RelationshipClause::getRightInput() {
	return aRightInput;
}