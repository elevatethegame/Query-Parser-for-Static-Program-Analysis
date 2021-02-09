#include "StmtNum.h"

using namespace std;

StmtNum::StmtNum(string value) {
	this->aQueryInputType = QueryInputType::STMT_NUM;
	this->aValue = value;
}