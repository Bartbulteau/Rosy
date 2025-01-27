#include "token.h"

char *substr(Arena *a, char *buffer, size_t start, size_t end) {
    size_t len = end - start;
    assert(len >= 0);

    char *sub = (char *)ArenaAlloc(a, len);
    strncpy(sub, buffer + start, len);
    sub[len] = '\0';
    return sub;
}

void KeywordMapInit(KeywordMap *map) {
    ArenaInit(&map->a, ARENA_KB(8));
    map->entries = (KeywordMapEntry **)ArenaAlloc(&map->a, sizeof(KeywordMapEntry *) * KEYWORD_MAP_SIZE);

    KeywordMapInsert(map, "nil", NIL);
    KeywordMapInsert(map, "true", TRUE);
    KeywordMapInsert(map, "false", FALSE);

    KeywordMapInsert(map, "exit", EXIT);
    KeywordMapInsert(map, "print", PRINT);
    KeywordMapInsert(map, "fn", FUNC);
    KeywordMapInsert(map, "return", RETURN);
    KeywordMapInsert(map, "for", FOR);
    KeywordMapInsert(map, "while", WHILE);
    KeywordMapInsert(map, "if", IF);
    KeywordMapInsert(map, "else", ELSE);
    KeywordMapInsert(map, "or", OR);
    KeywordMapInsert(map, "and", AND);
    KeywordMapInsert(map, "var", VAR);
    KeywordMapInsert(map, "global", GLOBAL);
}

void KeywordMapFree(KeywordMap *map) {
    ArenaFree(&map->a);
}

void KeywordMapInsert(KeywordMap *map, char *lexeme, TokenType type) {
    size_t hash = KeywordMapHash(lexeme) % KEYWORD_MAP_SIZE;
    KeywordMapEntry *entry = (KeywordMapEntry *)ArenaAlloc(&map->a, sizeof(KeywordMapEntry));
    entry->lexeme = (char *)ArenaAlloc(&map->a, strlen(lexeme) + 1);
    strcpy(entry->lexeme, lexeme);
    entry->type = type;
    entry->next = map->entries[hash];
    map->entries[hash] = entry;
}

TokenType KeywordMapGet(KeywordMap *map, char *lexeme) {
    size_t hash = KeywordMapHash(lexeme) % KEYWORD_MAP_SIZE;

    for (KeywordMapEntry *entry = map->entries[hash]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->lexeme, lexeme) == 0) return entry->type;
    }

    return IDENTIFIER;
}

size_t KeywordMapHash(char *key) {
    size_t hash = 5381;
    size_t len = strlen(key);
    while (len--) {
        hash += (hash << 5);
        hash ^= *key++;
    }
    return hash;
}