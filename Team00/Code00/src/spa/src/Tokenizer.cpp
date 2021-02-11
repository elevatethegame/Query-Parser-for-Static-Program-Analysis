#include "Tokenizer.h"
#include <ctype.h>  // for std::isdigit, std::isalpha
#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end
#include <stdexcept> // for std::invalid_argument

Tokenizer::Tokenizer(const std::string givenInput) : inputStream(givenInput)
{

}

std::unique_ptr<Token> Tokenizer::readInteger()
{
    std::string integer = readWhile(std::isdigit);
    return std::unique_ptr<Token>(new Token{ TokenTypes::Integer, integer });
}

std::unique_ptr<Token> Tokenizer::readIdentifier()
{
    std::string identifier = std::string(1, inputStream.next());
    identifier += readWhile(std::isalnum);
    std::string designEntities[] = { "stmt", "read", "print", "while", "if", "assign", "variable", "constant", "procedure" };
    std::unique_ptr<Token> token;
    if (identifier == "Select") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Select, identifier });
    }
    else if (identifier == "such") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Such, identifier });
    }
    else if (identifier == "that") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::That, identifier });
    }
    else if (identifier == "pattern") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Pattern, identifier });
    }
    else if (identifier == "Modifies") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Modifies, identifier });
    }
    else if (identifier == "Uses") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Uses, identifier });
    }
    else if (identifier == "Parent") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Parent, identifier });
    }
    else if (identifier == "Follows") {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Follows, identifier });
    }
    else if (std::find(std::begin(designEntities), std::end(designEntities), identifier) != std::end(designEntities)) {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::DesignEntity, identifier });
    }
    else {
        token = std::unique_ptr<Token>(new Token{ TokenTypes::Identifier, identifier });
    }
    return token;
}

std::string Tokenizer::readWhile(std::function<bool(char)> predicate)
{
    std::string str = "";
    while (!inputStream.eof() && predicate(inputStream.peek())) {
        str += inputStream.next();
    }
    return str;
}

std::unique_ptr<Token> Tokenizer::readNext()
{
    readWhile(std::isspace);
    if (inputStream.eof()) return std::unique_ptr<Token>();
    char ch = inputStream.peek();
    switch (ch)
    {
    case '(':
        return std::unique_ptr<Token>(new Token{ TokenTypes::LeftParen, std::string(1, inputStream.next()) });
        break;
    case ')':
        return std::unique_ptr<Token>(new Token{ TokenTypes::RightParen, std::string(1, inputStream.next()) });
        break;
    case '"':
        return std::unique_ptr<Token>(new Token{ TokenTypes::DoubleQuote, std::string(1, inputStream.next()) });
        break;
    case '_':
        return std::unique_ptr<Token>(new Token{ TokenTypes::Underscore, std::string(1, inputStream.next()) });
        break;
    case ';':
        return std::unique_ptr<Token>(new Token{ TokenTypes::Semicolon, std::string(1, inputStream.next()) });
        break;
    case ',':
        return std::unique_ptr<Token>(new Token{ TokenTypes::Comma, std::string(1, inputStream.next()) });
        break;
    case '*':
        return std::unique_ptr<Token>(new Token{ TokenTypes::Asterisk, std::string(1, inputStream.next()) });
        break;
    default:
        break;
    }
    if (std::isdigit(ch)) return readInteger();
    if (std::isalpha(ch)) return readIdentifier();
    throw std::invalid_argument("Cannot handle invalid character: " + ch);
}
