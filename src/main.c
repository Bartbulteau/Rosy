#include <stdio.h>
#include "arena/arena.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"
#include "stdlib/stdlib.h"

extern const char *stdlib_code;

int main(int argc, const char *argv[]) {
    /*
        ARGUMENTS CHECK
    */
    
    if (argc != 2) {
        printf("Usage: %s <input_filename>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }

    /*
        READING SOURCE FILE
    */

    fseek(file, 0, SEEK_END);
    const long length = ftell(file);

    fseek(file, 0, SEEK_SET);
    char *code = (char *)MALLOC(length);

    if (code == NULL) {
        printf("Error allocating memory for source file buffer.\n");
        return 1;
    }
    fread(code, 1, length, file);
    fclose(file);

    /*
        INCLUDING STDLIB
    */

    size_t full_code_length = strlen(code) + strlen(stdlib_code);
    char *full_code = (char *)malloc(full_code_length + 1);
    strcpy(full_code, stdlib_code);
    strcat(full_code, code);
    FREE(code);

    /*
        EXECUTION
    */

    Lexer lex;
    LexerInit(&lex, full_code, full_code_length);

    RyVM vm;
    RyVMInit(&vm);

    Parser p;
    ParserInit(&p, &lex, &vm);

    execute(&p);
    
    RyVMFree(&vm);
    LexerFree(&lex);
    ParserFree(&p);
    FREE(full_code);


    MemoryDebug();
    return 0;
}