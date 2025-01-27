#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include "object.h"
#include "../../arena/arena.h"
#include "../code/code_block.h"
#include "../variable/variable_array.h"

#define GC_HEAP_GROWTH_FACTOR 2

typedef struct RyObjectPool_t {
    RyObject *allocated_list;
    size_t obj_count;
    size_t bytes_allocated;
    size_t next_gc;
} RyObjectPool;

typedef struct RyNil_t {
    RyObject base;
} RyNil;

typedef struct RyNumber_t {
    RyObject base;
    double value;
} RyNumber;

typedef struct RyBool_t {
    RyObject base;
    bool value;
} RyBool;

typedef struct RyString_t {
    RyObject base;
    char *value;
} RyString;

typedef struct RyFunction_t {
    RyObject base;
    RyCodeBlock block;
    RyVariableArray locals;
    size_t ip;
} RyFunction;

void RyObjectPoolInit(RyObjectPool *pool);
void RyObjectPoolFree(RyObjectPool *pool);
RyObject *RyObjectPoolBorrow(RyObjectPool *pool, size_t size);
void RyObjectPoolRelease(RyObjectPool *pool, RyObject *o);

RyNil *RyObjectPoolBorrow_Nil(RyObjectPool *pool);
RyNumber *RyObjectPoolBorrow_Number(RyObjectPool *pool);
RyBool *RyObjectPoolBorrow_Bool(RyObjectPool *pool);
RyString *RyObjectPoolBorrow_String(RyObjectPool *pool);
RyFunction *RyObjectPoolBorrow_Function(RyObjectPool *pool);

#endif // OBJECT_POOL_H