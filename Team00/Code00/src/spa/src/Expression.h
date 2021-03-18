#pragma once
#include "ExpressionType.h"
#include <string>

using namespace std;

class Expression {

private:

	string aValue;
	ExpressionType type;

public:

	Expression combineExpression(string operand, const Expression& other, ExpressionType t);

	Expression(string value, ExpressionType t);

	string getValue() const;

	ExpressionType getType() const;

};

