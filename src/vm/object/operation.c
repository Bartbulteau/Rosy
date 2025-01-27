#include "operation.h"

RyBinaryOperation RyBinaryOps[N_BINOP][NUM_OF_OBJ_TYPE][NUM_OF_OBJ_TYPE];
RyUnaryOperation RyUnaryOps[N_UNOP][NUM_OF_OBJ_TYPE];

RyObject *RyInvalidBinaryTypes(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyObject *res = (RyObject *)RyObjectPoolBorrow_Nil(pool);

    printf("Incompatible object types %s and %s in binary operation.\n", RyObjectTypeNames[o1->type], RyObjectTypeNames[o2->type]);

    return res;
}

RyObject *RyInvalidUnaryType(RyObjectPool *pool, RyObject *o) {
    RyObject *res = (RyObject *)RyObjectPoolBorrow_Nil(pool);

    printf("Invalid object type %s in unary operation.\n", RyObjectTypeNames[o->type]);

    return res;
}

RyObject *RyNumNumAdd(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyNumber *res = RyObjectPoolBorrow_Number(pool);
    res->value = ((RyNumber *)o1)->value + ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyStringStringAdd(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyString *res = RyObjectPoolBorrow_String(pool);
    char *lhs = ((RyString *)o1)->value;
    char *rhs = ((RyString *)o2)->value;
    res->value = (char *)MALLOC(strlen(lhs) + strlen(rhs) + 1);
    strcpy(res->value, lhs);
    strcat(res->value, rhs);
    return (RyObject *)res;
}

void RyLoadAdd() {
    RyBinaryOps[ADD_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumAdd;
    RyBinaryOps[ADD_BINOP][STRING_OBJ][STRING_OBJ] = &RyStringStringAdd;
}

RyObject *RyNumNumSub(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyNumber *res = RyObjectPoolBorrow_Number(pool);
    res->value = ((RyNumber *)o1)->value - ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

void RyLoadSub() {
    RyBinaryOps[SUB_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumSub;
}

RyObject *RyNumNumMul(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyNumber *res = RyObjectPoolBorrow_Number(pool);
    res->value = ((RyNumber *)o1)->value * ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

void RyLoadMul() {
    RyBinaryOps[MUL_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumMul;
}

RyObject *RyNumNumDiv(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyNumber *res = RyObjectPoolBorrow_Number(pool);
    res->value = ((RyNumber *)o1)->value / ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

void RyLoadDiv() {
    RyBinaryOps[DIV_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumDiv;
}

RyObject *RyNumNumMod(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyNumber *res = RyObjectPoolBorrow_Number(pool);
    res->value = (int64_t)((RyNumber *)o1)->value % (int64_t)((RyNumber *)o2)->value;
    return (RyObject *)res;
}

void RyLoadMod() {
    RyBinaryOps[MOD_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumMod;
}

RyObject *RyNumNumLess(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyNumber *)o1)->value < ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyNumNumLeq(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyNumber *)o1)->value <= ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyNumNumGreat(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyNumber *)o1)->value > ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyNumNumGreq(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyNumber *)o1)->value >= ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyNumNumEq(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyNumber *)o1)->value == ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyNumNumNeq(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyNumber *)o1)->value != ((RyNumber *)o2)->value;
    return (RyObject *)res;
}

void RyLoadComparisions() {
    RyBinaryOps[LESS_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumLess;
    RyBinaryOps[LEQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumLeq;
    RyBinaryOps[GREAT_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumGreat;
    RyBinaryOps[GREQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumGreq;
    RyBinaryOps[EQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumEq;
    RyBinaryOps[NEQ_BINOP][NUMBER_OBJ][NUMBER_OBJ] = &RyNumNumNeq;
}

RyObject *RyBoolBoolAnd(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyBool *)o1)->value && ((RyBool *)o2)->value;
    return (RyObject *)res;
}

RyObject *RyBoolBoolOr(RyObjectPool *pool, RyObject *o1, RyObject *o2) {
    RyBool *res = RyObjectPoolBorrow_Bool(pool);
    res->value = ((RyBool *)o1)->value || ((RyBool *)o2)->value;
    return (RyObject *)res;
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

RyObject *RyNumOpp(RyObjectPool *pool, RyObject *o) {
    RyNumber *res = RyObjectPoolBorrow_Number(pool);
    res->value = -((RyNumber *)o)->value;
    return (RyObject *)res;
}

RyObject *RyBoolNot(RyObjectPool *pool, RyObject *o) {
    RyBool *b = RyObjectPoolBorrow_Bool(pool);
    b->value = !((RyBool *)o)->value;
    return (RyObject *)b;
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