#pragma once

#include "Declaration.h"
#include <memory>

class SelectClause {
private:
	shared_ptr<Declaration> aDeclaration;

public:
	SelectClause(shared_ptr<Declaration> declaration);
	shared_ptr<Declaration> getDeclaration();
};