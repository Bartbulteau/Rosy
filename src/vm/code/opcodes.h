#ifndef OPCODES_H
#define OPCODES_H

#define OP_NOP                           0

#define OP_LOAD_CONST                    1
#define OP_POP                           2
#define OP_LOAD_LOCAL                    3
#define OP_STORE_LOCAL                   4
#define OP_LOAD_GLOBAL                   5
#define OP_STORE_GLOBAL                  6
#define OP_LOAD_PARAM                    7
#define OP_STORE_PARAM                   8

#define OP_CALL                          9
#define OP_RET                           10
#define OP_RET_NIL                       11

#define OP_JMP                           12
#define OP_JT                            13
#define OP_JF                            14
#define OP_JB                            15

#define OP_BINARY_OP                     16
#define OP_UNARY_OP                      17

#define OP_PRINT                         18

#define OP_HALT                          19


#define N_OPCODES 20


static char RyOpcodeNames[N_OPCODES][100] = {
	"NOP",
	"LOAD_CONST",
	"POP",
	"LOAD_LOCAL",
	"STORE_LOCAL",
	"LOAD_GLOBAL",
	"STORE_GLOBAL",
	"LOAD_PARAM",
	"STORE_PARAM",
	"CALL",
	"RET",
	"RET_NIL",
	"JMP",
	"JT",
	"JF",
	"JB",
	"BINARY_OP",
	"UNARY_OP",
	"PRINT",
	"HALT",
};


#endif // OPCODES_H
