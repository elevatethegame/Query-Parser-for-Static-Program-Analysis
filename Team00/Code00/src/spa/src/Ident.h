#pragma once
#ifndef IDENT_H
#define IDENT_H

#include "QueryInput.h"
#include <string>

class Ident : public QueryInput {
public:
	Ident(QueryInputType queryInputType, std::string value);
};

#endif