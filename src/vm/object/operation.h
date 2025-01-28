#ifndef OPERATION_H
#define OPERATION_H

#include <string.h>
#include "../../memory_debug/memory_debug.h"
#include "object.h"
#include "object_pool.h"

typedef RyValue (*RyBinaryOperation)(RyObjectPool *, RyValue, RyValue);
typedef RyValue (*RyUnaryOperation)(RyObjectPool *, RyValue);

typedef enum RyBinaryOpType_t {
    ADD_BINOP,
    SUB_BINOP,
    MUL_BINOP,
    DIV_BINOP,
    MOD_BINOP,

    LESS_BINOP,
    LEQ_BINOP,
    GREAT_BINOP,
    GREQ_BINOP,
    EQ_BINOP,
    NEQ_BINOP,
    AND_BINOP,
    OR_BINOP,
    N_BINOP
} RyBinaryOpType;

typedef enum RyUnaryOpType_t {
    OPP_UNOP,
    NOT_UNOP,
    N_UNOP,
} RyUnaryOpType;

extern RyBinaryOperation RyBinaryOps[N_BINOP][NUM_OF_OBJ_TYPE][NUM_OF_OBJ_TYPE];
extern RyUnaryOperation RyUnaryOps[N_UNOP][NUM_OF_OBJ_TYPE];

/*
    BINARY OPERATIONS
*/

// ERRORS

RyValue RyInvalidBinaryTypes(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyInvalidUnaryType(RyObjectPool *pool, RyValue val);

// ADDITION

RyValue RyNumNumAdd(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyStringStringAdd(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadAdd();

// SUBSTRACTION

RyValue RyNumNumSub(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadSub();

// MULTIPLICATION

RyValue RyNumNumMul(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadMul();

// DIVISION

RyValue RyNumNumDiv(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadDiv();

// MODULO

RyValue RyNumNumMod(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadMod();

// COMPARISIONS

RyValue RyNumNumLess(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyNumNumLeq(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyNumNumGreat(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyNumNumGreq(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyNumNumEq(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyNumNumNeq(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadComparisions();

// LOGIC

RyValue RyBoolBoolAnd(RyObjectPool *pool, RyValue val1, RyValue val2);
RyValue RyBoolBoolOr(RyObjectPool *pool, RyValue val1, RyValue val2);

void RyLoadLogics();

void RyLoadBinOps();

/*
    UNARY OPERATIONS
*/

RyValue RyNumOpp(RyObjectPool *pool, RyValue val);
RyValue RyBoolNot(RyObjectPool *pool, RyValue val);

void RyLoadUnOps();

#endif // OPERATION_H