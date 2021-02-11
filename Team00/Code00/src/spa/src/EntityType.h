#pragma once
#ifndef ENTITYTYPE_H
#define ENTITYTYPE_H

enum class EntityType {
	While,
	If,
	Assign,
	Stmt,
	Variable,
	Constant,
	Procedure,
	Read,
	Print,
	Call,
	Null
};

#endif