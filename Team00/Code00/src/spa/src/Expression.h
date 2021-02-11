#pragma once
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

class Expression {
private:
	std::string m_value;

public:
	Expression(std::string value);
	std::string getValue();
};

#endif

