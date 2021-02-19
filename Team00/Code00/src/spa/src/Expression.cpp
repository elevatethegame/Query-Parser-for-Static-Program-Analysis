#include "Expression.h"
#include <iostream>

using namespace std;

Expression::Expression(string value) : aValue(value) {}

//currently just return string addition
Expression Expression::combineExpression(string operand, const Expression& other) {
    //cout << "combining " <<  "(" + this->getValue() + operand + other.getValue() + ")" << endl;
	return Expression( "(" + this->getValue() + operand + other.getValue() + ")" );
}

string Expression::getValue() const {
	return this->aValue;
}
