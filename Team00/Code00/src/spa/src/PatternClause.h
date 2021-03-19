#pragma once

#include <memory>
#include "Declaration.h"
#include "QueryInput.h"
#include "Expression.h"

class PatternClause {
private:
	shared_ptr<Declaration> aSynonym;
	shared_ptr<QueryInput> aQueryInput;
	shared_ptr<Expression> aExpression;

public:
	PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression);
	PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput);
	shared_ptr<Declaration> getSynonym();
	shared_ptr<QueryInput> getQueryInput();
	shared_ptr<Expression> getExpression();
};