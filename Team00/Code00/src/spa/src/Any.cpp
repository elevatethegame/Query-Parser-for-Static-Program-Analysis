#include "Any.h"

using namespace std;


Any::Any() {
	this->aValue = "_";

Any::Any(string value) {
	this->aValue = value;
	this->aQueryInputType = QueryInputType::ANY;
}