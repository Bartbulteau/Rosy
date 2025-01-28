#include "object_pool.h"

void RyObjectPoolInit(RyObjectPool *pool) {
    pool->allocated_list = NULL;
    pool->obj_count = 0;
    pool->bytes_allocated = 0;
    pool->next_gc = 2048;
}

void RyObjectPoolFree(RyObjectPool *pool) {
    RyObject *prev = NULL;
    RyObject *curr = pool->allocated_list;
    while (curr != NULL) {
        prev = curr;
        curr = curr->next_allocated;
        RyObjectPoolRelease(pool, prev);
    }
}

RyObject *RyObjectPoolBorrow(RyObjectPool *pool, size_t size) {
    RyObject *o = (RyObject *)MALLOC(size);
    o->next_allocated = pool->allocated_list;
    pool->allocated_list = o;
    o->refc = 0;
    o->type = UNINITIALIZED_OBJ;
    o->size = size;

    pool->obj_count++;
    pool->bytes_allocated += size;
    
    return o;
}

void RyObjectPoolRelease(RyObjectPool *pool, RyObject *o) {
    if(o->type == FUNCTION_OBJ) {
        RyFunction *f = (RyFunction *)o;
        RyCodeBlockFree(&f->block);
    } else if (o->type == STRING_OBJ) {
        FREE(((RyString *)o)->str);
    } else {
        printf("Invalid object type to release.\n");
        exit(1);
    }

    pool->bytes_allocated -= o->size;
    pool->obj_count--;

    FREE(o);
}

void RyObjectPoolMark(RyValue val) {
    if (val.type == STRING_OBJ || val.type == FUNCTION_OBJ) {
        val.as.object->refc++;
    }
}

RyValue RyObjectPoolBorrow_String(RyObjectPool *pool) {
    RyValue res = {
        .type = STRING_OBJ,
        .as.object = RyObjectPoolBorrow(pool, sizeof(RyString))
    };

    res.as.object->type = STRING_OBJ;

    return res;
}

RyValue RyObjectPoolBorrow_Function(RyObjectPool *pool) {
    RyFunction *f = (RyFunction *)RyObjectPoolBorrow(pool, sizeof(RyFunction));

    RyCodeBlockInit(&f->block);
    RyVariableArrayInit(&f->locals);
    f->ip = 0;
    
    RyValue res = {
        .type = FUNCTION_OBJ,
        .as.object = (RyObject *)f
    };

    res.as.object->type = FUNCTION_OBJ;

    return res;
}