#include "PatternClause.h"

PatternClause::PatternClause(shared_ptr<QueryInput> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression) {
	this->aSynonym = synonym;
	this->aQueryInput = queryInput;
	this->aExpression = expression;
}

shared_ptr<QueryInput> PatternClause::getSynonym() {
	return this->aSynonym;
}

shared_ptr<QueryInput> PatternClause::getQueryInput() {
	return this->aQueryInput;
}

shared_ptr<Expression> PatternClause::getExpression() {
	return this->aExpression;
}