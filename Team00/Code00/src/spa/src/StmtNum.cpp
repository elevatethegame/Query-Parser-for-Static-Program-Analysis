#include "StmtNum.h"

StmtNum::StmtNum(QueryInputType queryInputType, std::string value) {
    m_queryInputType = queryInputType;
    m_value = value;
}