#pragma once
#include <string>
#include <vector>
#include <memory>
#include "QueryInterface.h"
#include "Declaration.h"
#include "SelectClause.h"
#include "RelationshipClause.h"
#include "PatternClause.h"

using namespace std;

class Query : public QueryInterface{
private:
	shared_ptr<SelectClause> aSelectClause;
	vector<shared_ptr<RelationshipClause>> aRelationshipClauses;
	vector<shared_ptr<PatternClause>> aPatternClauses;

public:
	Query();
	void setSelectClause(shared_ptr<Declaration> declaration);
	void addRelationshipClause(RelationshipType relationshipType,
		shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput);
	void addPatternClause(shared_ptr<QueryInput> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression);

	shared_ptr<SelectClause> getSelectClause();
	vector<shared_ptr<RelationshipClause>> getRelationshipClauses();
	vector<shared_ptr<PatternClause>> getPatternClauses();
};