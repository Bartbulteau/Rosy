#include "identifier_map.h"

void IdentifierMapInit(IdentifierMap *map) {
    ArenaInit(&map->a, ARENA_KB(8));
    map->entries = (IdentifierMapEntry **)ArenaAlloc(&map->a, sizeof(IdentifierMapEntry *) * IDENTIFIER_MAP_SIZE);
    map->count = 0;
}

void IdentifierMapFree(IdentifierMap *map) {
    ArenaFree(&map->a);
}

bool IdentifierMapContains(IdentifierMap *map, char *name) {
    size_t hash = IdentifierMapHash(name) % IDENTIFIER_MAP_SIZE;
    for(IdentifierMapEntry *entry = map->entries[hash]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) return true;
    }
    return false;
}

size_t IdentifierMapGet(IdentifierMap *map, char *name) {
    size_t hash = IdentifierMapHash(name) % IDENTIFIER_MAP_SIZE;
    for(IdentifierMapEntry *entry = map->entries[hash]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) return entry->addr;
    }
    return SIZE_MAX;
}

size_t IdentifierMapInsertUnique(IdentifierMap *map, char *name) {
    size_t hash = IdentifierMapHash(name) % IDENTIFIER_MAP_SIZE;
    for(IdentifierMapEntry *entry = map->entries[hash]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) return SIZE_MAX;
    }
    IdentifierMapEntry *entry = (IdentifierMapEntry *)ArenaAlloc(&map->a, sizeof(IdentifierMapEntry));
    entry->name = name;
    entry->addr = map->count;
    entry->next = map->entries[hash];
    map->entries[hash] = entry;
    return map->count++;
}

size_t IdentifierMapHash(char *key) {
    size_t hash = 5381;
    size_t len = strlen(key);
    while (len--) {
        hash += (hash << 5);
        hash ^= *key++;
    }
    return hash;
}