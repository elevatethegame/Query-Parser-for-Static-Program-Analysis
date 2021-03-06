#include "Tokenizer.h"
#include "SyntacticException.h" // for throwing SyntacticException
#include <ctype.h>  // for std::isdigit, std::isalpha
#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end


std::set<char> Tokenizer::specialCharactersAmongIdentifiers = {'_', '.', '#'};
std::set<std::string> Tokenizer::designEntities = { "stmt", "read", "print", "while", "if", "assign",
            "variable", "constant", "procedure", "prog_line", "call" };
std::set<std::string> Tokenizer::attrNames = { "procName", "varName", "value", "stmt#" };

bool Tokenizer::canTreatAsIdent(TokenTypes type, std::string value) {
    // Keyword TokenTypes which can also be treated as identifiers when accepting tokens
    TokenTypes keywordTypes[] = { TokenTypes::Modifies, TokenTypes::Uses, TokenTypes::Parent,
                TokenTypes::Follows, TokenTypes::Select, TokenTypes::Such, TokenTypes::That, TokenTypes::Pattern,
                TokenTypes::Next, TokenTypes::Calls, TokenTypes::Affects, 
                TokenTypes::And, TokenTypes::With, TokenTypes::Boolean };
    if (std::find(std::begin(keywordTypes), std::end(keywordTypes), type) != std::end(keywordTypes))
        return true;
    // Possibly an identifier with same name as design entity token
    if (type == TokenTypes::DesignEntity && value != "prog_line")
        return true;
    if (type == TokenTypes::AttrName && value != "stmt#")
        return true;
    return false;
}

bool Tokenizer::canTreatAsTermSymbol(TokenTypes type) {
    return type == TokenTypes::TermSymbol || type == TokenTypes::Asterisk;
}

std::string Tokenizer::getAttrRefSynonym(std::string tokenValue)
{
    int periodIndex = tokenValue.find(".");
    std::string synonym = tokenValue.substr(0, periodIndex);
    return synonym;
}

std::string Tokenizer::getAttrRefAttrName(std::string tokenValue)
{
    int periodIndex = tokenValue.find(".");
    std::string attrName = tokenValue.substr(periodIndex + 1);
    return attrName;
}

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
    int periodIndex = identifier.find(".");
    if (periodIndex <= 0 || periodIndex >= identifier.size() - 1) return false;
    std::string synonym = identifier.substr(0, periodIndex);
    std::string attrName = identifier.substr(periodIndex + 1);
    // Synonym is not a valid identifier
    for (int i = 1; i < synonym.size(); i++) {
        if (!std::isalnum(synonym[i])) return false;
    }
    // AttrName is not valid
    if (std::find(std::begin(Tokenizer::attrNames), std::end(Tokenizer::attrNames), identifier) != std::end(Tokenizer::attrNames)) {
        return false;
    }
    return true;
}

std::unique_ptr<Token> Tokenizer::readIdentifier()
{
    std::string identifier = std::string(1, inputStream.next());
    identifier += readWhile(Tokenizer::isAlphanumericOrSpecialChar);
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
    else if (identifier == "Follows") {
        token = std::make_unique<Token>(Token{ TokenTypes::Follows, identifier });
    }
    else if (identifier == "Calls") {
        token = std::make_unique<Token>(Token{ TokenTypes::Calls, identifier });
    }
    else if (identifier == "Next") {
        token = std::make_unique<Token>(Token{ TokenTypes::Next, identifier });
    }
    else if (identifier == "Affects") {
        token = std::make_unique<Token>(Token{ TokenTypes::Affects, identifier });
    }
    else if (identifier == "with") {
        token = std::make_unique<Token>(Token{ TokenTypes::With, identifier });
    }
    else if (identifier == "BOOLEAN") {
        token = std::make_unique<Token>(Token{ TokenTypes::Boolean, identifier });
    }
    else if (std::find(std::begin(Tokenizer::designEntities), std::end(Tokenizer::designEntities), identifier) != std::end(Tokenizer::designEntities)) {
        token = std::make_unique<Token>(Token{ TokenTypes::DesignEntity, identifier });
    }
    else if (std::find(std::begin(Tokenizer::attrNames), std::end(Tokenizer::attrNames), identifier) != std::end(Tokenizer::attrNames)) {
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
    case '*':
        return std::make_unique<Token>(Token{ TokenTypes::Asterisk, std::string(1, inputStream.next()) });
        break;
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
        return std::make_unique<Token>(Token{ TokenTypes::TermSymbol, std::string(1, inputStream.next()) });
        break;
    case '<':
        return std::make_unique<Token>(Token{ TokenTypes::LeftAngleBracket, std::string(1, inputStream.next()) });
        break;
    case '>':
        return std::make_unique<Token>(Token{ TokenTypes::RightAngleBracket, std::string(1, inputStream.next()) });
        break;
    case '=':
        return std::make_unique<Token>(Token{ TokenTypes::Equals, std::string(1, inputStream.next()) });
        break;
    default:
        break;
    }
    if (std::isdigit(ch)) return readInteger();
    if (std::isalpha(ch)) return readIdentifier();
    throw SyntacticException("Cannot handle invalid character: " + ch);
}
