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
    o->next = NULL;
    o->next_allocated = pool->allocated_list;
    pool->allocated_list = o;
    o->refc = 0;
    o->size = size;
    o->type = UNINITIALIZED_OBJ;

    pool->obj_count++;
    pool->bytes_allocated += size;

    return o;
}

void RyObjectPoolRelease(RyObjectPool *pool, RyObject *o) {

    if(o->type == FUNCTION_OBJ) {
        RyFunction *f = (RyFunction *)o;
        RyCodeBlockFree(&f->block);
    } else if (o->type == STRING_OBJ) {
        FREE(((RyString *)o)->value);
    }

    pool->bytes_allocated -= o->size;
    pool->obj_count--;

    FREE(o);
}

RyNil *RyObjectPoolBorrow_Nil(RyObjectPool *pool) {
    RyNil *nil = (RyNil *)RyObjectPoolBorrow(pool, sizeof(RyNil));
    nil->base.type = NIL_OBJ;
    return nil;
}

RyNumber *RyObjectPoolBorrow_Number(RyObjectPool *pool) {
    RyNumber *num = (RyNumber *)RyObjectPoolBorrow(pool, sizeof(RyNumber));
    num->base.type = NUMBER_OBJ;
    num->value = 0;
    return num;
}

RyBool *RyObjectPoolBorrow_Bool(RyObjectPool *pool) {
    RyBool *b = (RyBool *)RyObjectPoolBorrow(pool, sizeof(RyBool));
    b->base.type = BOOL_OBJ;
    b->value = true;
    return b;
}

RyString *RyObjectPoolBorrow_String(RyObjectPool *pool) {
    RyString *str = (RyString *)RyObjectPoolBorrow(pool, sizeof(RyString));
    str->base.type = STRING_OBJ;
    str->value = NULL;
    return str;
}

RyFunction *RyObjectPoolBorrow_Function(RyObjectPool *pool) {
    RyFunction *f = (RyFunction *)RyObjectPoolBorrow(pool, sizeof(RyFunction));
    f->base.type = FUNCTION_OBJ;
    RyCodeBlockInit(&f->block);
    RyVariableArrayInit(&f->locals);
    f->ip = 0;
    return f;
}