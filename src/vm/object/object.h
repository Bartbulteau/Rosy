#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdbool.h>

typedef enum RyObjectType_t {
    UNINITIALIZED_OBJ,
    NIL_OBJ,
    NUMBER_OBJ,
    BOOL_OBJ,
    STRING_OBJ,
    FUNCTION_OBJ,
    NUM_OF_OBJ_TYPE,
} RyObjectType;

static char RyObjectTypeNames[NUM_OF_OBJ_TYPE][100] = {
    "unintialized_obj",
    "nil_obj",
    "number_obj",
    "bool_obj",
    "string_obj",
    "function_obj"
};

typedef struct RyObject_t {
    struct RyObject_t *next;
    struct RyObject_t *next_allocated;
    RyObjectType type;
    size_t refc;
    size_t size;
} RyObject;

#endif // OBJECT_H