#pragma once
#include <string>

using namespace std;

class Expression {
private:
	string aValue;

public:
	Expression combineExpression(string operand, const Expression& other);

	Expression(string value);
	string getValue() const;
};

