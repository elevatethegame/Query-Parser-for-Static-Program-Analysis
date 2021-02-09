#include "Indent.h"

using namespace std;

Indent::Indent(string value) {
	this->aQueryInputType = QueryInputType::INDENT;
	this->aValue = value;
}