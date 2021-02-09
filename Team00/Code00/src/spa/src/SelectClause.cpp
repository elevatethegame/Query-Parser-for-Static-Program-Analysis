#include "SelectClause.h"

SelectClause::SelectClause(Declaration* declaration) {
	aDeclaration = declaration;
}

SelectClause::~SelectClause() {
	delete aDeclaration;
}

Declaration* SelectClause::getDeclaration() {
	return aDeclaration;
}