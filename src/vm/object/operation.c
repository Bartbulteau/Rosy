#include "operation.h"

RyBinaryOperation RyBinaryOps[N_BINOP][NUM_OF_OBJ_TYPE][NUM_OF_OBJ_TYPE];
RyUnaryOperation RyUnaryOps[N_UNOP][NUM_OF_OBJ_TYPE];

RyValue RyInvalidBinaryTypes(RyObjectPool *pool, RyValue val1, RyValue val2) {
    RyValue res = {.type = NIL_OBJ};

    printf("Incompatible object types %s and %s in binary operation.\n", RyValueTypeNames[val1.type], RyValueTypeNames[val2.type]);

    return res;
}

RyValue RyInvalidUnaryType(RyObjectPool *pool, RyValue val) {
    RyValue res = {.type = NIL_OBJ};

    printf("Invalid object type %s in unary operation.\n", RyValueTypeNames[val.type]);

    return res;
}

RyValue RyNumNumAdd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return NUM2VAL(VAL2NUM(val1) + VAL2NUM(val2));
}

RyValue RyStringStringAdd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    RyValue res = RyObjectPoolBorrow_String(pool);
    char *lhs = ((RyString *)val1.as.object)->str;
    char *rhs = ((RyString *)val2.as.object)->str;
    char *str = (char *)MALLOC(strlen(lhs) + strlen(rhs) + 1);
    strcpy(str, lhs);
    strcat(str, rhs);
    ((RyString *)res.as.object)->str = str;
    return res;
}

void RyLoadAdd() {
    RyBinaryOps[ADD_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumAdd;
    RyBinaryOps[ADD_BINOP][STRING_OBJ][STRING_OBJ] = &RyStringStringAdd;
}

RyValue RyNumNumSub(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return NUM2VAL(VAL2NUM(val1) - VAL2NUM(val2));
}

void RyLoadSub() {
    RyBinaryOps[SUB_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumSub;
}

RyValue RyNumNumMul(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return NUM2VAL(VAL2NUM(val1) * VAL2NUM(val2));
}

void RyLoadMul() {
    RyBinaryOps[MUL_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumMul;
}

RyValue RyNumNumDiv(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return NUM2VAL(VAL2NUM(val1) / VAL2NUM(val2));
}

void RyLoadDiv() {
    RyBinaryOps[DIV_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumDiv;
}

RyValue RyNumNumMod(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return NUM2VAL((int64_t)(VAL2NUM(val1)) % (int64_t)(VAL2NUM(val2)));
}

void RyLoadMod() {
    RyBinaryOps[MOD_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumMod;
}

RyValue RyNumNumLess(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2NUM(val1) < VAL2NUM(val2));
}

RyValue RyNumNumLeq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2NUM(val1) <= VAL2NUM(val2));
}

RyValue RyNumNumGreat(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2NUM(val1) > VAL2NUM(val2));
}

RyValue RyNumNumGreq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2NUM(val1) >= VAL2NUM(val2));
}

RyValue RyNumNumEq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2NUM(val1) == VAL2NUM(val2));
}

RyValue RyNumNumNeq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2NUM(val1) != VAL2NUM(val2));
}

void RyLoadComparisions() {
    RyBinaryOps[LESS_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumLess;
    RyBinaryOps[LEQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumLeq;
    RyBinaryOps[GREAT_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumGreat;
    RyBinaryOps[GREQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumGreq;
    RyBinaryOps[EQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumEq;
    RyBinaryOps[NEQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumNeq;
}

RyValue RyBoolBoolAnd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2BOOL(val1) && VAL2BOOL(val2));
}

RyValue RyBoolBoolOr(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2BOOL(val1) || VAL2BOOL(val2));
}

void RyLoadLogics() {
    RyBinaryOps[AND_BINOP][BOOL_OBJ][BOOL_OBJ] = &RyNumNumLess;
    RyBinaryOps[OR_BINOP][BOOL_OBJ][BOOL_OBJ] = &RyNumNumLess;
}

void RyLoadBinOps() {

    for (size_t i = 0; i < N_BINOP; i++) {
        for (size_t j = 0; j < NUM_OF_OBJ_TYPE; j++) {
            for (size_t k = 0; k < NUM_OF_OBJ_TYPE; k++) {
                RyBinaryOps[i][j][k] = &RyInvalidBinaryTypes;
            }
        }
    }

    RyLoadAdd();
    RyLoadSub();
    RyLoadMul();
    RyLoadDiv();
    RyLoadMod();
    RyLoadComparisions();
    RyLoadLogics();
}

RyValue RyNumOpp(RyObjectPool *pool, RyValue val) {
    return NUM2VAL(-VAL2NUM(val));
}

RyValue RyBoolNot(RyObjectPool *pool, RyValue val) {
    return BOOL2VAL(!VAL2BOOL(val));
}

void RyLoadUnOps() {
    for (size_t i = 0; i < N_UNOP; i++) {
        for(size_t j = 0; j < NUM_OF_OBJ_TYPE; j++) {
            RyUnaryOps[i][j] = &RyInvalidUnaryType;
        }
    }

    RyUnaryOps[OPP_UNOP][NUMBER_OBJ] = &RyNumOpp;
    RyUnaryOps[NOT_UNOP][BOOL_OBJ] = &RyBoolNot;
}