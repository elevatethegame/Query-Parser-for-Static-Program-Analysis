#include "Expression.h"

using namespace std;

Expression::Expression(string value) : aValue(value) {}

string Expression::getValue() {
	return this->aValue;
}
