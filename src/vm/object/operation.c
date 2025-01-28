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

RyValue RyFalseBinaryResult(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(false);
}

RyValue RyTrueBinaryResult(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(true);
}

RyValue RyNumNumAdd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return NUM2VAL(VAL2NUM(val1) + VAL2NUM(val2));
}

RyValue RyStringStringAdd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    RyValue res = RyObjectPoolBorrow_String(pool);
    char *lhs = VAL2STRING(val1)->str;
    char *rhs = VAL2STRING(val2)->str;
    char *str = (char *)MALLOC(strlen(lhs) + strlen(rhs) + 1);
    strcpy(str, lhs);
    strcat(str, rhs);
    VAL2STRING(res)->str = str;
    return res;
}

RyValue RyStringNumAdd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    RyValue res = RyObjectPoolBorrow_String(pool);
    char *lhs = VAL2STRING(val1)->str;
    int rhs_len = snprintf(NULL, 0, "%g", VAL2NUM(val2));
    char *rhs = (char *)MALLOC(rhs_len + 1);
    snprintf(rhs, rhs_len + 1, "%g", VAL2NUM(val2));
    char *str = (char *)MALLOC(strlen(lhs) + rhs_len + 1);
    strcpy(str, lhs);
    strcat(str, rhs);
    FREE(rhs);
    VAL2STRING(res)->str = str;
    return res;
}

RyValue RyNumStringAdd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    RyValue res = RyObjectPoolBorrow_String(pool);
    int lhs_len = snprintf(NULL, 0, "%g", VAL2NUM(val1));
    char *lhs = (char *)MALLOC(lhs_len + 1);
    snprintf(lhs, lhs_len + 1, "%g", VAL2NUM(val1));
    char *rhs = VAL2STRING(val2)->str;
    char *str = (char *)MALLOC(lhs_len + strlen(rhs) + 1);
    strcpy(str, lhs);
    strcat(str, rhs);
    FREE(lhs);
    VAL2STRING(res)->str = str;
    return res;
}

void RyLoadAdd() {
    RyBinaryOps[ADD_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumAdd;
    RyBinaryOps[ADD_BINOP][STRING_OBJ][STRING_OBJ] = &RyStringStringAdd;
    RyBinaryOps[ADD_BINOP][STRING_OBJ][NUMBER_OBJ] = &RyStringNumAdd;
    RyBinaryOps[ADD_BINOP][NUMBER_OBJ][STRING_OBJ] = &RyNumStringAdd;
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

RyValue RyStringStringEq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(strcmp(VAL2STRING(val1)->str, VAL2STRING(val2)->str) == 0);
}

RyValue RyStringStringNeq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(strcmp(VAL2STRING(val1)->str, VAL2STRING(val2)->str) != 0);
}

RyValue RyBoolBoolEq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2BOOL(val1) == VAL2BOOL(val2));
}

RyValue RyBoolBoolNeq(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2BOOL(val1) != VAL2BOOL(val2));
}

void RyLoadComparisions() {
    
    for (int i = 0; i < NUM_OF_OBJ_TYPE; i++) {
        for (int j = 0; j < NUM_OF_OBJ_TYPE; j++) {
            if (i != j) {
                RyBinaryOps[EQ_BINOP][i][j] = &RyFalseBinaryResult;
                RyBinaryOps[NEQ_BINOP][i][j] = &RyTrueBinaryResult;
            }
        }
    }

    RyBinaryOps[LESS_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumLess;
    RyBinaryOps[LEQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumLeq;
    RyBinaryOps[GREAT_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumGreat;
    RyBinaryOps[GREQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumGreq;
    RyBinaryOps[EQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumEq;
    RyBinaryOps[NEQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumNeq;

    RyBinaryOps[EQ_BINOP][STRING_OBJ][STRING_OBJ] = &RyStringStringEq;
    RyBinaryOps[NEQ_BINOP][STRING_OBJ][STRING_OBJ] = &RyStringStringNeq;

    RyBinaryOps[EQ_BINOP][BOOL_OBJ][BOOL_OBJ] = &RyBoolBoolEq;
    RyBinaryOps[NEQ_BINOP][BOOL_OBJ][BOOL_OBJ] = &RyBoolBoolNeq;
}

RyValue RyBoolBoolAnd(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2BOOL(val1) && VAL2BOOL(val2));
}

RyValue RyBoolBoolOr(RyObjectPool *pool, RyValue val1, RyValue val2) {
    return BOOL2VAL(VAL2BOOL(val1) || VAL2BOOL(val2));
}

void RyLoadLogics() {
    RyBinaryOps[AND_BINOP][BOOL_OBJ][BOOL_OBJ] = &RyBoolBoolAnd;
    RyBinaryOps[OR_BINOP][BOOL_OBJ][BOOL_OBJ] = &RyBoolBoolOr;
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