#pragma once
#include "ClauseType.h"

class OptionalClause {
protected:
	ClauseType clauseType;

public:
	ClauseType getClauseType();
	virtual ~OptionalClause();
};