#pragma once

#include <string>
#include <list>
#include "SelectClause.h"
#include "ResultsTable.h"
#include "PKBInterface.h"

class ResultsProjector {
public:
	static void projectResults(ResultsTable* evaluatedResults, SelectClause* selectClause, PKBInterface* PKB, list<string> results);
};