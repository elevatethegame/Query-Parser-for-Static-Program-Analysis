#pragma once
#include <memory>
#include "OptionalClause.h"
#include "QueryInput.h"

class WithClause : public OptionalClause {
private:
	shared_ptr<QueryInput> aLeftInput;
	shared_ptr<QueryInput> aRightInput;

public:
	WithClause(shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput);
	shared_ptr<QueryInput> getLeftInput();
	shared_ptr<QueryInput> getRightInput();
};