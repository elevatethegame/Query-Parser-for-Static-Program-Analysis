#include "StmtNum.h"

using namespace std;

StmtNum::StmtNum(int value) {
	this->aQueryInputType = QueryInputType::STMT_NUM;
	this->aValue = to_string(value);
}