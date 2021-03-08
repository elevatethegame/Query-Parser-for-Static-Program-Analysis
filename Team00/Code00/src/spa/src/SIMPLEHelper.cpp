#include "SIMPLEHelper.h"

#include <cassert>
#include <algorithm>

using namespace std;

void SIMPLEHelper::buildStrToTokenType() {
    for (auto s : relSymbols) strToTokenType[s] = TokenType::relSymbol;
    for (auto s : condSymbols) strToTokenType[s] = TokenType::condSymbol;
    for (auto s : exprSymbols) strToTokenType[s] = TokenType::exprSymbol;
    for (auto s : termSymbols) strToTokenType[s] = TokenType::termSymbol;
    for (auto s : braces) strToTokenType[s] = TokenType::brace;
    for (auto s : semicolon) strToTokenType[s] = TokenType::semicolon;
    for (auto s : notSymbols) strToTokenType[s] = TokenType::notSymbol;
}

SIMPLEHelper::SIMPLEHelper() {
    buildStrToTokenType();
}

bool SIMPLEHelper::isBrace(char ch) {
    string chAsStr = string(1, ch);
    return getTokenType(chAsStr) == TokenType::brace;
}

bool SIMPLEHelper::isSemicolon(char ch) {
    string chAsStr = string(1, ch);
    return getTokenType(chAsStr) == TokenType::semicolon;
}

bool SIMPLEHelper::isDigit(char ch) {
    return '0' <= ch && ch <= '9';
}

bool SIMPLEHelper::isInteger(string token) {
    assert(token.size() > 0);
    for (auto &ch : token) {
        if (!isDigit(ch)) return false;
    }

    /// can not have leading 0
    if (token.size() > 1 && token[0] == '0') {
        return false;
    }

    return true;
}

bool SIMPLEHelper::isName(string token) {
    assert(token.size() > 0);
    if (!isAlpha(token[0])) return false;
    for (auto &ch : token) {
        if (!isAlphaNumeric(ch)) return false;
    }
    return true;
}

bool SIMPLEHelper::isKeyword(string token) {
    return find(keywords.begin(), keywords.end(), token) != keywords.end();
}

bool SIMPLEHelper::isAlpha(char ch) {
    if ('a' <= ch && ch <= 'z') return true;
    if ('A' <= ch && ch <= 'Z') return true;
    return false;
}

bool SIMPLEHelper::isAlphaNumeric(char ch) {
    return isDigit(ch) || isAlpha(ch);
}

TokenType SIMPLEHelper::getTokenType(string token) {
    /// safe-guard
    if (token.empty()) {
        return TokenType::other;
    }

    auto ite = strToTokenType.find(token);
    if (ite != strToTokenType.end()) {
        ///pre-defined symbol
        return ite->second;
    }

    if (isInteger(token)) {
        return TokenType::integer;
    }

    if (isName(token)) {
        return TokenType::name;
    }

    return TokenType::other;
}

string SIMPLEHelper::getTokenTypeName(TokenType type) {
    switch (type) {
    case TokenType::notSymbol :
        return "!";
    case TokenType::keyword :
        return "keyword";
    case TokenType::relSymbol :
        return "relSymbol";
    case TokenType::condSymbol :
        return "condSymbol";
    case TokenType::exprSymbol :
        return "exprSymbol";
    case TokenType::termSymbol :
        return "termSymbol";
    case TokenType::name :
        return "name";
    case TokenType::integer :
        return "integer";
    case TokenType::brace :
        return "brace";
    case TokenType::semicolon :
        return "semicolon";
    case TokenType::other :
        return "other";
    };
}

bool SIMPLEHelper::isSeparator(char ch) {
    for(auto i : separators) {
        if (i == ch) return true;
    }
    return false;
}



