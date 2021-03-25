#pragma once
#ifndef TOKENIZERINTERFACE_H
#define TOKENIZERINTERFACE_H
#include "Token.h"
#include <memory>  // for std::unique_ptr (smart pointers)

class TokenizerInterface {

public:

	virtual std::unique_ptr<Token> readNext() = 0;

};

#endif