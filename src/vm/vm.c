#include "vm.h"

void runtime_error(const char *msg, RyBytecode bytecode) {
    printf("Runtime error [%s %d, l%zu]: %s\n", RyOpcodeNames[bytecode.opcode], bytecode.arg, bytecode.line, msg);
    exit(1);
}

void printStack(RyVM *vm) {
    printf("\n\n==========================\n");
    printf("           STACK          \n");
    printf("==========================\n");
    for (size_t i = 0; i < vm->sp; i++) {
        RyValue val = vm->stack[i];
        if (val.type == NUMBER_OBJ) {
            printf("%g\n", VAL2NUM(val));
        } else if (val.type == STRING_OBJ) {
            printf("%s\n", VAL2STRING(val)->str);
        } else if (val.type == BOOL_OBJ) {
            printf("%s\n", VAL2BOOL(val) ? "true" : "false");
        } else {
            printf("<%s %p>\n", RyValueTypeNames[val.type], val.as.object);
        }
        printf("==========================\n");
    }
    printf("==========================\n\n");
}

void RyVMInit(RyVM *vm) {
    vm->sp = 0;
    vm->psp = 0;
    vm->csp = 0;
    vm->isp = 0;

    RyVariableArrayInit(&vm->globals);
    RyVariableArrayInit(&vm->constants);

    RyObjectPoolInit(&vm->object_pool);

    RyValue main_func = RyObjectPoolBorrow_Function(&vm->object_pool);
    RyVariableArrayWrite(&vm->globals, 0, main_func);
    vm->current_func = main_func;

    RyValue nil = {.type = NIL_OBJ};
    RyVariableArrayWrite(&vm->constants, 0, nil);

    vm->record_only = 0;

    RyLoadBinOps();
    RyLoadUnOps();
}

void RyVMFree(RyVM *vm) {
    RyVariableArrayFree(&VAL2FUNC(RyVariableArrayGet(&vm->globals, 0))->locals);
    RyVariableArrayFree(&vm->globals);
    RyVariableArrayFree(&vm->constants);
    #ifdef MEMORY_DEBUG
    printf("\n\nNumber of objects allocated: %zu.\n", vm->object_pool.obj_count);
    printf("Average object size: %f\n", ((double)vm->object_pool.bytes_allocated)/vm->object_pool.obj_count);
    #endif
    RyObjectPoolFree(&vm->object_pool);
}

void RyVMExecute(RyVM *vm, uint16_t opcode, uint16_t arg, size_t line) {

    RyCodeBlockWrite(&VAL2FUNC(vm->current_func)->block, opcode, arg, line);
    if (vm->record_only > 0) {return;}

    RyValue val, val1, val2;

    while (VAL2FUNC(vm->current_func)->ip < VAL2FUNC(vm->current_func)->block.size) {

        RyBytecode bytecode = VAL2FUNC(vm->current_func)->block.code[VAL2FUNC(vm->current_func)->ip];

        if (vm->object_pool.bytes_allocated > vm->object_pool.next_gc) RyCollectGarbage(vm);

        // printStack(vm);
        // for (size_t i = 0; i < vm->csp; i++) printf("\t");
        // printf("%s %d\n", RyOpcodeNames[bytecode.opcode], bytecode.arg);

        switch(bytecode.opcode) {

            case OP_NOP:
                break;

            case OP_LOAD_CONST:
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                val = RyVariableArrayGet(&vm->constants, bytecode.arg);
                vm->stack[vm->sp++] = val;
                
                break;

            case OP_POP:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];                
                break;

            case OP_LOAD_LOCAL:
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                val = RyVariableArrayGet(&VAL2FUNC(vm->current_func)->locals, bytecode.arg);
                
                vm->stack[vm->sp++] = val;
                break;

            case OP_STORE_LOCAL:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[vm->sp - 1];
                RyVariableArrayWrite(&VAL2FUNC(vm->current_func)->locals, bytecode.arg, val);
                break;

            case OP_LOAD_GLOBAL:
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                val = RyVariableArrayGet(&vm->globals, bytecode.arg);
                vm->stack[vm->sp++] = val;
                
                break;
            
            case OP_STORE_GLOBAL:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[vm->sp - 1];
                RyVariableArrayWrite(&vm->globals, bytecode.arg, val);
                break;

            case OP_LOAD_PARAM:
                if (vm->psp >= VM_PARAM_STACK_SIZE) runtime_error("parameters stack overflow.", bytecode);
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                vm->param_stack[vm->psp++] = vm->stack[--vm->sp];
                break;
            
            case OP_STORE_PARAM:
                if (vm->psp < 1) runtime_error("parameters stack underflow.", bytecode);
                RyVariableArrayWrite(&VAL2FUNC(vm->current_func)->locals, bytecode.arg, vm->param_stack[--vm->psp]);
                break;

            case OP_CALL:
                if (vm->csp >= VM_CALL_STACK_SIZE) runtime_error("call stack overflow.", bytecode);
                vm->call_stack[vm->csp] = VAL2FUNC(vm->current_func)->ip;
                vm->frame_call_stack[vm->csp] = VAL2FUNC(vm->current_func)->locals;
                vm->function_call_stack[vm->csp++] = vm->current_func;
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];
                if (val.type != FUNCTION_OBJ) runtime_error("invalid object type to call.", bytecode);
                vm->current_func = val;
                VAL2FUNC(vm->current_func)->ip = 0;
                RyVariableArrayInit(&VAL2FUNC(vm->current_func)->locals);
                continue;
                break;

            case OP_RET:
                if (vm->csp < 1) runtime_error("call stack underflow.", bytecode);
                RyVariableArrayFree(&VAL2FUNC(vm->current_func)->locals);
                vm->current_func = vm->function_call_stack[--vm->csp];
                VAL2FUNC(vm->current_func)->ip = vm->call_stack[vm->csp];
                VAL2FUNC(vm->current_func)->locals = vm->frame_call_stack[vm->csp];
                break;

            case OP_RET_NIL:
                if (vm->csp < 1) runtime_error("call stack underflow.", bytecode);
                RyVariableArrayFree(&VAL2FUNC(vm->current_func)->locals);
                vm->current_func = vm->function_call_stack[--vm->csp];
                VAL2FUNC(vm->current_func)->ip = vm->call_stack[vm->csp];
                VAL2FUNC(vm->current_func)->locals = vm->frame_call_stack[vm->csp];
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                val = RyVariableArrayGet(&vm->constants, 0);
                vm->stack[vm->sp++] = val;
                break;

            case OP_JMP:
                VAL2FUNC(vm->current_func)->ip += bytecode.arg;
                break;

            case OP_JF:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];
                if (val.type != BOOL_OBJ) runtime_error("expected boolean value in 'jump if false'.", bytecode);
                if (!VAL2BOOL(val)) VAL2FUNC(vm->current_func)->ip += bytecode.arg;
                
                break;

            case OP_JT:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];
                if (val.type != BOOL_OBJ) runtime_error("expected boolean value in 'jump if true'.", bytecode);
                if (VAL2BOOL(val)) VAL2FUNC(vm->current_func)->ip += bytecode.arg;
                
                break;

            case OP_JB:
                VAL2FUNC(vm->current_func)->ip -= bytecode.arg;
                break;

            case OP_BINARY_OP:
                if (vm->sp < 2) runtime_error("stack underflow.", bytecode);
                val1 = vm->stack[--vm->sp];
                val2 = vm->stack[--vm->sp];
                val = RyBinaryOps[bytecode.arg][val2.type][val1.type](&vm->object_pool, val2, val1);
                if (val.type == NIL_OBJ) runtime_error("incompatible types in binary operation.", bytecode);
                vm->stack[vm->sp++] = val;
                break;

            case OP_UNARY_OP:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];
                val1 = RyUnaryOps[bytecode.arg][val.type](&vm->object_pool, val);
                if (val.type == NIL_OBJ) runtime_error("incompatible type in unary operation.", bytecode);
                vm->stack[vm->sp++] = val1;
                break;

            case OP_PRINT:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];
                if (val.type == NUMBER_OBJ) {
                    printf("%g", VAL2NUM(val));
                } else if (val.type == STRING_OBJ) {
                    printf("%s", VAL2STRING(val)->str);
                } else if (val.type == BOOL_OBJ) {
                    printf("%s", VAL2BOOL(val) ? "true" : "false");
                } else {
                    printf("<%s %p>", RyValueTypeNames[val.type], val.as.object);
                }
                
                break;

            case OP_HALT:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                val = vm->stack[--vm->sp];
                if (val.type == NUMBER_OBJ) {
                    if (VAL2NUM(val) == 0) exit(EXIT_SUCCESS);
                    exit(EXIT_FAILURE);
                } else {
                    exit(EXIT_FAILURE);
                }

            default:
                if (bytecode.opcode < N_OPCODES) runtime_error("opcode not implemented.", bytecode);
                printf("Runtime error : unrecognized opcode %d with argument %d generated at line %zu.\n", bytecode.opcode, bytecode.arg, bytecode.line);
                exit(1);
        }
        VAL2FUNC(vm->current_func)->ip++;
    }
}

void RyVMMakeFunction(RyVM *vm, size_t f_addr) {
    vm->main_save_ip = VAL2FUNC(vm->current_func)->ip;
    vm->current_func = RyObjectPoolBorrow_Function(&vm->object_pool);
    RyVariableArrayWrite(&vm->globals, f_addr, vm->current_func);
    vm->record_only++;
}

void RyVMEndMakeFunction(RyVM *vm) {
    vm->current_func = RyVariableArrayGet(&vm->globals, 0);
    vm->record_only--;
    VAL2FUNC(vm->current_func)->ip = vm->main_save_ip;
}

void RyVMMakeControlFlow(RyVM *vm) {
    vm->record_only++;
}

void RyVMEndMakeControlFlow(RyVM *vm, size_t ip, size_t offset) {
    VAL2FUNC(vm->current_func)->block.code[ip].arg = offset;
    vm->record_only--;
}

void RyCollectGarbage(RyVM *vm) {

    // mark
    for(size_t i = 0; i < vm->globals.size; i++) {
        RyObjectPoolMark(vm->globals.entries[i]);
    }

    for(size_t i = 0; i < vm->constants.size; i++) {
        RyObjectPoolMark(vm->constants.entries[i]);
    }

    for(size_t i = 0; i < VAL2FUNC(vm->current_func)->locals.size; i++) {
        VAL2FUNC(vm->current_func)->locals.entries[i];
    }

    for(size_t i = 0; i < vm->csp; i++) {
        for(size_t j = 0; j < vm->frame_call_stack[i].size; j++) {
            RyObjectPoolMark(vm->frame_call_stack[i].entries[j]);
        }
    }

    for(size_t i = 0; i < vm->psp; i++) {
        RyObjectPoolMark(vm->param_stack[i]);
    }

    for(size_t i = 0; i < vm->sp; i++) {
        RyObjectPoolMark(vm->stack[i]);
    }

    // sweep

    RyObject *prev = NULL;
    RyObject *o = vm->object_pool.allocated_list;

    while (o != NULL) {
        if (o->refc > 0) {
            o->refc = 0;
            prev = o;
            o = o->next_allocated;
        } else {
            RyObject *unreachable = o;
            o = o->next_allocated;
            if (prev != NULL) {
                prev->next_allocated = o;
            } else {
                vm->object_pool.allocated_list = o;
            }

            RyObjectPoolRelease(&vm->object_pool, unreachable);
        }
    }

    vm->object_pool.next_gc = vm->object_pool.bytes_allocated * GC_HEAP_GROWTH_FACTOR;
}