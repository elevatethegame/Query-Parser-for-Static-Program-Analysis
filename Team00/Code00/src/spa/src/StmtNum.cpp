#include <string>
#include "StmtNum.h"
#include "QueryInput.h"
using namespace std;

StmtNum::StmtNum(string value) {
	aValue = value;
	aQueryInputType = QueryInputType::STMT_NUM;
};