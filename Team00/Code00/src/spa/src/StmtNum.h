#pragma once
#ifndef STMTNUM_H
#define STMTNUM_H

#include "QueryInput.h"
#include <string>

class StmtNum : public QueryInput {
public:
	StmtNum(QueryInputType queryInputType, std::string value);
};

#endif
