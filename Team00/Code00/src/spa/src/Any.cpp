#pragma once
#include <string>
#include "QueryInput.h"

using namespace std;

class Any : public QueryInput {
public:
	Any(QueryInputType queryInputType, string value);
};