#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>

#include "PKBInterface.h"
#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInput.h"
#include "Declaration.h"
#include "Expression.h"

using namespace std;

inline bool operator==(const Expression& a, const Expression& b) {
	return a.getType() == b.getType() && a.getValue() == b.getValue();
}

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

struct EnumClassHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

struct KeyHasher {
	std::size_t operator()(const Expression& e) const {
		using std::size_t;
		using std::hash;
		using std::string;

		return ((hash<string>()(e.getValue())
			^ (hash<ExpressionType>()(e.getType()) << 1)) >> 1);
	}
};

class PKB : public PKBInterface {

public:
	/**
	* Constructor for PKB
	*
	* @param number the number of statements in the program
	*/
	PKB(const int& n);

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

	bool insertParentStar(const int& parent, const int& child);

	/**
	* Inserts a follow relationship into PKB
	*
	* @param former the index of the statement being followed
	* @param latter the index of the statement following
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertFollow(const int& former, const int& latter);

	bool insertFollowStar(const int& former, const int& latter);

	/**
	* Inserts a next relationship into PKB
	*
	* @param former the index of the statement before
	* @param latter the index of the statement after
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertNext(const int& former, const int& latter);

	bool insertNextStar(const int& former, const int& latter);

	/**
	* Insert a set of variables used by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables the name of the variable being used by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertUses(const int& index, const string& variable);

	bool insertProcUses(const string& procedure, const string& variable);

	/**
	* Inserts a set of variables modified by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables the name of the variable being modified by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertModifies(const int& index, const string& variable);

	bool insertProcModifies(const string& procedure, const string& variable);

	/**
	* Inserts a calls relationship into PKB
	*
	* @param caller name of the procedure that calls
	* @param callee name of the procedure being called
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertCalls(const string& caller, const string& callee);

	bool insertCallsStar(const string& caller, const string& callee);

	/**
	* Inserts an expression of an assign statement into PKB
	*
	* @param index the index of the given statement
	* @param expression hashed value of the RHS of the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertExpression(const int& index, const Expression& expression);


	/**
	* Inserts a control variable of a container statement into PKB.
	* The specified statement should have been set as IF or WHILE beforehand.
	*
	* @param index the index of the given statement
	* @param varaible control variable of the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool setControlVariable(const int& index, const string& variable);

	/**
	* Inserts a variable name into PKB
	*
	* @param variable
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertVariable(const string& variable);

	bool insertConst(const string& number);

	bool insertProcedure(const string& proc);

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
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	/**
	* Retrieves results of a given relationship query
	*
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	*
	* @return set of results there is exactly one Declaration
	*/
	set<string> getSetResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	/**
	* Retrieves results of a given relationship query
	*
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	*
	* @return set of results there are exactly two Declarations
	*
	*/
	unordered_map<string, set<string>> getMapResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);


	/**
	* Retrieves results of an assign pattern query
	*
	* @param type the type of statements being queried
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a set of indices if input is Any or Ident
	*
	*/
	set<string> getSetResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	/**
	* Retrieves results of a pattern query
	*
	* @param type the type of statements being queried
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a map of results (stmt_index, set<input_value>) if input is a Declaration
	*
	*/
	unordered_map<string, set<string>> getMapResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	set<string> getSetResultsOfContainerPattern(const EntityType& type, shared_ptr<QueryInput> input);

	unordered_map<string, set<string>> getMapResultsOfContainerPattern(const EntityType& type, shared_ptr<QueryInput> input);


private:

	const int number; // number of statement

	unordered_map<string, EntityType> types; // map from index to type

	unordered_map<EntityType, set<string>, EnumClassHash> entities; // map from type to indices

	unordered_map<string, set<string>> relations[12]; // relationship maps

	unordered_map<string, set<string>> relationsBy[12]; // by-relationship maps

	set<string> relationKeys[13]; // key set for relationships

	set<string> relationByKeys[13]; // key set for by-relationships 

	// procedure uses / modifies

	unordered_map<string, set<string>> procUses, procModifies;

	unordered_map<string, set<string>> usedByProc, modifiedByProc;

	set<string> procUsesKeys, procModifiesKeys;

	set<string> usedByProcKeys, modByProcKeys;

	// container pattern

	unordered_map<string, set<string>> contPattern[2]; // map from index -> var

	unordered_map<string, set<string>> contPatternBy[2]; // map from var -> index

	set<string> contPatternKeys[2]; // key set for contPattern

	// assign pattern

	unordered_map<Expression, set<string>, KeyHasher> expressions; // map from expression to indices


	// utility methods

	bool insertRelationship(const RelationshipType& type, const string& e1, const string& e2);

	void filterSetOfType(const EntityType& type, set<string>* res);

	void filterMapOfType(
		const EntityType& t1, const EntityType& t2,
		unordered_map<string, set<string>>* res);
};
