#ifndef CONSTANT_MAP_H
#define CONSTANT_MAP_H

#include "../lexer/token.h"
#include "../vm/vm.h"

#define CONSTANT_MAP_SIZE 512

typedef struct ConstantMapEntry_t {
    struct ConstantMapEntry_t* next;
    Token t;
    size_t addr;
} ConstantMapEntry;

typedef struct ConstantMap_t {
    Arena a;
    ConstantMapEntry **entries;
    size_t count;
} ConstantMap;

void ConstantMapInit(ConstantMap *map);
void ConstantMapFree(ConstantMap *map);
size_t ConstantMapInsert(ConstantMap *map, RyVM *vm, Token t);
size_t ConstantMapHash(char *key);

#endif // CONSTANT_MAP_H