#ifndef __SIMPLEHELPER__H__
#define __SIMPLEHELPER__H__



#include <vector>
#include <string>
#include <map>

using namespace std;

enum class TokenType {
    keyword = 10, ///deprecated since variable name can also be the same
    relSymbol = 3,
    notSymbol = 2,
    condSymbol = 1,
    exprSymbol = 4,
    termSymbol = 5,
    name = 7,
    integer = 8,
    brace = 6,
    semicolon = 9,
    other = 11 /// in current implementation mean that something is wrong
};

class SIMPLEHelper {
private:
    const vector<string> relSymbols = {">", ">=", "<", "<=", "==", "!="};
    const vector<string> condSymbols = {"&&", "||"};
    const vector<string> notSymbols = {"!"};

    /// Take note of the priority and possible expression
    const vector<string> exprSymbols = {"+", "-"};
    const vector<string> termSymbols = {"*", "/", "%"};
    const vector<string> braces = {"{", "}", "(", ")"};
    const vector<string> semicolon = {";"};

    const vector<string> keywords = {
        "procedure",
        "read",
        "print",
        "call",
        "while",
        "if",
        "assign",
    };

    map<string, TokenType> strToTokenType;
    void buildStrToTokenType();

public:
    SIMPLEHelper();

    bool isBrace(char ch);
    bool isSemicolon(char ch);
    TokenType getTokenType(string token);



    /** There are library implementation but system dependence **/
    bool isDigit(char ch);
    bool isAlpha(char ch);
    bool isAlphaNumeric(char ch);

    bool isInteger(string token);
    bool isName(string token);
    bool isKeyword(string token);

    string getTokenTypeName(TokenType type);
};

#endif  //!__SIMPLEHELPER__H__

