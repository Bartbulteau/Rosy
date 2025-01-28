#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdbool.h>

typedef enum RyValueType_t {
    UNINITIALIZED_OBJ,
    NIL_OBJ,
    NUMBER_OBJ,
    BOOL_OBJ,
    STRING_OBJ,
    FUNCTION_OBJ,
    NUM_OF_OBJ_TYPE,
} RyValueType;

extern char RyValueTypeNames[NUM_OF_OBJ_TYPE][100];

typedef struct RyObject_t {
    struct RyObject_t *next_allocated;
    RyValueType type;
    size_t refc;
    size_t size;
} RyObject;

typedef struct RyValue_t {
    RyValueType type;
    union {
        bool boolean;
        double number;
        RyObject *object;
    } as;
} RyValue;

#endif // OBJECT_H