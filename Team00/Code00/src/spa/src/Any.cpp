#include "Any.h"

using namespace std;

Any::Any(string value) {
	this->aValue = value;
	this->aQueryInputType = QueryInputType::ANY;
}