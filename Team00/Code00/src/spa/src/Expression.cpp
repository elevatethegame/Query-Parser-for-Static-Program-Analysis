#include "Expression.h"

Expression::Expression(std::string value) : m_value(value) {

}

std::string Expression::getValue() {
    return m_value;
}