#include "Expression.h"
#include <iostream>

using namespace std;

Expression::Expression(string value, ExpressionType t) : aValue(value), type(t) {}

//currently just return string addition
Expression Expression::combineExpression(
	string operand, const Expression& other, ExpressionType t) {
	//cout << "combining " <<  "(" + this->getValue() + operand + other.getValue() + ")" << endl;
	return Expression("(" + this->getValue() + operand + other.getValue() + ")", t);
}

string Expression::getValue() const {
	return this->aValue;
}

ExpressionType Expression::getType() const {
	return this->type;
}
