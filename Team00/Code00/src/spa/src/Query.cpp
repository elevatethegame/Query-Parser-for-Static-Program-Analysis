#include "Query.h"

Query::Query() {
	this->aRelationshipClauses = vector<shared_ptr<RelationshipClause>>();
	this->aPatternClauses = vector<shared_ptr<PatternClause>>();
	this->aOptionalClauses = vector<shared_ptr<OptionalClause>>();
	this->aSelectClause = make_shared<SelectClause>();
}

void Query::addDeclarationToSelectClause(shared_ptr<Declaration> declaration) {
	this->aSelectClause->addDeclaration(declaration);
}

void Query::addRelationshipClause(RelationshipType relationshipType, shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput) {
	shared_ptr<RelationshipClause> relationshipClause = make_shared<RelationshipClause>(relationshipType, leftQueryInput, rightQueryInput);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(relationshipClause));
}

void Query::addAssignPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression) {
	shared_ptr<PatternClause> patternClause = make_shared<PatternClause>(synonym, queryInput, expression);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(patternClause));
}

void Query::addContainerPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput) {
	shared_ptr<PatternClause> patternClause = make_shared<PatternClause>(synonym, queryInput);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(patternClause));
}

vector<shared_ptr<OptionalClause>>  Query::getOptionalClauses() {
	return this->aOptionalClauses;
}

shared_ptr<SelectClause> Query::getSelectClause() {
	return this->aSelectClause;
}

vector<shared_ptr<RelationshipClause>> Query::getRelationshipClauses() {
	return this->aRelationshipClauses;
}

vector<shared_ptr<PatternClause>> Query::getPatternClauses() {
	return this->aPatternClauses;
}

// remove after merge with QPP
void Query::setSelectClause(shared_ptr<Declaration> declaration) {
	this->aSelectClause = make_shared<SelectClause>(declaration);
}