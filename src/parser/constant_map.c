#include "constant_map.h"

void ConstantMapInit(ConstantMap *map) {
    ArenaInit(&map->a, ARENA_KB(8));
    map->count = 1;
    map->entries = (ConstantMapEntry **)ArenaAlloc(&map->a, sizeof(ConstantMapEntry *) * CONSTANT_MAP_SIZE);
    size_t hash = ConstantMapHash("nil") % CONSTANT_MAP_SIZE;
    map->entries[hash] = (ConstantMapEntry *)ArenaAlloc(&map->a, sizeof(ConstantMapEntry));
    map->entries[hash]->addr = 0;
    map->entries[hash]->t.type = NIL;
    map->entries[hash]->t.line = 0;
    map->entries[hash]->t.lexeme = (char *)ArenaAlloc(&map->a, 4);
    strcpy(map->entries[hash]->t.lexeme, "nil");
    map->entries[hash]->next = NULL;
}

void ConstantMapFree(ConstantMap *map) {
    ArenaFree(&map->a);
}

size_t ConstantMapInsert(ConstantMap *map, RyVM *vm, Token t) {
    size_t hash = ConstantMapHash(t.lexeme) % CONSTANT_MAP_SIZE;
    
    for (ConstantMapEntry *entry = map->entries[hash]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->t.lexeme, t.lexeme) == 0 && entry->t.type == t.type) {
            return entry->addr;
        }
    }

    ConstantMapEntry *entry = (ConstantMapEntry *)ArenaAlloc(&map->a, sizeof(ConstantMapEntry));
    entry->t = t;
    entry->next = map->entries[hash];
    map->entries[hash] = entry;
    entry->addr = map->count;

    RyObject *o;

    if (t.type == NUMBER) {
        RyNumber *num = RyObjectPoolBorrow_Number(&vm->object_pool);
        num->value = atof(t.lexeme);
        o = (RyObject *)num;
    } else if (t.type == STRING) {
        RyString *str = RyObjectPoolBorrow_String(&vm->object_pool);
        str->value = (char *)MALLOC(strlen(t.lexeme) + 1);
        strcpy(str->value, t.lexeme);
        o = (RyObject *)str;
    } else if (t.type == NIL) {
        o = (RyObject *)RyObjectPoolBorrow_Nil(&vm->object_pool);
    } else if (t.type == TRUE) {
        RyBool *b = RyObjectPoolBorrow_Bool(&vm->object_pool);
        b->value = true;
        o = (RyObject *)b;
    } else if (t.type == FALSE) {
        RyBool *b = RyObjectPoolBorrow_Bool(&vm->object_pool);
        b->value = false;
        o = (RyObject *)b;
    } else {
        printf("ConstantMap error: invalid token type to insert in constant map.\n");
        exit(1);
    }

    RyVariableArrayWrite(&vm->constants, map->count, o);

    return map->count++;
}

size_t ConstantMapHash(char *key) {
    size_t hash = 5381;
    size_t len = strlen(key);
    while (len--) {
        hash += (hash << 5);
        hash ^= *key++;
    }
    return hash;
}