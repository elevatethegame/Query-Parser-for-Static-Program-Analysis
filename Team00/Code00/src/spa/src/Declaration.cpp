#include "Declaration.h"

Declaration::Declaration(QueryInputType queryInputType, std::string value, EntityType entityType) {
    m_queryInputType = queryInputType;
    m_value = value;
    m_entityType = entityType;
}

EntityType Declaration::getEntityType() {
    return m_entityType;
}