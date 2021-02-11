#include "Token.h"
#include <unordered_map>

std::string Token::TokenTypeToString(TokenTypes type)
{
    switch (type)
    {
    case TokenTypes::Asterisk:
        return "Token Type of Asterisk";
        break;
    case TokenTypes::Comma:
        return "Token Type of Comma";
        break;
    case TokenTypes::DesignEntity:
        return "Token Type of DesignEntity";
        break;
    case TokenTypes::DoubleQuote:
        return "Token Type of DoubleQuote";
        break;
    case TokenTypes::Follows:
        return "Token Type of Follows";
        break;
    case TokenTypes::Identifier:
        return "Token Type of Identifier";
        break;
    case TokenTypes::Integer:
        return "Token Type of Integer";
        break;
    case TokenTypes::LeftParen:
        return "Token Type of LeftParen";
        break;
    case TokenTypes::Modifies:
        return "Token Type of Modifies";
        break;
    case TokenTypes::Parent:
        return "Token Type of Parent";
        break;
    case TokenTypes::Pattern:
        return "Token Type of Pattern";
        break;
    case TokenTypes::RightParen:
        return "Token Type of RightParen";
        break;
    case TokenTypes::Select:
        return "Token Type of Select";
        break;
    case TokenTypes::Semicolon:
        return "Token Type of Semicolon";
        break;
    case TokenTypes::Such:
        return "Token Type of Such";
        break;
    case TokenTypes::That:
        return "Token Type of That";
        break;
    case TokenTypes::Underscore:
        return "Token Type of Underscore";
        break;
    default:  // case TokenTypes::Uses
        return "Token Type of Uses";
        break;
    }
}

std::string Token::EntityTypeToString(EntityType type)
{
    switch (type)
    {
    case EntityType::Assign:
        return "Entity Type of Assign";
        break;
    case EntityType::Call:
        return "Entity Type of Call";
        break;
    case EntityType::Constant:
        return "Entity Type of Constant";
        break;
    case EntityType::If:
        return "Entity Type of If";
        break;
    case EntityType::Print:
        return "Entity Type of Print";
        break;
    case EntityType::Procedure:
        return "Entity Type of Procedure";
        break;
    case EntityType::Read:
        return "Entity Type of Read";
        break;
    case EntityType::Stmt:
        return "Entity Type of Stmt";
        break;
    case EntityType::Variable:
        return "Entity Type of Variable";
        break;
    case EntityType::While:
        return "Entity Type of While";
        break;
    default:  // case EntityType::Null
        return "Entity Type of Null";
        break;
    }
}

Token::Token(TokenTypes tokenType, std::string tokenValue)
{
    type = tokenType;
    value = tokenValue;
}

TokenTypes Token::getType()
{
    return type;
}

std::string Token::getValue()
{
    return value;
}

std::string Token::toString()
{
    switch (type)
    {
    case TokenTypes::Asterisk:
        return "Token Type of Asterisk with value: " + value;
        break;
    case TokenTypes::Comma:
        return "Token Type of Comma with value: " + value;
        break;
    case TokenTypes::DesignEntity:
        return "Token Type of DesignEntity with value: " + value;
        break;
    case TokenTypes::DoubleQuote:
        return "Token Type of DoubleQuote with value: " + value;
        break;
    case TokenTypes::Follows:
        return "Token Type of Follows with value: " + value;
        break;
    case TokenTypes::Identifier:
        return "Token Type of Identifier with value: " + value;
        break;
    case TokenTypes::Integer:
        return "Token Type of Integer with value: " + value;
        break;
    case TokenTypes::LeftParen:
        return "Token Type of LeftParen with value: " + value;
        break;
    case TokenTypes::Modifies:
        return "Token Type of Modifies with value: " + value;
        break;
    case TokenTypes::Parent:
        return "Token Type of Parent with value: " + value;
        break;
    case TokenTypes::Pattern:
        return "Token Type of Pattern with value: " + value;
        break;
    case TokenTypes::RightParen:
        return "Token Type of RightParen with value: " + value;
        break;
    case TokenTypes::Select:
        return "Token Type of Select with value: " + value;
        break;
    case TokenTypes::Semicolon:
        return "Token Type of Semicolon with value: " + value;
        break;
    case TokenTypes::Such:
        return "Token Type of Such with value: " + value;
        break;
    case TokenTypes::That:
        return "Token Type of That with value: " + value;
        break;
    case TokenTypes::Underscore:
        return "Token Type of Underscore with value: " + value;
        break;
    default:  // case TokenTypes::Uses
        return "Token Type of Uses with value: " + value;
        break;
    }
}

EntityType Token::getEntityType() {
    if (type != TokenTypes::DesignEntity) return EntityType::Null;
    std::unordered_map<std::string, EntityType> stringToCase = { {"stmt", EntityType::Stmt}, {"read", EntityType::Read},
        {"print", EntityType::Print}, {"while", EntityType::While}, {"if", EntityType::If}, {"assign", EntityType::Assign},
        {"variable", EntityType::Variable}, {"constant", EntityType::Constant}, {"procedure", EntityType::Procedure} };
    return stringToCase[value];
}