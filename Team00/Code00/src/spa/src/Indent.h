#pragma once

#include <string>
#include "QueryInput.h"

using namespace std;

class Indent : public QueryInput {
public:
	Indent(string value);
};