#include "Query.h"

Query::Query() {
	aRelationshipClauses = new vector<RelationshipClause*>;
	aPatternClauses = new vector<PatternClause*>;
}

// API to be used by query parser 
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

Query::~Query() {
	delete aSelectClause;
	for (vector<RelationshipClause*>::iterator it = aRelationshipClauses->begin(); it != aRelationshipClauses->end(); it++) {
		delete *it;
	}

	for (vector<PatternClause*>::iterator it = aPatternClauses->begin(); it != aPatternClauses->end(); it++) {
		delete *it;
	}

	delete aRelationshipClauses;
	delete aPatternClauses;
}