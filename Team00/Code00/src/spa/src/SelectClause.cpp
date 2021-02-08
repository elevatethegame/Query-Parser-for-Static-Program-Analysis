#include "SelectClause.h"

SelectClause::SelectClause(Declaration* declaration) {
	aDeclaration = declaration;
}

Declaration* SelectClause::getDeclaration() {
	return aDeclaration;
}