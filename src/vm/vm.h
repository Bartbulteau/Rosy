#ifndef VM_H
#define VM_H

#include "code/opcodes.h"
#include "code/code_block.h"
#include "gc/gc.h"
#include "object/object.h"
#include "object/object_pool.h"
#include "object/operation.h"
#include "variable/variable_array.h"

#define VM_STACK_SIZE 512
#define VM_PARAM_STACK_SIZE 128
#define VM_CALL_STACK_SIZE 256
#define VM_IF_STACK_SIZE 64

typedef struct RyVM_t {
    RyValue stack[VM_STACK_SIZE];
    size_t sp;

    RyValue param_stack[VM_PARAM_STACK_SIZE];
    size_t psp;

    size_t call_stack[VM_CALL_STACK_SIZE];
    RyValue function_call_stack[VM_CALL_STACK_SIZE];
    RyVariableArray frame_call_stack[VM_CALL_STACK_SIZE];
    size_t csp;

    size_t if_stack[VM_IF_STACK_SIZE];
    size_t isp;

    RyVariableArray globals;
    RyVariableArray constants;

    RyObjectPool object_pool;
    
    RyValue current_func;

    size_t record_only;

    size_t main_save_ip;
} RyVM;

void runtime_error(const char *msg, RyBytecode bytecode);
void printStack(RyVM *vm);

void RyVMInit(RyVM *vm);
void RyVMFree(RyVM *vm);
void RyVMExecute(RyVM *vm, uint16_t opcode, uint16_t arg, size_t line);
void RyVMMakeFunction(RyVM *vm, size_t f_addr);
void RyVMEndMakeFunction(RyVM *vm);
void RyVMMakeControlFlow(RyVM *vm);
void RyVMEndMakeControlFlow(RyVM *vm, size_t ip, size_t offset);

void RyCollectGarbage(RyVM *vm);

#endif // VM_H