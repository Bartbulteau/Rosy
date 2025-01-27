#ifndef INDENTIFIER_MAP_H
#define INDENTIFIER_MAP_H

#include "../arena/arena.h"

#define IDENTIFIER_MAP_SIZE 512

typedef struct IdentifierMapEntry_t {
    struct IdentifierMapEntry_t *next;
    char *name;
    size_t addr;
} IdentifierMapEntry;

typedef struct IdentifierMap_t {
    Arena a;
    IdentifierMapEntry **entries;
    size_t count;
} IdentifierMap;

void IdentifierMapInit(IdentifierMap *map);
void IdentifierMapFree(IdentifierMap *map);
bool IdentifierMapContains(IdentifierMap *map, char *name);
size_t IdentifierMapGet(IdentifierMap *map, char *name);
size_t IdentifierMapInsertUnique(IdentifierMap *map, char *name);

size_t IdentifierMapHash(char *key);

#endif // INDENTIFIER_MAP_H