#ifndef MEMORY_DEBUG_H
#define MEMORY_DEBUG_H

//#define MEMORY_DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef MEMORY_DEBUG

#define MEMORY_CHECK(ptr) if (ptr == NULL) {printf("Ran out of memory.\n"); exit(EXIT_FAILURE);}
#define MALLOC(size) malloc(size)
#define REALLOC(ptr, size) realloc(ptr, size)
#define FREE(ptr) free(ptr)
void MemoryDebug();

#else // MEMORY_DEBUG

typedef struct MemoryLog_t {
    struct MemoryLog_t* next;
    char log[200];
    void *ptr;
} MemoryLog;

static MemoryLog *memory_log = NULL;
static size_t malloc_call_count = 0;

void* debug_malloc(size_t size, const char *file, int line, const char *func);
void* debug_realloc(void *ptr, size_t size, const char *file, int line, const char *func);
void debug_free(void *ptr);
void MemoryDebug();

#define MALLOC(size) debug_malloc(size, __FILE__, __LINE__, __FUNCTION__)
#define REALLOC(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#define FREE(ptr) debug_free(ptr)

#endif // MEMORY_DEBUG

#endif // MEMORY_DEBUG_H