#pragma once
#ifndef ANY_H
#define ANY_H

#include "QueryInput.h"
#include <string>

class Any : public QueryInput {
public:
	Any(QueryInputType queryInputType, std::string value);
};

#endif
