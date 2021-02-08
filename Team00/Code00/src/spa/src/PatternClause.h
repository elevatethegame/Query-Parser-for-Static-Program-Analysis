#pragma once
#include "QueryInput.h"
#include "Expression.h"

class PatternClause {
private:
	QueryInput* aSynonym;
	QueryInput* aQueryInput;
	Expression* aExpression;

public:
	PatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression);
	QueryInput* getSynonym();
	QueryInput* getQueryInput();
	Expression* getExpression();
};