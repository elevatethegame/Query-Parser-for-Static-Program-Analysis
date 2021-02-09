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
	void setSelectClause(SelectClause* selectClause);
	void addRelationshipClause(RelationshipClause* relationshipClause);
	void addPatternClause(PatternClause* patternClause);

	SelectClause* getSelectClause();
	vector<RelationshipClause*>* getRelationshipClauses();
	vector<PatternClause*>* getPatternClauses();

	void deletePointers();
};