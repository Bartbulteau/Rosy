#ifndef CODE_BLOCK_H
#define CODE_BLOCK_H

#include <stddef.h>
#include <stdint.h>
#include "../../memory_debug/memory_debug.h"

typedef struct RyBytecode_t {
    uint16_t opcode;
    uint16_t arg;
    size_t line;
} RyBytecode;

typedef struct RyCodeBlock_t {
    size_t capacity;
    size_t size;
    RyBytecode *code;
} RyCodeBlock;

void RyCodeBlockInit(RyCodeBlock *block);
void RyCodeBlockWrite(RyCodeBlock *block, uint16_t code, uint16_t arg, size_t line);
void RyCodeBlockFree(RyCodeBlock *block);

#endif // CODE_BLOCK_H