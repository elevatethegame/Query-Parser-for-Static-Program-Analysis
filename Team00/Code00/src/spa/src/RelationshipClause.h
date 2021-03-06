#pragma once

#include <memory>
#include "OptionalClause.h"
#include "RelationshipType.h"
#include "QueryInput.h"

class RelationshipClause : public OptionalClause {
private:
	RelationshipType aRelationshipType;
	shared_ptr<QueryInput> aLeftInput;
	shared_ptr<QueryInput> aRightInput;

public:
	RelationshipClause(RelationshipType relationshipType, shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput);
	RelationshipType getRelationshipType();
	shared_ptr<QueryInput> getLeftInput();
	shared_ptr<QueryInput> getRightInput();
};