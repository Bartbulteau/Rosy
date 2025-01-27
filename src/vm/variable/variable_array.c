#include "variable_array.h"

void RyVariableArrayInit(RyVariableArray *arr) {
    arr->size = 0;
    arr->capacity = 0;
    arr->entries = NULL;
}

void RyVariableArrayWrite(RyVariableArray *arr, size_t idx, RyObject *o) {
    if(arr->capacity < idx + 1) {
        size_t new_cap = arr->capacity < 8 ? 8 : (arr->capacity * 2);
        arr->capacity = new_cap;
        arr->entries = (RyObject **)REALLOC(arr->entries, new_cap * sizeof(RyObject *));
    }
    if (arr->size < idx + 1) {
        arr->size = idx + 1;
    }
    arr->entries[idx] = o;
}

RyObject *RyVariableArrayGet(RyVariableArray *arr, size_t idx) {
    if (idx >= arr->capacity) {
        printf("Runtime error : RyVariableArrayGet - index out of bounds\n");
        exit(1);
    }
    return arr->entries[idx];
}

void RyVariableArrayFree(RyVariableArray *arr) {
    FREE(arr->entries);
}