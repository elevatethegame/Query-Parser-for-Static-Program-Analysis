#include "Query.h"
#include "catch.hpp"

RelationshipClause* createRelationshipClause();

PatternClause* createPatternClause();

TEST_CASE("Adding Relationship clause pointer to vector") {
	Query query;
	Declaration* declaration = new Declaration(QueryInputType::DECLARATION, EntityType::ASSIGN, "x");
	SelectClause* selectClause = new SelectClause(declaration);
	query.setSelectClause(selectClause);
	
	SECTION("Adding to empty vector") {
		REQUIRE(query.getRelationshipClauses()->size() == 0);
		query.addRelationshipClause(createRelationshipClause());
		REQUIRE(query.getRelationshipClauses()->size() == 1);
	}

	SECTION("Adding to non empty vector") {
		query.addRelationshipClause(createRelationshipClause());
		REQUIRE(query.getRelationshipClauses()->size() == 1);
		query.addRelationshipClause(createRelationshipClause());
		REQUIRE(query.getRelationshipClauses()->size() == 2);
	}
}

TEST_CASE("Adding Pattern clause pointer to vector") {
	Query query;
	Declaration* declaration = new Declaration(QueryInputType::DECLARATION, EntityType::ASSIGN, "x");
	SelectClause* selectClause = new SelectClause(declaration);
	query.setSelectClause(selectClause);

	SECTION("Adding to empty vector") {
		REQUIRE(query.getPatternClauses()->size() == 0);
		query.addPatternClause(createPatternClause());
		REQUIRE(query.getPatternClauses()->size() == 1);
	}

	SECTION("Adding to non empty vector") {
		query.addPatternClause(createPatternClause());
		REQUIRE(query.getPatternClauses()->size() == 1);
		query.addPatternClause(createPatternClause());
		REQUIRE(query.getPatternClauses()->size() == 2);
	}
}

RelationshipClause* createRelationshipClause() {
	QueryInput* leftQueryInput = new QueryInput();
	QueryInput* rightQueryInput = new QueryInput();
	RelationshipClause* relationship = new RelationshipClause(RelationshipType::FOLLOWS, leftQueryInput, rightQueryInput);
	return relationship;
}

PatternClause* createPatternClause() {
	QueryInput* leftQueryInput = new QueryInput();
	QueryInput* rightQueryInput = new QueryInput();
	Expression* expression = new Expression("_");
	PatternClause* pattern = new PatternClause(leftQueryInput, rightQueryInput, expression);
	return pattern;
}
