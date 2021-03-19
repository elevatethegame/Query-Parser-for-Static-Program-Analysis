#include "Query.h"
#include "catch.hpp"

TEST_CASE("Adding Relationship clause pointer to vector") {
	Query query;
	shared_ptr<QueryInput> leftQueryInput = make_shared<QueryInput>();
	shared_ptr<QueryInput> rightQueryInput = make_shared<QueryInput>();
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "x");
	query.setSelectClause(declaration);
	
	SECTION("Adding to empty vector") {
		REQUIRE(query.getRelationshipClauses().size() == 0);
		query.addRelationshipClause(RelationshipType::FOLLOWS, leftQueryInput, rightQueryInput);
		REQUIRE(query.getRelationshipClauses().size() == 1);
		REQUIRE(query.getRelationshipClauses().at(0)->getRelationshipType() == RelationshipType::FOLLOWS);
	}

	SECTION("Adding to non empty vector") {
		shared_ptr<QueryInput> leftQueryInput2 = make_shared<QueryInput>();
		shared_ptr<QueryInput> rightQueryInput2 = make_shared<QueryInput>();
		query.addRelationshipClause(RelationshipType::FOLLOWS, leftQueryInput, rightQueryInput);
		REQUIRE(query.getRelationshipClauses().size() == 1);
		query.addRelationshipClause(RelationshipType::MODIFIES, leftQueryInput2, rightQueryInput2);
		REQUIRE(query.getRelationshipClauses().size() == 2);
		REQUIRE(query.getRelationshipClauses().at(0)->getRelationshipType() == RelationshipType::FOLLOWS);
		REQUIRE(query.getRelationshipClauses().at(1)->getRelationshipType() == RelationshipType::MODIFIES);
	}
}

TEST_CASE("Adding Pattern clause pointer to vector") {
	Query query;
	shared_ptr<Declaration> assignDeclaration1 = make_shared<Declaration>(EntityType::ASSIGN, "a");
	shared_ptr<Declaration> containerDeclaration1 = make_shared<Declaration>(EntityType::WHILE, "w");
	shared_ptr<QueryInput> rightQueryInput = make_shared<QueryInput>();
	shared_ptr<Expression> expression = make_shared<Expression>("_");

	shared_ptr<Declaration> assignDeclaration2 = make_shared<Declaration>(EntityType::ASSIGN, "a");
	shared_ptr<Declaration> containerDeclaration2 = make_shared<Declaration>(EntityType::IF, "ifs");
	shared_ptr<QueryInput> rightQueryInput2 = make_shared<QueryInput>();
	shared_ptr<Expression> expression2 = make_shared<Expression>("_");

	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "x");
	query.setSelectClause(declaration);

	SECTION("Adding assign pattern clause to empty vector") {
		REQUIRE(query.getPatternClauses().size() == 0);
		query.addAssignPatternClause(assignDeclaration1, rightQueryInput, expression);
		REQUIRE(query.getPatternClauses().size() == 1);
	}

	SECTION("Adding assign pattern clause to non empty vector") {
		query.addAssignPatternClause(assignDeclaration1, rightQueryInput, expression);
		REQUIRE(query.getPatternClauses().size() == 1);
		query.addAssignPatternClause(assignDeclaration2, rightQueryInput2, expression2);
		REQUIRE(query.getPatternClauses().size() == 2);
	}

	SECTION("Adding container pattern clause to empty vector") {
		REQUIRE(query.getPatternClauses().size() == 0);
		query.addContainerPatternClause(containerDeclaration1, rightQueryInput);
		REQUIRE(query.getPatternClauses().size() == 1);
	}

	SECTION("Adding container pattern clause to non empty vector") {
		query.addContainerPatternClause(containerDeclaration1, rightQueryInput);
		REQUIRE(query.getPatternClauses().size() == 1);
		query.addContainerPatternClause(containerDeclaration2, rightQueryInput2);
		REQUIRE(query.getPatternClauses().size() == 2);
	}
}