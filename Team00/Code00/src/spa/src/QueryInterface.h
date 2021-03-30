#pragma once

#include "Declaration.h"
#include "SelectClause.h"
#include "RelationshipClause.h"
#include "PatternClause.h"

class QueryInterface {
public:
	virtual void setSelectClause(shared_ptr<Declaration> declaration) = 0; // remove after merge with qpp
	virtual void addDeclarationToSelectClause(shared_ptr<Declaration> declaration) = 0;
	virtual void addRelationshipClause(RelationshipType relationshipType,
		shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput) = 0;
	virtual void addAssignPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput,
		shared_ptr<Expression> expression) = 0;
	virtual void addContainerPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput) = 0;
	virtual shared_ptr<SelectClause> getSelectClause() = 0;
	virtual vector<shared_ptr<OptionalClause>> getOptionalClauses() = 0;

	// remove after merge with QPP
	virtual vector<shared_ptr<RelationshipClause>> getRelationshipClauses() = 0;
	virtual vector<shared_ptr<PatternClause>> getPatternClauses() = 0;
};