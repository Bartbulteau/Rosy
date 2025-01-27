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
        RyObject *o = vm->stack[i];
        if (o->type == NUMBER_OBJ) {
            printf("%g\n", ((RyNumber *)o)->value);
        } else if (o->type == STRING_OBJ) {
            printf("%s\n", ((RyString *)o)->value);
        } else if (o->type == BOOL_OBJ) {
            printf("%s\n", ((RyBool *)o)->value ? "true" : "false");
        } else {
            printf("<%s %p>\n", RyObjectTypeNames[o->type], o);
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

    RyFunction *main_func = RyObjectPoolBorrow_Function(&vm->object_pool);
    RyVariableArrayWrite(&vm->globals, 0, (RyObject *)main_func);
    vm->current_func = main_func;

    RyObject *nil = (RyObject *)RyObjectPoolBorrow_Nil(&vm->object_pool);
    RyVariableArrayWrite(&vm->constants, 0, nil);

    vm->record_only = 0;

    RyLoadBinOps();
    RyLoadUnOps();
}

void RyVMFree(RyVM *vm) {
    RyVariableArrayFree(&((RyFunction *)RyVariableArrayGet(&vm->globals, 0))->locals);
    RyVariableArrayFree(&vm->globals);
    RyVariableArrayFree(&vm->constants);
    #ifdef MEMORY_DEBUG
    printf("\n\nNumber of objects allocated: %zu.\n", vm->object_pool.obj_count);
    printf("Average object size: %f\n", ((double)vm->object_pool.bytes_allocated)/vm->object_pool.obj_count);
    #endif
    RyObjectPoolFree(&vm->object_pool);
}

void RyVMExecute(RyVM *vm, uint16_t opcode, uint16_t arg, size_t line) {

    RyCodeBlockWrite(&vm->current_func->block, opcode, arg, line);
    if (vm->record_only > 0) {return;}

    RyObject *o, *o1, *o2;

    while (vm->current_func->ip < vm->current_func->block.size) {

        RyBytecode bytecode = vm->current_func->block.code[vm->current_func->ip];

        if (vm->object_pool.bytes_allocated > vm->object_pool.next_gc) RyCollectGarbage(vm);

        // printStack(vm);
        // for (size_t i = 0; i < vm->csp; i++) printf("\t");
        // printf("%s %d\n", RyOpcodeNames[bytecode.opcode], bytecode.arg);

        switch(bytecode.opcode) {

            case OP_NOP:
                break;

            case OP_LOAD_CONST:
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                o = RyVariableArrayGet(&vm->constants, bytecode.arg);
                vm->stack[vm->sp++] = o;
                
                break;

            case OP_POP:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                
                break;

            case OP_LOAD_LOCAL:
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                o = RyVariableArrayGet(&vm->current_func->locals, bytecode.arg);
                
                vm->stack[vm->sp++] = o;
                break;

            case OP_STORE_LOCAL:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[vm->sp - 1];
                RyVariableArrayWrite(&vm->current_func->locals, bytecode.arg, o);
                break;

            case OP_LOAD_GLOBAL:
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                o = RyVariableArrayGet(&vm->globals, bytecode.arg);
                vm->stack[vm->sp++] = o;
                
                break;
            
            case OP_STORE_GLOBAL:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[vm->sp - 1];
                RyVariableArrayWrite(&vm->globals, bytecode.arg, o);
                break;

            case OP_LOAD_PARAM:
                if (vm->psp >= VM_PARAM_STACK_SIZE) runtime_error("parameters stack overflow.", bytecode);
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                vm->param_stack[vm->psp++] = vm->stack[--vm->sp];
                break;
            
            case OP_STORE_PARAM:
                if (vm->psp < 1) runtime_error("parameters stack underflow.", bytecode);
                RyVariableArrayWrite(&vm->current_func->locals, bytecode.arg, vm->param_stack[--vm->psp]);
                break;

            case OP_CALL:
                if (vm->csp >= VM_CALL_STACK_SIZE) runtime_error("call stack overflow.", bytecode);
                vm->call_stack[vm->csp] = vm->current_func->ip;
                vm->frame_call_stack[vm->csp] = vm->current_func->locals;
                vm->function_call_stack[vm->csp++] = vm->current_func;
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                if (o->type != FUNCTION_OBJ) runtime_error("invalid object type to call.", bytecode);
                vm->current_func = (RyFunction *)o;
                vm->current_func->ip = 0;
                RyVariableArrayInit(&vm->current_func->locals);
                continue;
                break;

            case OP_RET:
                if (vm->csp < 1) runtime_error("call stack underflow.", bytecode);
                RyVariableArrayFree(&vm->current_func->locals);
                vm->current_func = vm->function_call_stack[--vm->csp];
                vm->current_func->ip = vm->call_stack[vm->csp];
                vm->current_func->locals = vm->frame_call_stack[vm->csp];
                break;

            case OP_RET_NIL:
                if (vm->csp < 1) runtime_error("call stack underflow.", bytecode);
                RyVariableArrayFree(&vm->current_func->locals);
                vm->current_func = vm->function_call_stack[--vm->csp];
                vm->current_func->ip = vm->call_stack[vm->csp];
                vm->current_func->locals = vm->frame_call_stack[vm->csp];
                if (vm->sp >= VM_STACK_SIZE) runtime_error("stack overflow.", bytecode);
                o = RyVariableArrayGet(&vm->constants, 0);
                vm->stack[vm->sp++] = o;
                break;

            case OP_JMP:
                vm->current_func->ip += bytecode.arg;
                break;

            case OP_JF:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                if (o->type != BOOL_OBJ) runtime_error("expected boolean value in 'jump if false'.", bytecode);
                if (!((RyBool *)o)->value) vm->current_func->ip += bytecode.arg;
                
                break;

            case OP_JT:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                if (o->type != BOOL_OBJ) runtime_error("expected boolean value in 'jump if true'.", bytecode);
                if (((RyBool *)o)->value) vm->current_func->ip += bytecode.arg;
                
                break;

            case OP_JB:
                vm->current_func->ip -= bytecode.arg;
                break;

            case OP_BINARY_OP:
                if (vm->sp < 2) runtime_error("stack underflow.", bytecode);
                o1 = vm->stack[--vm->sp];
                o2 = vm->stack[--vm->sp];
                o = RyBinaryOps[bytecode.arg][o2->type][o1->type](&vm->object_pool, o2, o1);
                if (o->type == NIL_OBJ) runtime_error("incompatible types in binary operation.", bytecode);
                vm->stack[vm->sp++] = o;
                
                
                
                break;

            case OP_UNARY_OP:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                o1 = RyUnaryOps[bytecode.arg][o->type](&vm->object_pool, o);
                if (o->type == NIL_OBJ) runtime_error("incompatible type in unary operation.", bytecode);
                vm->stack[vm->sp++] = o1;
                
                
                break;

            case OP_PRINT:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                if (o->type == NUMBER_OBJ) {
                    printf("%g", ((RyNumber *)o)->value);
                } else if (o->type == STRING_OBJ) {
                    printf("%s", ((RyString *)o)->value);
                } else if (o->type == BOOL_OBJ) {
                    printf("%s", ((RyBool *)o)->value ? "true" : "false");
                } else {
                    printf("<%s %p>", RyObjectTypeNames[o->type], o);
                }
                
                break;

            case OP_HALT:
                if (vm->sp < 1) runtime_error("stack underflow.", bytecode);
                o = vm->stack[--vm->sp];
                if (o->type == NUMBER_OBJ) {
                    if (((RyNumber *)o)->value == 0) exit(EXIT_SUCCESS);
                    exit(EXIT_FAILURE);
                } else {
                    exit(EXIT_FAILURE);
                }

            default:
                if (bytecode.opcode < N_OPCODES) runtime_error("opcode not implemented.", bytecode);
                printf("Runtime error : unrecognized opcode %d with argument %d generated at line %zu.\n", bytecode.opcode, bytecode.arg, bytecode.line);
                exit(1);
        }

        vm->current_func->ip++;
    }
}

void RyVMMakeFunction(RyVM *vm, size_t f_addr) {
    vm->main_save_ip = vm->current_func->ip;
    vm->current_func = RyObjectPoolBorrow_Function(&vm->object_pool);
    RyVariableArrayWrite(&vm->globals, f_addr, (RyObject *)vm->current_func);
    vm->record_only++;
}

void RyVMEndMakeFunction(RyVM *vm) {
    vm->current_func = (RyFunction *)RyVariableArrayGet(&vm->globals, 0);
    vm->record_only--;
    vm->current_func->ip = vm->main_save_ip;
}

void RyVMMakeControlFlow(RyVM *vm) {
    vm->record_only++;
}

void RyVMEndMakeControlFlow(RyVM *vm, size_t ip, size_t offset) {
    vm->current_func->block.code[ip].arg = offset;
    vm->record_only--;
}

void RyCollectGarbage(RyVM *vm) {

    // mark
    for(size_t i = 0; i < vm->globals.size; i++) {
        vm->globals.entries[i]->refc++;
    }

    for(size_t i = 0; i < vm->constants.size; i++) {
        vm->constants.entries[i]->refc++;
    }

    for(size_t i = 0; i < vm->current_func->locals.size; i++) {
        vm->current_func->locals.entries[i]->refc++;
    }

    for(size_t i = 0; i < vm->csp; i++) {
        for(size_t j = 0; j < vm->frame_call_stack[i].size; j++) {
            vm->frame_call_stack[i].entries[j]->refc++;
        }
    }

    for(size_t i = 0; i < vm->psp; i++) {
        vm->param_stack[i]->refc++;
    }

    for(size_t i = 0; i < vm->sp; i++) {
        vm->stack[i]->refc++;
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