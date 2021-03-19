#include "PatternClause.h"

PatternClause::PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression) {
	this->aSynonym = synonym;
	this->aQueryInput = queryInput;
	this->aExpression = expression;
}

PatternClause::PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput) {
	this->aSynonym = synonym;
	this->aQueryInput = queryInput;
}

shared_ptr<Declaration> PatternClause::getSynonym() {
	return this->aSynonym;
}

shared_ptr<QueryInput> PatternClause::getQueryInput() {
	return this->aQueryInput;
}

shared_ptr<Expression> PatternClause::getExpression() {
	return this->aExpression;
}