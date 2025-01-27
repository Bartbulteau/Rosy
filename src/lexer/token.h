#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <string.h>
#include "../memory_debug/memory_debug.h"
#include "../arena/arena.h"

typedef enum TokenType_t {
    // used for parsing errors
    ERROR_TOK,
    // single character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SLASH, STAR, MOD,
    EOL,

    // on or two characters
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // literals
    IDENTIFIER, STRING, NUMBER, NIL, TRUE, FALSE,

    // keywords
    EXIT,
    PRINT,
    FUNC, RETURN,
    FOR, WHILE, 
    IF, ELSE, 
    OR, AND,
    VAR, GLOBAL,
    
    // end of file
    EOF_,
} TokenType;

typedef struct Token_t {
    TokenType type;
    char *lexeme;
    size_t line;
} Token;

char *substr(Arena *a, char *buffer, size_t start, size_t end);

#define KEYWORD_MAP_SIZE 64     // must be a power of 2 !!! (for fast modulo computation)

typedef struct KeywordMapEntry_t {
    struct KeywordMapEntry_t *next;
    char *lexeme;
    TokenType type;
} KeywordMapEntry;

typedef struct KeywordMap_t {
    Arena a;
    KeywordMapEntry **entries;
} KeywordMap;

void KeywordMapInit(KeywordMap *map);
void KeywordMapFree(KeywordMap *map);
void KeywordMapInsert(KeywordMap *map, char *lexeme, TokenType type);
TokenType KeywordMapGet(KeywordMap *map, char *lexeme);
size_t KeywordMapHash(char *key);

#endif // TOKEN_H