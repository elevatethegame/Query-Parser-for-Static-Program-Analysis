#pragma once
#include "Any.h"

Any::Any(string value) {
	aQueryInputType = QueryInputType::ANY;
	aValue = value;
}