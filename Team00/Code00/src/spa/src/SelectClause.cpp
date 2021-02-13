#include "SelectClause.h"

SelectClause::SelectClause(shared_ptr<Declaration> declaration) {
	this->aDeclaration = declaration;
}

shared_ptr<Declaration> SelectClause::getDeclaration() {
	return this->aDeclaration;
}