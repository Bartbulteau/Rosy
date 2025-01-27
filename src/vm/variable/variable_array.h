#ifndef VARIABLE_ARRAY_H
#define VARIABLE_ARRAY_H

#include "../../memory_debug/memory_debug.h"
#include "../object/object.h"

typedef struct RyVariableArray_t {
    size_t capacity;
    size_t size;
    RyObject **entries;
} RyVariableArray;

void RyVariableArrayInit(RyVariableArray *arr);
void RyVariableArrayWrite(RyVariableArray *arr, size_t idx, RyObject *o);
RyObject *RyVariableArrayGet(RyVariableArray *arr, size_t idx);
void RyVariableArrayFree(RyVariableArray *arr);

#endif // VARIABLE_ARRAY_H