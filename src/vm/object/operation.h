#ifndef OPERATION_H
#define OPERATION_H

#include <string.h>
#include "../../memory_debug/memory_debug.h"
#include "object.h"
#include "object_pool.h"

typedef RyObject *( *RyBinaryOperation)(RyObjectPool *, RyObject *, RyObject *);
typedef RyObject *( *RyUnaryOperation)(RyObjectPool *, RyObject *);

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

RyObject *RyInvalidBinaryTypes(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyInvalidUnaryType(RyObjectPool *pool, RyObject *o);

// ADDITION

RyObject *RyNumNumAdd(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyStringStringAdd(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadAdd();

// SUBSTRACTION

RyObject *RyNumNumSub(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadSub();

// MULTIPLICATION

RyObject *RyNumNumMul(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadMul();

// DIVISION

RyObject *RyNumNumDiv(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadDiv();

// MODULO

RyObject *RyNumNumMod(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadMod();

// COMPARISIONS

RyObject *RyNumNumLess(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyNumNumLeq(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyNumNumGreat(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyNumNumGreq(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyNumNumEq(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyNumNumNeq(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadComparisions();

// LOGIC

RyObject *RyBoolBoolAnd(RyObjectPool *pool, RyObject *o1, RyObject *o2);
RyObject *RyBoolBoolOr(RyObjectPool *pool, RyObject *o1, RyObject *o2);

void RyLoadLogics();

void RyLoadBinOps();

/*
    UNARY OPERATIONS
*/

RyObject *RyNumOpp(RyObjectPool *pool, RyObject *o);
RyObject *RyBoolNot(RyObjectPool *pool, RyObject *o);

void RyLoadUnOps();

#endif // OPERATION_H