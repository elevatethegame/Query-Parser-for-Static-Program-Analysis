#pragma once
#ifndef QUERYINPUT_H
#define QUERYINPUT_H

#include "QueryInputType.h"
#include <string>

class QueryInput {
protected:
	std::string m_value;
	QueryInputType m_queryInputType;

public:
	// Dynamic_cast works ONLY with polymorphic class.
	// Polymorphic class is that which has atleast one virtual function, even be it the destructor.
	// Hence this is necessary in order for QueryInput to be dynamic-casted to Declaration
	virtual ~QueryInput() {};

	QueryInputType getType();

	std::string getValue();
};

#endif 