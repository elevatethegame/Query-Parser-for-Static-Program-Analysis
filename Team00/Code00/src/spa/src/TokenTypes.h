#pragma once
#ifndef TOKENTYPES_H
#define TOKENTYPES_H

enum class TokenTypes
{
    Integer,
    Identifier,
    DesignEntity,
    Modifies,
    Uses,
    Parent,
    ParentT,
    Follows,
    FollowsT,
    Calls,
    CallsT,
    Next,
    NextT,
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