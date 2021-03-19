#pragma once
#include "ExpressionType.h"
#include <string>

using namespace std;

class Expression {

private:

	string aValue;
	ExpressionType type;

public:

	Expression(string value); // remove after merge with QPP

	Expression combineExpression(string operand, const Expression& other); // remove after merge with PKB/Parser side

	Expression combineExpression(string operand, const Expression& other, ExpressionType t);

	Expression(string value, ExpressionType t);

	string getValue() const;

	ExpressionType getType() const;

};

