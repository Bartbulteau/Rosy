#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "../memory_debug/memory_debug.h"
#include "../arena/arena.h"
#include "token.h"

typedef struct Lexer_t {
    Arena a;

    char *buffer;
    size_t buffer_len;
    size_t start;
    size_t current;
    size_t line;

    bool isDone;

    KeywordMap kmap;
} Lexer;

void LexerInit(Lexer *lex, char *buffer, size_t buffer_len);
void LexerFree(Lexer *lex);

bool l_isAtEnd(Lexer *lex);
char l_advance(Lexer *lex);
bool l_match(Lexer *lex, char expected);
char l_peek(Lexer *lex);
char l_peekNext(Lexer *lex);

bool isDigit(char c);
bool isAlpha(char c);
bool isAlphaNumeric(char c);

Token LexerString(Lexer *lex);
Token LexerNumber(Lexer *lex);
Token LexerIdentifier(Lexer *lex);
Token LexerGetNextToken(Lexer *lex);

#endif // LEXER_H