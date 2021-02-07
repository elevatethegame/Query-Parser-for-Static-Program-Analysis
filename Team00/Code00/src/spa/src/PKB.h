#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "EntityType.h"

using namespace std;

class PKB {

public:
	/**
	* Constructor for PKB
	* 
	* @param number the number of statements in the program
	*/
	PKB(int n);

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
	bool setStatementType(int index, EntityType type);

	/**
	* Inserts a parent relationship into PKB
	*
	* @param parent the index of the parent statement
	* @param child the index of the child statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertParent(int parent, int child);

	/**
	* Inserts a follow relationship into PKB
	*
	* @param former the index of the statement being followed
	* @param latter the index of the statement following
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertFollow(int former, int latter);

	/**
	* Insert a set of variables directly used by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables a set of variable names being directly used by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertDirectUses(int index, set<string> variables);

	/**
	* Inserts the variable directly modified by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables the name of the variable being directly modified by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertDirectModifies(int index, string variables);

	/**
	* Inserts a hashed expression of an assign statement into PKB
	*
	* @param index the index of the given statement
	* @param expression hashed value of the RHS of the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertExpression(int index, long expression);

	/**
	* Retrieves induces of all statement of some type
	* 
	* @param type
	* 
	* @return set of statement indices as string
	*/
	set<string> getEntities(EntityType type);



private:
	
	const int number; // number of statement
	
	vector<EntityType> types; // type of each statement
	
	map<string, set<string>> relations[5]; // relationship maps
	
	map<string, set<string>> relationsBy[5]; // by-relationship maps
	
	map<long, set<string>> expressions; // map from expression to indices
	
	void extractFollowStar();

	void extractParentStar();

	void extractUses();

	void extractModifies();

};