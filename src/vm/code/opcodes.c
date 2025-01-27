#include "opcodes.h"

char RyOpcodeNames[N_OPCODES][100] = {
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