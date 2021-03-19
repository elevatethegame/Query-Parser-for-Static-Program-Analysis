#ifndef __SIMPLEPARSEERROR__H__
#define __SIMPLEPARSEERROR__H__


#include "SimpleParseError.h"

using namespace std;
#include "SIMPLEToken.h"

ParseError::ParseError(
    const SIMPLEToken token,
    string errorMessage) : token(token), errorMessage(errorMessage) {
    hasErrorValue = true;
}

ParseError::ParseError() {
    hasErrorValue = false;
}

bool ParseError::hasError() const {
    return this->hasErrorValue;
}

ParseError ParseError::combineWith(const ParseError& other) {
    if (this->hasError()) {
        return *this;
    }
    else return other;
}

string ParseError::getErrorMessage() const {
    return this->errorMessage;
}

string ParseError::getToken() const {
    return token.toString();
}
#endif  //!__SIMPLEPARSEERROR__H__