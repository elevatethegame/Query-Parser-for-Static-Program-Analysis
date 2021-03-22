#ifndef __SIMPLETOKEN__H__
#define __SIMPLETOKEN__H__

#include <vector>
#include <string>
#include <exception>

#include "SIMPLEHelper.h"

/**
    Represent a lexical token, store information of where it actually is in the text as well.
    Guarantee that the value is of correct type
**/
class SIMPLEToken {
private:
    static SIMPLEHelper helper;

    string value;
    int row;
    int column;
    TokenType type;


public:
    SIMPLEToken(string value, int row, int column);
    SIMPLEToken();
    string getValue();
    int getRow();
    int getColumn();

    TokenType getTokenType();

    bool operator == (const SIMPLEToken& oth) const;

    string toString() const;
};

#endif  //!__SIMPLETOKEN__H__