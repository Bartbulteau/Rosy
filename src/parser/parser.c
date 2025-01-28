#include "parser.h"

void ParserInit(Parser *p, Lexer *lex, RyVM *vm) {
    p->lex = lex;
    p->vm = vm;

    ConstantMapInit(&p->cmap);
    IdentifierMapInit(&p->global_map);
    p->global_map.count = 1; // accounting for the main function
    IdentifierMapInit(&p->main_local_map);

    p->isInMain = true;
}

void ParserFree(Parser *p) {
    ConstantMapFree(&p->cmap);
    IdentifierMapFree(&p->global_map);
    IdentifierMapFree(&p->main_local_map);
}

Token parser_error(Token t, const char *msg) {
    printf("Parser error: [%d %s, l%zu] %s\n", t.type, t.lexeme, t.line, msg);
    exit(1);
}

bool p_isAtEnd(Parser *p) {
    return p->current.type == EOF_;
}

Token p_peek(Parser *p) {
    return p->current;
}

Token p_previous(Parser *p) {
    return p->previous;
}

Token p_advance(Parser *p) {
    if (!p_isAtEnd(p)) {
        p->previous = p->current;
        p->current = p->next;
        p->next = LexerGetNextToken(p->lex);
    }
    return p->previous;
}

bool p_check(Parser *p, TokenType type) {
    return p->current.type == type;
}

bool p_checkNext(Parser *p, TokenType type) {
    return p->next.type == type;
}

Token p_consume(Parser *p, TokenType type, char *msg) {
    if (p_check(p, type)) return p_advance(p);
    return parser_error(p_peek(p), msg);
}

bool p_match(Parser *p, size_t ntypes, ...) {
    va_list valist;
    va_start(valist, ntypes);

    for (size_t i = 0; i < ntypes; i++) {
        if (p_check(p, va_arg(valist, TokenType))) {
            p_advance(p);
            return true;
        }
    }

    va_end(valist);
    return false;
}

void execute(Parser *p) {
    p_advance(p);
    p_advance(p);
    while (!p_isAtEnd(p)) {
        p->isInMain = true;
        ParseGeneralDeclaration(p);
    }
}

void ParseDeclaration(Parser *p) {
    if (p_match(p, 1, VAR)) {
        ParseVarDecl(p);
    } else if (p_match(p, 1, GLOBAL)) {
        ParseGlobalVarDecl(p);
    } else if (p_match(p, 1, FUNC)) {
        parser_error(p_previous(p), "nested function declarations are not allowed.");
    } else if (p_match(p, 1, EOL)) {
        // do nothing
    } else {
        ParseStatement(p);
    }
}

void ParseGeneralDeclaration(Parser *p) {
    if (p_match(p, 1, VAR)) {
        ParseVarDecl(p);
    } else if (p_match(p, 1, GLOBAL)) {
        ParseGlobalVarDecl(p);
    } else if (p_match(p, 1, FUNC)) {
        ParseFunctionDecl(p);
    } else if (p_match(p, 1, EOL)) {
        // do nothing
    } else {
        ParseStatement(p);
    }
}

void ParseVarDecl(Parser *p) {
    Token idt = p_consume(p, IDENTIFIER, "expected an identifier name in variable declaration.");
    if (IdentifierMapContains(&p->global_map, idt.lexeme)) parser_error(idt, "identifier in variable declaration alreadfy exists in global scope.");
    size_t var_addr = p->isInMain ? IdentifierMapInsertUnique(&p->main_local_map, idt.lexeme) : IdentifierMapInsertUnique(&p->local_map, idt.lexeme);
    if (var_addr == SIZE_MAX) parser_error(idt, "double declaration of identifier.");
    if (p_match(p, 1, EQUAL)) {
        ParseExpression(p);
        RyVMExecute(p->vm, OP_STORE_LOCAL, var_addr, idt.line);
        RyVMExecute(p->vm, OP_POP, OP_NOP, idt.line);
        p_consume(p, EOL, "expected a new line or a ';' after variable declaration.");
    }
}

void ParseGlobalVarDecl(Parser *p) {
    Token idt = p_consume(p, IDENTIFIER, "expected an identifier name in global variable declaration.");
    size_t var_addr = IdentifierMapInsertUnique(&p->global_map, idt.lexeme);
    if (var_addr == SIZE_MAX) parser_error(idt, "double declaration of global identifier.");
    if (p_match(p, 1, EQUAL)) {
        ParseExpression(p);
        RyVMExecute(p->vm, OP_STORE_GLOBAL, var_addr, idt.line);
        RyVMExecute(p->vm, OP_POP, OP_NOP, idt.line);
        p_consume(p, EOL, "expected a new line or a ';' after variable declaration.");
    }
}

void ParseFunctionDecl(Parser *p) {
    p->isInMain = false;
    IdentifierMapInit(&p->local_map);
    ParseFunction(p);
    IdentifierMapFree(&p->local_map);
    p->isInMain = true;
}

void ParseStatement(Parser *p) {
    switch (p_peek(p).type) {
        case FOR: parser_error(p_peek(p), "for statements not implemented."); break;
        case WHILE: ParseWhileStatement(p); break;
        case IF: ParseIfStatement(p); break;
        case ELSE: parser_error(p_peek(p), "else statement should be preceeded by an if statement."); break;
        case EXIT: ParseExitStatement(p); break;
        case PRINT: ParsePrintStatement(p); break;
        case RETURN: ParseReturnStatement(p); break;
        case LEFT_BRACE: ParseBlock(p); break;
        default: ParseExpressionStmt(p); break;
    }
}

void ParseExpressionStmt(Parser *p) {
    ParseExpression(p);
    p_consume(p, EOL, "expected newline or ';' at the end of expression statement.");
    RyVMExecute(p->vm, OP_POP, OP_NOP, p_previous(p).line);
}

void ParsePrintStatement(Parser *p) {
    p_consume(p, PRINT, "internal error while parsing print statement.");
    p_consume(p, LEFT_PAREN, "expected a '(' after the print keyword.");
    ParseExpression(p);
    p_consume(p, RIGHT_PAREN, "expected a ')' to close the print statement.");
    p_consume(p, EOL, "expected a new line or a ';' after print statement.");
    RyVMExecute(p->vm, OP_PRINT, OP_NOP, p_previous(p).line);
}

void ParseReturnStatement(Parser *p) {
    p_consume(p, RETURN, "internal error while parsing return statement.");
    if (p_match(p, 1, EOL)) {
        RyVMExecute(p->vm, OP_RET, OP_NOP, p_previous(p).line);
    } else {
        ParseExpression(p);
        p_consume(p, EOL, "expected a new line or a ';' at the end of return statement.");
        RyVMExecute(p->vm, OP_RET, OP_NOP, p_previous(p).line);
    }
}

void ParseExpression(Parser *p) {
    ParseAssignment(p);
}

void ParseAssignment(Parser *p) {
    if (p_check(p, IDENTIFIER) && p_checkNext(p, EQUAL)) {
        Token idt = p_advance(p);
        p_advance(p);
        size_t var_addr = p->isInMain ? IdentifierMapGet(&p->main_local_map, idt.lexeme) : IdentifierMapGet(&p->local_map, idt.lexeme);
        if (var_addr == SIZE_MAX) {
            var_addr = IdentifierMapGet(&p->global_map, idt.lexeme);
            if (var_addr == SIZE_MAX) parser_error(idt, "use of undeclared identifier.");
            ParseAssignment(p);
            RyVMExecute(p->vm, OP_STORE_GLOBAL, var_addr, idt.line);
            return;
        }
        ParseAssignment(p);
        RyVMExecute(p->vm, OP_STORE_LOCAL, var_addr, idt.line);
    } else {
        ParseLogicOr(p);
    }
}

void ParseLogicOr(Parser *p) {
    ParseLogicAnd(p);
    while (p_match(p, 1, OR)) {
        ParseLogicAnd(p);
        RyVMExecute(p->vm, OP_BINARY_OP, OR_BINOP, p_peek(p).line);
    }
}

void ParseLogicAnd(Parser *p) {
    ParseEquality(p);
    while (p_match(p, 1, AND)) {
        ParseEquality(p);
        RyVMExecute(p->vm, OP_BINARY_OP, AND_BINOP, p_peek(p).line);
    }
}

void ParseEquality(Parser *p) {
    ParseComparison(p);
    while(p_match(p, 2, BANG_EQUAL, EQUAL_EQUAL)) {
        Token op = p_previous(p);
        ParseComparison(p);
        RyVMExecute(p->vm, OP_BINARY_OP, op.type == BANG_EQUAL ? NEQ_BINOP : EQ_BINOP, op.line);
    }
}

void ParseComparison(Parser *p) {
    ParseTerm(p);
    while(p_match(p, 4, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        Token op = p_previous(p);
        ParseTerm(p);
        switch(op.type) {
            case GREATER: RyVMExecute(p->vm, OP_BINARY_OP, GREAT_BINOP, op.type); break;
            case GREATER_EQUAL: RyVMExecute(p->vm, OP_BINARY_OP, GREQ_BINOP, op.type); break;
            case LESS: RyVMExecute(p->vm, OP_BINARY_OP, LESS_BINOP, op.type); break;
            case LESS_EQUAL: RyVMExecute(p->vm, OP_BINARY_OP, LEQ_BINOP, op.type); break;
            default : parser_error(op, "internal error while matching comparision token."); break;
        }
    }
}

void ParseTerm(Parser *p) {
    ParseFactor(p);
    while(p_match(p, 2, MINUS, PLUS)) {
        Token op = p_previous(p);
        ParseFactor(p);
        RyVMExecute(p->vm, OP_BINARY_OP, op.type == PLUS ? ADD_BINOP : SUB_BINOP, op.line);
    }
}

void ParseFactor(Parser *p) {
    ParseUnary(p);
    while (p_match(p, 3, SLASH, STAR, MOD)) {
        Token op = p_previous(p);
        ParseUnary(p);
        RyBinaryOpType op_type;
        if (op.type == STAR) {
            op_type = MUL_BINOP;
        } else if (op.type == SLASH) {
            op_type = DIV_BINOP;
        } else if (op.type == MOD) {
            op_type = MOD_BINOP;
        } else {
            parser_error(op, "invalid token encountered while parsing factor.");
        }

        RyVMExecute(p->vm, OP_BINARY_OP, op_type, op.line);
    }
}

void ParseUnary(Parser *p) {
    if (p_match(p, 2, BANG, MINUS)) {
        Token op = p_previous(p);
        ParseUnary(p);
        RyVMExecute(p->vm, OP_UNARY_OP, op.type == BANG ? NOT_UNOP : OPP_UNOP, op.line);
    } else {
        ParseCall(p);
    }
}

void ParseCall(Parser *p) {
    ParsePrimary(p);
    while (true) {
        if (p_match(p, 1, LEFT_PAREN)) {
            if (!p_check(p, RIGHT_PAREN)) {
                do {
                    ParseExpression(p);
                    RyVMExecute(p->vm, OP_LOAD_PARAM, OP_NOP, p_peek(p).line);
                } while (p_match(p, 1, COMMA));
            }
            p_consume(p, RIGHT_PAREN, "expected a ')' after function call.");
            RyVMExecute(p->vm, OP_CALL, OP_NOP, p_previous(p).line);
        } else {
            break;
        }
    }
}

void ParsePrimary(Parser *p) {
    if (p_match(p, 1, LEFT_PAREN)) {
        ParseExpression(p);
        p_consume(p, RIGHT_PAREN, "expected a ')' at the end of expression. Parenthesis mismatch.");
    } else {
        Token t = p_peek(p);
        size_t const_addr, var_addr, func_addr;
        switch(t.type) {
            case NUMBER:
            case NIL:
            case TRUE:
            case FALSE:
            case STRING:
                const_addr = ConstantMapInsert(&p->cmap, p->vm, t);
                RyVMExecute(p->vm, OP_LOAD_CONST, const_addr, t.line);
                break;
            case IDENTIFIER:
                var_addr = p->isInMain ? IdentifierMapGet(&p->main_local_map, t.lexeme) : IdentifierMapGet(&p->local_map, t.lexeme);
                func_addr = IdentifierMapGet(&p->global_map, t.lexeme);
                if (var_addr != SIZE_MAX) {
                    RyVMExecute(p->vm, OP_LOAD_LOCAL, var_addr, t.line);
                } else if (func_addr != SIZE_MAX) {
                    RyVMExecute(p->vm, OP_LOAD_GLOBAL, func_addr, t.line);
                } else {
                    parser_error(t, "use of undeclared identifier in primary expression.");
                }
                break;
            default:
                parser_error(t, "unexpected token while parsing primary expression.");
                break;
        }
        p_advance(p);
    }
}

void ParseBlock(Parser *p) {
    p_consume(p, LEFT_BRACE, "expected a '{' to start code block definition.");
    while (!p_check(p, RIGHT_BRACE)) {
        ParseDeclaration(p);
    }
    p_consume(p, RIGHT_BRACE, "expected a '}' to end code block definition.");
}

void ParseFunction(Parser *p) {
    size_t f_addr = IdentifierMapInsertUnique(&p->global_map, p_consume(p, IDENTIFIER, "expected an identifier name after 'fn' keyword in function declaration.").lexeme);
    RyVMMakeFunction(p->vm, f_addr);

    p_consume(p, LEFT_PAREN, "expected a '(' in function declaration : fn function_name(param1, param2, ...)");
    size_t nparams = 0;
    if (!p_check(p, RIGHT_PAREN)) {
        do {
            IdentifierMapInsertUnique(&p->local_map, p_consume(p, IDENTIFIER, "expected unqiue parameter name.").lexeme);
            nparams++;
        } while (p_match(p, 1, COMMA));
        for(size_t i = 0; i < nparams; i++) {
            RyVMExecute(p->vm, OP_STORE_PARAM, nparams - i - 1, p_previous(p).line);
        }
    }
    p_consume(p, RIGHT_PAREN, "expected a ')' after function declaration : fn function_name(param1, param2, ...)");
    ParseBlock(p);
    RyVMExecute(p->vm, OP_RET_NIL, OP_NOP, p_previous(p).line);
    RyVMEndMakeFunction(p->vm);
}

void ParseExitStatement(Parser *p) {
    p_consume(p, EXIT, "internal error while parsing exit statement.");
    p_consume(p, LEFT_PAREN, "expected a '(' after the exit keyword.");
    ParseExpression(p);
    p_consume(p, RIGHT_PAREN, "expected a ')' to close the exit statement.");
    p_consume(p, EOL, "expected a new line or a ';' after exit statement.");
    RyVMExecute(p->vm, OP_HALT, OP_NOP, p_previous(p).line);
}

void ParseIfStatement(Parser *p) {

    p_consume(p, IF, "internal error while parsing if statement.");
    p_consume(p, LEFT_PAREN, "expected a '(' after the if keyword.");
    ParseExpression(p);
    p_consume(p, RIGHT_PAREN, "expected a ')' to close the if statement.");

    size_t if_jmp = VAL2FUNC(p->vm->current_func)->block.size;
    RyVMMakeControlFlow(p->vm);
    RyVMExecute(p->vm, OP_JF, OP_NOP, p_previous(p).line);
    ParseStatement(p);
    size_t else_jmp = VAL2FUNC(p->vm->current_func)->block.size;
    RyVMExecute(p->vm, OP_JMP, OP_NOP, p_previous(p).line);
    size_t if_jmp_target = VAL2FUNC(p->vm->current_func)->block.size;
    RyVMEndMakeControlFlow(p->vm, if_jmp, if_jmp_target - if_jmp - 1);

    RyVMMakeControlFlow(p->vm);
    if(p_match(p, 1, ELSE)) {
        ParseStatement(p);
    }
    size_t else_jmp_target = VAL2FUNC(p->vm->current_func)->block.size;
    RyVMEndMakeControlFlow(p->vm, else_jmp, else_jmp_target - else_jmp - 1);
    RyVMExecute(p->vm, OP_NOP, OP_NOP, p_previous(p).line);
}

void ParseWhileStatement(Parser *p) {
    p_consume(p, WHILE, "internal error while parsing while statement.");
    p_consume(p, LEFT_PAREN, "expected a '(' after the while keyword.");
    RyVMMakeControlFlow(p->vm);
    size_t condition_jmp = VAL2FUNC(p->vm->current_func)->block.size;
    ParseExpression(p);
    p_consume(p, RIGHT_PAREN, "expected a ')' to close the while statement.");
    size_t while_jmp = VAL2FUNC(p->vm->current_func)->block.size;
    RyVMExecute(p->vm, OP_JF, OP_NOP, p_previous(p).line);
    ParseStatement(p);
    RyVMExecute(p->vm, OP_JB, VAL2FUNC(p->vm->current_func)->block.size - condition_jmp + 1, p_previous(p).line);
    size_t while_jmp_target = VAL2FUNC(p->vm->current_func)->block.size;
    RyVMEndMakeControlFlow(p->vm, while_jmp, while_jmp_target - while_jmp - 1);
    RyVMExecute(p->vm, OP_NOP, OP_NOP, p_previous(p).line);
}