#include "lexer.h"

void LexerInit(Lexer *lex, char *buffer, size_t buffer_len) {
    ArenaInit(&lex->a, ARENA_KB(8));
    lex->buffer = buffer;
    lex->buffer_len = buffer_len;
    lex->start = 0;
    lex->current = 0;
    lex->line = 1;
    lex->isDone = false;
    KeywordMapInit(&lex->kmap);
}

void LexerFree(Lexer *lex) {
    ArenaFree(&lex->a);
    KeywordMapFree(&lex->kmap);
}

bool l_isAtEnd(Lexer *lex) {
    return lex->current >= lex->buffer_len;
}

char l_advance(Lexer *lex) {
    return lex->buffer[lex->current++];
}

bool l_match(Lexer *lex, char expected) {
    if (l_isAtEnd(lex)) return false;
    if (lex->buffer[lex->current] != expected) return false;
    lex->current++;
    return true;
}

char l_peek(Lexer *lex) {
    if (l_isAtEnd(lex)) return '\0';
    return lex->buffer[lex->current];
}

char l_peekNext(Lexer *lex) {
    if (lex->current + 1 >= lex->buffer_len) return '\0';
    return lex->buffer[lex->current + 1];
}

bool isDigit(char c) {
    return '0' <= c && c <= '9'; 
}

bool isAlpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool isAlphaNumeric(char c) {
    return isDigit(c) || isAlpha(c);
}

Token LexerString(Lexer *lex) {
    while (l_peek(lex) != '"' && !l_isAtEnd(lex)) {
        if (l_peek(lex) == '\n') {
            printf("Lexer error: unterminated string at line %zu\n", lex->line);
            exit(1);
        }
        l_advance(lex);
    }

    if(l_isAtEnd(lex)) {
        printf("Lexer error: unterminated string.\n");
        exit(1);
    }

    l_advance(lex);

    char *str = substr(&lex->a, lex->buffer, lex->start + 1, lex->current - 1);

    size_t i = 0;
    bool spe_mode = false;
    for (size_t j = 0; j < lex->current - lex->start - 2; j++) {
        if (!spe_mode) {
            if (str[j] == '\\') spe_mode = true;
            else str[i++] = str[j];
        } else {
            if (str[j] == '\\') {
                str[i++] = '\\';
            } else if (str[j] == 'n') {
                str[i++] = '\n';
            } else if (str[j] == 't') {
                str[i++] = '\t';
            } else if (str[j] == 'r') {
                str[i++] = '\r';
            } else if (str[j] == '"') {
                str[i++] = '"';
            } else {
                str[i++] = '\\';
                str[i++] = str[j];
            }
            spe_mode = false;
        }
    }

    while (i < lex->current - lex->start - 2) {
        str[i++] = '\0';
    }

    Token t = {
        .lexeme = str,
        .line = lex->line,
        .type = STRING
    };
    return t;
}

Token LexerNumber(Lexer *lex) {

    while (isDigit(l_peek(lex))) l_advance(lex);

    if(l_peek(lex) == '.' && isDigit(l_peekNext(lex))) {
        l_advance(lex);
        while(isDigit(l_peek(lex))) l_advance(lex);
    }

    char *nbr = substr(&lex->a, lex->buffer, lex->start, lex->current);
    Token t = {
        .lexeme = nbr,
        .line = lex->line,
        .type = NUMBER
    };

    return t;
}

Token LexerIdentifier(Lexer *lex) {
    while(isAlphaNumeric(l_peek(lex))) l_advance(lex);

    char *name = substr(&lex->a, lex->buffer, lex->start, lex->current);
    
    Token t = {
        .lexeme = name,
        .line = lex->line,
        .type = KeywordMapGet(&lex->kmap, name)
    };

    return t;
}

Token LexerGetNextToken(Lexer *lex) {
    Token t = {0};
    
    while (!l_isAtEnd(lex)) {
        lex->start = lex->current;
        char c = l_advance(lex);

        switch (c) {

            case '(': t.type = LEFT_PAREN; t.line = lex->line; return t;
            case ')': t.type = RIGHT_PAREN; t.line = lex->line; return t;
            case '{': t.type = LEFT_BRACE; t.line = lex->line; return t;
            case '}': t.type = RIGHT_BRACE; t.line = lex->line; return t;
            case ',': t.type = COMMA; t.line = lex->line; return t;
            case ';': t.type = EOL; t.line = lex->line; return t;
            case '.': t.type = DOT; t.line = lex->line; return t;
            case '-': t.type = MINUS; t.line = lex->line; return t;
            case '+': t.type = PLUS; t.line = lex->line; return t;
            case '*': t.type = STAR; t.line = lex->line; return t;
            case '/': t.type = SLASH; t.line = lex->line; return t;
            case '%': t.type = MOD; t.line = lex->line; return t;

            case '!': t.type = l_match(lex, '=') ? BANG_EQUAL : BANG; t.line = lex->line; return t;
            case '=': t.type = l_match(lex, '=') ? EQUAL_EQUAL : EQUAL; t.line = lex->line; return t;
            case '<': t.type = l_match(lex, '=') ? LESS_EQUAL : LESS; t.line = lex->line; return t;
            case '>': t.type = l_match(lex, '=') ? GREATER_EQUAL : GREATER; t.line = lex->line; return t;

            case '#': while(l_peek(lex) != '\n' && !l_isAtEnd(lex)) {l_advance(lex);} break;

            case ' ':
            case '\r':
            case '\t': break;
            case '\n': lex->line++; t.type = EOL; t.line = lex->line; return t;

            case '"': return LexerString(lex);

            case '\0': t.type = EOL; t.line = lex->line; return t;

            default:
                if (isDigit(c)) {
                    return LexerNumber(lex);
                } else if (isAlpha(c)) {
                    return LexerIdentifier(lex);
                } else {
                    printf("Lexer error: unexpected character '%c' at line %zu.\n", c, lex->line);
                    exit(1);
                }
                break;
        }

    }

    if (!lex->isDone){
        t.lexeme = NULL;
        t.line = lex->line;
        t.type = EOL;
        lex->isDone = true;
        return t;
    }

    t.lexeme = NULL;
    t.line = lex->line;
    t.type = EOF_;

    return t;
}