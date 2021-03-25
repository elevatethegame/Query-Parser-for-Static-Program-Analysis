#pragma once
using namespace std;

#include "SimpleParseError.h"
#include "SIMPLEToken.h"
#include "SIMPLETokenStream.h"
#include "PKB.h"
#include "DesignExtractor.h"

enum ParseResult {
    EXPRESSION,
    CONDITION_N0,
    CONDITION_N1,
    NONERESULT,
    FAILED
};

ParseResult combineResult(ParseResult first, ParseResult second, SIMPLEToken operand);

class Parser {
public:
    /**
    * Constructor for Parser
    */
    Parser(DesignExtractor& extractor);


    /**
     * Parses a Token Stream and adds information to design extractor
     *
     *
     * returns UnexpectedToken error only, return the position
     * of the token
     */
    ParseError parseExpression(SIMPLETokenStream& stream, Expression& result, int userStatement);

    ParseError parseConditionExpression(SIMPLETokenStream& stream, int userStatement);

    ParseError parseProcedure(SIMPLETokenStream& stream);

    ParseError parseReadStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseProgram(SIMPLETokenStream &stream);
private:
    //Design
    DesignExtractor& designExtractor;

    //number of statement to be incremented
    int numberOfStatements;

    //current procName
    string currentProcName;

    //set of all variables
    set<string> variables;

    //set of all literals
    set<string> literals;

    //set of all procedures
    set<string> procedures;
    
    ///range of procedures
    map<string, pair<int, int> > rangeProcedure;

    ///all calling relationships.
    unordered_map<string, vector<string> > callingRelationships;

    void addStatementCall(int statementId, string callee);

    void addCallingRelationship(int, string callee);

    //set the first statement index of the procName
    void startAProcedure(string procName);

    //set the last statement index of the procName
    void endAProcedure(string procName);

    ///add one literals
    void addLiterals(string literals);

    //add a new variable to the table
    void addVariable(string variableName);

    //add to DesignExtractor
    void addModifies(int statement, const string& variable);

    //add to DesignExtractor    
    void addUses(int statement, const string& variable);

    //add to DesignExtractor
    void addParentRelationship(int parent, int child);

    //add expression
    void addExpression(int statement, Expression expression);

    void addFollowRelationship(int before, int after);

    //increment number of statement
    int startNewStatement(int parentStatement, EntityType type);

    //return the next statement's index
    int expectNextStatementIndex() {
        return numberOfStatements + 1;
    }

    ParseError parseAssignmentStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseWhileStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseIfStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseStatementList(SIMPLETokenStream& stream, int parentStatementIndex = -1);

    ParseError parseRelationExpression(SIMPLETokenStream& stream, int userStatement);

    ParseError parsePrintStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseCallStatement(SIMPLETokenStream &stream, int parentStatementIndex);

    ParseError parseFactor(SIMPLETokenStream &stream, Expression& result, int userStatement);

    ParseError parseTerm(SIMPLETokenStream& stream, Expression& result, int userStatement);

    ParseError parseRelationFactor(SIMPLETokenStream& stream, int userStatement);

    ParseError evaluateConditionOrExpression(int level, SIMPLETokenStream& stream, ParseResult& result, ParseResult currentResult, int userStatement);
};