#include "Expression.h"

Expression::Expression(string value) {
	aValue = value;
}

string Expression::getValue() {
	return aValue;
}