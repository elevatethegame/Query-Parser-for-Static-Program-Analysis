#include "TokenizerStub.h"

TokenizerStub::TokenizerStub(std::vector<Token> tokens) : m_tokens(tokens)
{

}

std::unique_ptr<Token> TokenizerStub::readNext() 
{
    if (m_index >= m_tokens.size()) return std::unique_ptr<Token>();  // No more tokens left
    Token currToken = m_tokens.at(m_index);
    m_index++;
    return std::make_unique<Token>(currToken);
}