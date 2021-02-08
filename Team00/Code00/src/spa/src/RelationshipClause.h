#pragma once

#include "RelationshipType.h"
#include "QueryInput.h"

class RelationshipClause {
private:
	RelationshipType aRelationshipType;
	QueryInput* aLeftInput;
	QueryInput* aRightInput;

public:
	RelationshipClause(RelationshipType relationshipType, QueryInput* leftInput, QueryInput* rightInput);
	RelationshipType getRelationshipType();
	QueryInput* getLeftInput();
	QueryInput* getRightInput();
};