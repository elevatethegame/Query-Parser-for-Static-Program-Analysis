#include "Any.h"

Any::Any(QueryInputType queryInputType, std::string value) {
    m_queryInputType = queryInputType;
    m_value = value;
}