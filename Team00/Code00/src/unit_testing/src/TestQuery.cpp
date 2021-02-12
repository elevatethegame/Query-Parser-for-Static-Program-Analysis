#include "Query.h"
#include "catch.hpp"

TEST_CASE("Adding Relationship clause pointer to vector") {
	Query query;
	QueryInput* leftQueryInput = new QueryInput();
	QueryInput* rightQueryInput = new QueryInput();
	Declaration* declaration = new Declaration(EntityType::ASSIGN, "x");
	query.setSelectClause(declaration);
	
	SECTION("Adding to empty vector") {
		REQUIRE(query.getRelationshipClauses()->size() == 0);
		query.addRelationshipClause(RelationshipType::FOLLOWS, leftQueryInput, rightQueryInput);
		REQUIRE(query.getRelationshipClauses()->size() == 1);
		REQUIRE(query.getRelationshipClauses()->at(0)->getRelationshipType() == RelationshipType::FOLLOWS);
	}

	SECTION("Adding to non empty vector") {
		QueryInput* leftQueryInput2 = new QueryInput();
		QueryInput* rightQueryInput2 = new QueryInput();
		query.addRelationshipClause(RelationshipType::FOLLOWS, leftQueryInput, rightQueryInput);
		REQUIRE(query.getRelationshipClauses()->size() == 1);
		query.addRelationshipClause(RelationshipType::MODIFIES, leftQueryInput2, rightQueryInput2);
		REQUIRE(query.getRelationshipClauses()->size() == 2);
		REQUIRE(query.getRelationshipClauses()->at(0)->getRelationshipType() == RelationshipType::FOLLOWS);
		REQUIRE(query.getRelationshipClauses()->at(1)->getRelationshipType() == RelationshipType::MODIFIES);
	}
}

TEST_CASE("Adding Pattern clause pointer to vector") {
	Query query;
	QueryInput* leftQueryInput = new QueryInput();
	QueryInput* rightQueryInput = new QueryInput();
	Expression* expression = new Expression("_");
	Declaration* declaration = new Declaration(EntityType::ASSIGN, "x");
	query.setSelectClause(declaration);

	SECTION("Adding to empty vector") {
		REQUIRE(query.getPatternClauses()->size() == 0);
		query.addPatternClause(leftQueryInput, rightQueryInput, expression);
		REQUIRE(query.getPatternClauses()->size() == 1);
	}

	SECTION("Adding to non empty vector") {
		QueryInput* leftQueryInput2 = new QueryInput();
		QueryInput* rightQueryInput2 = new QueryInput();
		Expression* expression2 = new Expression("_");
		query.addPatternClause(leftQueryInput, rightQueryInput, expression);
		REQUIRE(query.getPatternClauses()->size() == 1);
		query.addPatternClause(leftQueryInput2, rightQueryInput2, expression2);
		REQUIRE(query.getPatternClauses()->size() == 2);
	}
}