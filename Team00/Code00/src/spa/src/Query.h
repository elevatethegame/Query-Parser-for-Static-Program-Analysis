#pragma once
#include <string>
#include <vector>
#include "Declaration.h"
#include "SelectClause.h"
#include "RelationshipClause.h"
#include "PatternClause.h"

using namespace std;

class Query {
private:
	SelectClause* aSelectClause;
	vector<RelationshipClause*>* aRelationshipClauses;
	vector<PatternClause*>* aPatternClauses;

public:
	Query();
	~Query();
	void setSelectClause(Declaration* declaration);
	void addRelationshipClause(RelationshipType relationshipType,
		QueryInput* leftQueryInput, QueryInput* rightQueryInput);
	void addPatternClause(QueryInput* synonym, QueryInput* queryInput, Expression* expression);

	SelectClause* getSelectClause();
	vector<RelationshipClause*>* getRelationshipClauses();
	vector<PatternClause*>* getPatternClauses();
};