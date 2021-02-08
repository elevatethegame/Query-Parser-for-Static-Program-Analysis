#include <string>
#include "StmtNum.h"
#include "QueryInput.h"
using namespace std;

StmtNum::StmtNum(QueryInputType queryInputType, string value) {
	aValue = value;
	aQueryInputType = queryInputType;
};