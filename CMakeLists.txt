cmake_minimum_required(VERSION 3.28)
project(rosy C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")

add_executable(rosy src/main.c

    src/memory_debug/memory_debug.h
    src/memory_debug/memory_debug.c
    src/arena/arena.h
    src/arena/arena.c

    src/lexer/token.h
    src/lexer/token.c
    src/lexer/lexer.h
    src/lexer/lexer.c

    src/parser/identifier_map.h
    src/parser/identifier_map.c
    src/parser/constant_map.h
    src/parser/constant_map.c
    src/parser/parser.h
    src/parser/parser.c

    src/vm/code/opcodes.h
    src/vm/code/code_block.h
    src/vm/code/code_block.c
    src/vm/gc/gc.h
    src/vm/gc/gc.c
    src/vm/object/object.h
    src/vm/object/object.c
    src/vm/object/object_pool.h
    src/vm/object/object_pool.c
    src/vm/object/operation.h
    src/vm/object/operation.c
    src/vm/variable/variable_array.h
    src/vm/variable/variable_array.c

    src/vm/vm.h
    src/vm/vm.c
)