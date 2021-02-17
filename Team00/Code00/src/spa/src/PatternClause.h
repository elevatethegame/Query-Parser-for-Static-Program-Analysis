#pragma once

#include <memory>
#include "QueryInput.h"
#include "Expression.h"

class PatternClause {
private:
	shared_ptr<QueryInput> aSynonym;
	shared_ptr<QueryInput> aQueryInput;
	shared_ptr<Expression> aExpression;

public:
	PatternClause(shared_ptr<QueryInput> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression);
	shared_ptr<QueryInput> getSynonym();
	shared_ptr<QueryInput> getQueryInput();
	shared_ptr<Expression> getExpression();
};