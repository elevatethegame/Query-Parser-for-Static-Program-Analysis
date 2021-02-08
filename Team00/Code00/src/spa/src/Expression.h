#pragma once

#include <string>

using namespace std;

class Expression {
private:
	string aValue;

public:
	Expression(string value);
	string getValue();
};