#include "code_block.h"

void RyCodeBlockInit(RyCodeBlock *block) {
    block->capacity = 0;
    block->size = 0;
    block->code = NULL;
}

void RyCodeBlockWrite(RyCodeBlock *block, uint16_t code, uint16_t arg, size_t line) {
    if (block->capacity < block->size + 1) {
        size_t new_capacity = block->capacity < 8 ? 8 : (block->capacity * 2);
        block->capacity = new_capacity;
        block->code = (RyBytecode *)REALLOC(block->code, new_capacity * sizeof(RyBytecode));
    }
    
    RyBytecode bytecode = {
        .opcode = code,
        .arg = arg,
        .line = line
    };

    block->code[block->size] = bytecode;
    block->size++;
}

void RyCodeBlockFree(RyCodeBlock *block) {
    FREE(block->code);
}