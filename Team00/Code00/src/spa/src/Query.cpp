#include "Query.h"

Query::Query() {
	aRelationshipClauses = new vector<RelationshipClause*>;
	aPatternClauses = new vector<PatternClause*>;
}

void Query::setSelectClause(SelectClause* selectClause) {
	aSelectClause = selectClause;
}

void Query::addRelationshipClause(RelationshipClause* relationshipClause) {
	aRelationshipClauses->push_back(relationshipClause);
}

void Query::addPatternClause(PatternClause* patternClause) {
	aPatternClauses->push_back(patternClause);
}

SelectClause* Query::getSelectClause() {
	return aSelectClause;
}

vector<RelationshipClause*>* Query::getRelationshipClauses() {
	return aRelationshipClauses;
}

vector<PatternClause*>* Query::getPatternClauses() {
	return aPatternClauses;
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