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

typedef struct RyString_t {
    RyObject base;
    char *str;
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
void RyObjectPoolMark(RyValue val);

RyValue RyObjectPoolBorrow_String(RyObjectPool *pool);
RyValue RyObjectPoolBorrow_Function(RyObjectPool *pool);

#define VAL2NUM(val) (val).as.number
#define NUM2VAL(num) (RyValue){NUMBER_OBJ, {.number = num}}

#define VAL2BOOL(val) (val).as.boolean
#define BOOL2VAL(b) (RyValue){BOOL_OBJ, {.boolean = b}}

#define VAL2STRING(val) ((RyString *)(val).as.object)
#define VAL2FUNC(val) ((RyFunction *)(val).as.object)

#endif // OBJECT_POOL_H