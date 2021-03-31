#pragma once
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "InputStream.h"
#include "TokenizerInterface.h"
#include <functional>
#include <set>

class Tokenizer : public TokenizerInterface
{
private:

    static std::set<char> specialCharactersAmongIdentifiers;
    InputStream inputStream;

    static bool isAlphanumericOrSpecialChar(char c);
    std::unique_ptr<Token> readInteger();
    bool canTokenizeAsAttrRef(std::string identifier);
    std::unique_ptr<Token> readIdentifier();
    std::string readWhile(std::function<bool(char)> predicate);

public:

    static bool canTreatAsIdent(TokenTypes type, std::string value);
    Tokenizer(const std::string givenInput);
    std::unique_ptr<Token> readNext();

};

#endif
