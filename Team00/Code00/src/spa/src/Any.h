#pragma once
#include "Any.h"

Any::Any(QueryInputType queryInputType, string value) {
	aQueryInputType = queryInputType;
	aValue = value;
}