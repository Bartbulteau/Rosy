#ifndef PARSER_H
#define PARSER_H

#include <stdarg.h>
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "../vm/vm.h"
#include "../vm/object/operation.h"
#include "constant_map.h"
#include "identifier_map.h"

typedef struct Parser_t {
    Lexer *lex;
    RyVM *vm;

    ConstantMap cmap;

    IdentifierMap global_map;
    IdentifierMap main_local_map;
    IdentifierMap local_map;

    Token previous;
    Token current;
    Token next;

    bool isInMain;
} Parser;

void ParserInit(Parser *p, Lexer *lex, RyVM *vm);
void ParserFree(Parser *p);

Token parser_error(Token t, const char *msg);

bool p_isAtEnd(Parser *p);
Token p_peek(Parser *p);
Token p_previous(Parser *p);
Token p_advance(Parser *p);
bool p_check(Parser *p, TokenType type);
bool p_checkNext(Parser *p, TokenType type);
Token p_consume(Parser *p, TokenType type, char *msg);
bool p_match(Parser *p, size_t ntypes, ...);

void execute(Parser *p);

void ParseFunctionDecl(Parser *p);
void ParseFunction(Parser *p);
void ParseBlock(Parser *p);
void ParseGeneralDeclaration(Parser *p);
void ParseDeclaration(Parser *p);
void ParseVarDecl(Parser *p);
void ParseGlobalVarDecl(Parser *p);
void ParseStatement(Parser *p);
void ParseExpression(Parser *p);
void ParseExitStatement(Parser *p);
void ParsePrintStatement(Parser *p);
void ParseReturnStatement(Parser *p);
void ParseExpressionStmt(Parser *p);
void ParseAssignment(Parser *p);
void ParseLogicOr(Parser *p);
void ParseLogicAnd(Parser *p);
void ParseEquality(Parser *p);
void ParseComparison(Parser *p);
void ParseTerm(Parser *p);
void ParseFactor(Parser *p);
void ParseUnary(Parser *p);
void ParseCall(Parser *p);
void ParsePrimary(Parser *p);
void ParseIfStatement(Parser *p);
void ParseWhileStatement(Parser *p);

#endif // PARSER_H