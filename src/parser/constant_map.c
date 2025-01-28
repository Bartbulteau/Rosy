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

    RyValue val;

    if (t.type == NUMBER) {
        val = NUM2VAL(atof(t.lexeme));
    } else if (t.type == STRING) {
        val = RyObjectPoolBorrow_String(&vm->object_pool);
        char *str = (char *)MALLOC(strlen(t.lexeme) + 1);
        strcpy(str, t.lexeme);
        ((RyString *)val.as.object)->str = str;
    } else if (t.type == NIL) {
        val.type = NIL_OBJ;
    } else if (t.type == TRUE) {
        val.type = BOOL_OBJ;
        val.as.boolean = true;
    } else if (t.type == FALSE) {
        val.type = BOOL_OBJ;
        val.as.boolean = false;
    } else {
        printf("ConstantMap error: invalid token type to insert in constant map.\n");
        exit(1);
    }

    RyVariableArrayWrite(&vm->constants, map->count, val);

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