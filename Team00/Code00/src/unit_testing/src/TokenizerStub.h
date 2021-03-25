#pragma once
#ifndef TOKENIZERSTUB_H
#define TOKENIZERSTUB_H
#include "TokenizerInterface.h"
#include <vector>

class TokenizerStub : public TokenizerInterface {
private:

	std::vector<Token> m_tokens;
	int m_index{ 0 };

public:

	TokenizerStub(std::vector<Token> tokens);

	std::unique_ptr<Token> readNext();

};

#endif