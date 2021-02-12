#include "Query.h"

Query::Query() {
	this->aRelationshipClauses = new vector<RelationshipClause*>;
	this->aPatternClauses = new vector<PatternClause*>;
}

void Query::setSelectClause(Declaration* declaration) {
	this->aSelectClause = new SelectClause(declaration);
}

void Query::addRelationshipClause(RelationshipType relationshipType, QueryInput* leftQueryInput, QueryInput* rightQueryInput) {
	RelationshipClause* relationshipClause = new RelationshipClause(relationshipType, leftQueryInput, rightQueryInput);
	this->aRelationshipClauses->push_back(relationshipClause);
}

void Query::addPatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression) {
	PatternClause* patternClause = new PatternClause(synonym, queryInput, expression);
	this->aPatternClauses->push_back(patternClause);
}

SelectClause* Query::getSelectClause() {
	return this->aSelectClause;
}

vector<RelationshipClause*>* Query::getRelationshipClauses() {
	return this->aRelationshipClauses;
}

vector<PatternClause*>* Query::getPatternClauses() {
	return this->aPatternClauses;
}