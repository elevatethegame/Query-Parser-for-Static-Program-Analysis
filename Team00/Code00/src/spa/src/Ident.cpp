#include "Ident.h"

Ident::Ident(QueryInputType queryInputType, std::string value) {
    m_queryInputType = queryInputType;
    m_value = value;
}