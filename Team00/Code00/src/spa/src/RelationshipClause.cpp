#include "RelationshipClause.h"

RelationshipClause::RelationshipClause(RelationshipType relationshipType, shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput) {
	aRelationshipType = relationshipType;
	aLeftInput = leftInput;
	aRightInput = rightInput;
}

RelationshipType RelationshipClause::getRelationshipType() {
	return aRelationshipType;
}

shared_ptr<QueryInput> RelationshipClause::getLeftInput() {
	return aLeftInput;
}

shared_ptr<QueryInput> RelationshipClause::getRightInput() {
	return aRightInput;
}