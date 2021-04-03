#pragma once
#ifndef TOKENTYPES_H
#define TOKENTYPES_H

enum class TokenTypes
{
    Asterisk,
    Integer,
    Identifier,
    DesignEntity,
    Modifies,
    Uses,
    Parent,
    Follows,
    Calls,
    Next,
    Affects,
    Select,
    Such,
    That,
    Pattern,
    And,
    Semicolon,
    Underscore,
    LeftParen,
    RightParen,
    Comma,
    DoubleQuote,
    ExprSymbol,
    TermSymbol,
    LeftAngleBracket,
    RightAngleBracket,
    AttrName,
    AttrRef,
    Equals,
    With,
    Boolean
};

#endif