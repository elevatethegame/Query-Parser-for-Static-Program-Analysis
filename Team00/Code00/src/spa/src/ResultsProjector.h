#pragma once

#include <string>
#include <list>
#include <memory>
#include "SelectClause.h"
#include "ResultsTable.h"
#include "PKBInterface.h"

class ResultsProjector {
public:
	static void projectResults(shared_ptr<ResultsTable> evaluatedResults, shared_ptr<SelectClause> selectClause, shared_ptr<PKBInterface> PKB, list<string>& results);
};