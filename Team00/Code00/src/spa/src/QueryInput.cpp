#include "QueryInput.h"

QueryInputType QueryInput::getType() {
	return m_queryInputType;
}

std::string QueryInput::getValue() {
	return m_value;
}