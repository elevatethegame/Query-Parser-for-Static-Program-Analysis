#pragma once

using namespace std;
#include "SIMPLEToken.h"

class ParseError {
public:
    /* Constructs parse error with an error*/
    ParseError(SIMPLEToken token, string errorMessage);

    /* Constructs an empty error*/
    ParseError();

    /* Returns true of this is an error*/
    bool hasError() const;

    /* For combining error*/ 
    ParseError combineWith(const ParseError& other);

    string getErrorMessage() const;

    string getToken() const;
private:
    SIMPLEToken token;
    string errorMessage;
    bool hasErrorValue;
};