#pragma once

#include "Declaration.h"

class SelectClause {
private:
	Declaration* aDeclaration;

public:
	SelectClause(Declaration* declaration);
	~SelectClause();
	Declaration* getDeclaration();
};