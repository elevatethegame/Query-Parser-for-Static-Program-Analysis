#include "PatternClause.h"

PatternClause::PatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression) {
	aSynonym = synonym;
	aQueryInput = queryInput;
	aExpression = expression;
}

QueryInput* PatternClause::getSynonym() {
	return aSynonym;
}

QueryInput* PatternClause::getQueryInput() {
	return aQueryInput;
}

Expression* PatternClause::getExpression() {
	return aExpression;
}