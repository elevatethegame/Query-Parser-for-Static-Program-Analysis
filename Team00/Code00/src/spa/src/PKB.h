#pragma once

#include<stdio.h>
#include <iostream>
#include "PKBInterface.h"

using namespace std;
typedef short PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer

class PKB : public PKBInterface{
public:
	static VarTable* varTable; 
	static int setProcToAST(PROC p, TNode* r);
	static TNode* getRootAST (PROC p);

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
	* Retrieves results of a pattern query
	*
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a map of results (stmt_index, set<input_value>)
	*/
	unordered_map<string, set<string>> getResultsOfPattern(
		const EntityType& type, QueryInput input, Expression expression);

};