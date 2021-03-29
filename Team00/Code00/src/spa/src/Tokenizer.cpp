#include "Tokenizer.h"
#include "SyntacticException.h" // for throwing SyntacticException
#include <ctype.h>  // for std::isdigit, std::isalpha
#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end

std::set<char> Tokenizer::specialCharactersAmongIdentifiers = {'*', '_', '.', '#'};

Tokenizer::Tokenizer(const std::string givenInput) : inputStream(givenInput)
{

}

std::unique_ptr<Token> Tokenizer::readInteger()
{
    std::string integer = readWhile(std::isdigit);
    return std::make_unique<Token>(Token{ TokenTypes::Integer, integer });
}

bool Tokenizer::isAlphanumericOrSpecialChar(char c)
{
    for (char allowedSpecialCharacter : Tokenizer::specialCharactersAmongIdentifiers) {
        if (c == allowedSpecialCharacter)
            return true;
    }

    return std::isalnum(c);
}

bool Tokenizer::canTokenizeAsAttrRef(std::string identifier)
{
    int numPeriods = 0;
    int periodIndex = -1;
    for (int i = 0; i < identifier.size(); i++) {
        char c = identifier[i];
        if (c == '.') {
            numPeriods++;
            periodIndex = i;
            if (numPeriods > 1) return false;
        }
    }
    return numPeriods == 1 && periodIndex > 0 && periodIndex < identifier.size() - 1;
}

std::unique_ptr<Token> Tokenizer::readIdentifier()
{
    std::string identifier = std::string(1, inputStream.next());
    identifier += readWhile(Tokenizer::isAlphanumericOrSpecialChar);
    std::string designEntities[] = { "stmt", "read", "print", "while", "if", "assign", 
            "variable", "constant", "procedure", "prog_line", "call" };
    std::string attrName[] = {"procName", "varName", "value", "stmt#"};
    std::unique_ptr<Token> token;
    if (identifier == "Select") {
        token = std::make_unique<Token>(Token{ TokenTypes::Select, identifier });
    }
    else if (identifier == "such") {
        token = std::make_unique<Token>(Token{ TokenTypes::Such, identifier });
    }
    else if (identifier == "that") {
        token = std::make_unique<Token>(Token{ TokenTypes::That, identifier });
    }
    else if (identifier == "pattern") {
        token = std::make_unique<Token>(Token{ TokenTypes::Pattern, identifier });
    }
    else if (identifier == "and") {
        token = std::make_unique<Token>(Token{ TokenTypes::And, identifier });
    }
    else if (identifier == "Modifies") {
        token = std::make_unique<Token>(Token{ TokenTypes::Modifies, identifier });
    }
    else if (identifier == "Uses") {
        token = std::make_unique<Token>(Token{ TokenTypes::Uses, identifier });
    }
    else if (identifier == "Parent") {
        token = std::make_unique<Token>(Token{ TokenTypes::Parent, identifier });
    }
    else if (identifier == "Parent*") {
        token = std::make_unique<Token>(Token{ TokenTypes::ParentT, identifier });
    }
    else if (identifier == "Follows") {
        token = std::make_unique<Token>(Token{ TokenTypes::Follows, identifier });
    }
    else if (identifier == "Follows*") {
        token = std::make_unique<Token>(Token{ TokenTypes::FollowsT, identifier });
    }
    else if (identifier == "Calls") {
        token = std::make_unique<Token>(Token{ TokenTypes::Calls, identifier });
    }
    else if (identifier == "Calls*") {
        token = std::make_unique<Token>(Token{ TokenTypes::CallsT, identifier });
    }
    else if (identifier == "Next") {
        token = std::make_unique<Token>(Token{ TokenTypes::Next, identifier });
    }
    else if (identifier == "Next*") {
        token = std::make_unique<Token>(Token{ TokenTypes::NextT, identifier });
    }
    else if (std::find(std::begin(designEntities), std::end(designEntities), identifier) != std::end(designEntities)) {
        token = std::make_unique<Token>(Token{ TokenTypes::DesignEntity, identifier });
    }
    else if (std::find(std::begin(attrName), std::end(attrName), identifier) != std::end(attrName)) {
        token = std::make_unique<Token>(Token{ TokenTypes::AttrName, identifier });
    }
    else if (std::find(std::begin(attrName), std::end(attrName), identifier) != std::end(attrName)) {
        token = std::make_unique<Token>(Token{ TokenTypes::AttrName, identifier });
    }
    else if (canTokenizeAsAttrRef(identifier)) {
        token = std::make_unique<Token>(Token{ TokenTypes::AttrRef, identifier });
    }
    else {
        // Valid identifier must not contain any special characters, throw syntax error if such characters found
        bool isValidIdentifier = true;
        for (char const& c : identifier) {
            for (char allowedSpecialCharacter : Tokenizer::specialCharactersAmongIdentifiers) {
                if (c == allowedSpecialCharacter) {
                    isValidIdentifier = false;
                    break;
                }
            }
            if (!isValidIdentifier) break;
        }
        if (!isValidIdentifier)
            throw SyntacticException("Invalid identifier encountered: " + identifier);
        token = std::make_unique<Token>(Token{ TokenTypes::Identifier, identifier });
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
        return std::make_unique<Token>(Token{ TokenTypes::LeftParen, std::string(1, inputStream.next()) });
        break;
    case ')':
        return std::make_unique<Token>(Token{ TokenTypes::RightParen, std::string(1, inputStream.next()) });
        break;
    case '"':
        return std::make_unique<Token>(Token{ TokenTypes::DoubleQuote, std::string(1, inputStream.next()) });
        break;
    case '_':
        return std::make_unique<Token>(Token{ TokenTypes::Underscore, std::string(1, inputStream.next()) });
        break;
    case ';':
        return std::make_unique<Token>(Token{ TokenTypes::Semicolon, std::string(1, inputStream.next()) });
        break;
    case ',':
        return std::make_unique<Token>(Token{ TokenTypes::Comma, std::string(1, inputStream.next()) });
        break;
    case '+':
    case '-':
        return std::make_unique<Token>(Token{ TokenTypes::ExprSymbol, std::string(1, inputStream.next()) });
        break;
    case '/':
    case '%':
    case '*':
        return std::make_unique<Token>(Token{ TokenTypes::TermSymbol, std::string(1, inputStream.next()) });
        break;
    default:
        break;
    }
    if (std::isdigit(ch)) return readInteger();
    if (std::isalpha(ch)) return readIdentifier();
    throw SyntacticException("Cannot handle invalid character: " + ch);
}
