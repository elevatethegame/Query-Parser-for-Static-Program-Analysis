#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>

#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInput.h"
#include "Declaration.h"
#include "Expression.h"

using namespace std;

/*
struct KeyHash {
	std::size_t operator()(const string& k) const {
		return std::stoi(k);
	}
};

struct KeyEqual {
	bool operator()(const string& lhs, const string& rhs) const {
		return std::stoi(lhs) == std::stoi(rhs);
	}
};

struct KeyComp {
	bool operator()(const string& lhs, const string& rhs) const {
		return std::stoi(lhs) < std::stoi(rhs);
	}
};
*/

class PKB {

public:
	/**
	* Constructor for PKB
	* 
	* @param number the number of statements in the program
	*/
	PKB(const int& n);

	/**
	* Initializes the PKB after the parser finishes 
	*/
	void init();

	/**
	* Sets the type of an indexed statement in PKB
	*
	* @param index
	* @param type
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool setStatementType(const int& index, const EntityType& type);

	/**
	* Inserts a parent relationship into PKB
	*
	* @param parent the index of the parent statement
	* @param child the index of the child statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertParent(const int& parent, const int& child);

	/**
	* Inserts a follow relationship into PKB
	*
	* @param former the index of the statement being followed
	* @param latter the index of the statement following
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertFollow(const int& former, const int& latter);

	/**
	* Insert a set of variables directly used by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables a set of variable names being directly used by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertDirectUses(const int& index, const set<string>& variables);

	/**
	* Inserts the variable directly modified by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables the name of the variable being directly modified by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertDirectModifies(const int& index, const string& variables);

	/**
	* Inserts a hashed expression of an assign statement into PKB
	*
	* @param index the index of the given statement
	* @param expression hashed value of the RHS of the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertExpression(const int& index, const long& expression);

	/**
	* Retrieves indices of all statements of some type as string
	* 
	* @param type
	* 
	* @return set of statement indices as string
	*/
	set<string> getEntities(const EntityType& type);

	/**
	* Retrieves a boolean result of a given relationship query
	* 
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	* 
	* @return a boolean indicating whether the relationship is true.
	* if there is any Declaration in inputs, returns false
	*/
	bool getBooleanResultOfRS(const RelationshipType& type, 
		const QueryInput& input1, const QueryInput& input2);

	/**
	* Retrieves results of a given relationship query
	*
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	*
	* @return set of results with a dummy key if there is exactly one Declaration,
	* or a map of (input1_value, set<input2_value>) if there are two,
	* or an empty map if neither of the inputs is a Declaration
	*/
	unordered_map<string, set<string>> getResultsOfRS(
		const RelationshipType& type, const QueryInput& input1, const QueryInput& input2);

	/**
	* Retrieves a boolean result of a pattern query
	*
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a boolean indicating whether the relationship is true.
	* if the input is a Declaration, returns false
	*/
	bool getBooleanResultOfPattern(const QueryInput& input, const Expression& expression);

	/**
	* Retrieves results of a pattern query
	*
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a map of results (stmt_index, set<input_value>) 
	*/
	unordered_map<string, set<string>> getResultsOfPattern(
		const Declaration& input, const Expression& expression);


private:
	
	const int number; // number of statement
	
	unordered_map<string, EntityType> types; // map from index to type
	
	unordered_map<EntityType, set<string>> stmts; // map from type to indices

	unordered_map<string, set<string>> relations[6]; // relationship maps
	
	unordered_map<string, set<string>> relationsBy[6]; // by-relationship maps
	
	unordered_map<long, set<string>> expressions; // map from expression to indices
	
	void extractFollowStar();

	void extractParentStar();

	void extractUses();

	void extractModifies();

};