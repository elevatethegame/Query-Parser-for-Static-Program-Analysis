#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <map>

#include "EntityType.h"

using namespace std;

#include "PKB.h"

PKB::PKB(int n) : number(n), types(n) {}

void PKB::init() {
}

bool PKB::setStatementType(int index, EntityType type) {
	return true;
}

bool PKB::insertParent(int parent, int child) {
	return true;
}


bool PKB::insertFollow(int former, int latter) {
	return true;
}


bool PKB::insertDirectUses(int index, set<string> variables) {
	return true;
}

bool PKB::insertDirectModifies(int index, string variables) {
		return true;
}

bool PKB::insertExpression(int index, long expression) {
	return true;
}

set<string> PKB::getEntities(EntityType type) {
	return set<string>();
}

void PKB::extractFollowStar() {
}

void PKB::extractParentStar() {
}

void PKB::extractUses() {
}

void PKB::extractModifies() {
}