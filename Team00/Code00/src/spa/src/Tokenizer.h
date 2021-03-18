#pragma once
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "InputStream.h"
#include "Token.h"
#include <functional>
#include <memory>  // for std::unique_ptr (smart pointers)

class Tokenizer
{
private:

    InputStream inputStream;

    static bool isAlphanumericOrUnderscore(char c);
    std::unique_ptr<Token> readInteger();
    std::unique_ptr<Token> readIdentifier();
    std::string readWhile(std::function<bool(char)> predicate);

public:

    Tokenizer(const std::string givenInput);
    std::unique_ptr<Token> readNext();

};

#endif
