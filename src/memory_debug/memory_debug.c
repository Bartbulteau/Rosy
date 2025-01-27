#include "memory_debug.h"

#ifdef MEMORY_DEBUG

void* debug_malloc(const size_t size, const char *file, const int line, const char *func) {

    void *p = malloc(size);

    MemoryLog *log = (MemoryLog*)malloc(sizeof(MemoryLog));
    assert(log);
    log->next = memory_log;
    log->ptr = p;

    memory_log = log;

    snprintf(memory_log->log, sizeof(memory_log->log), "Allocated = %s, %i, %s, %p[%li]\n", file, line, func, p, size);
    malloc_call_count++;

    return p;
}

void *debug_realloc(void *ptr, size_t size, const char *file, int line, const char *func) {
    if (ptr == NULL) {
        void *p = realloc(ptr, size);

        MemoryLog *log = (MemoryLog*)malloc(sizeof(MemoryLog));
        assert(log);
        log->next = memory_log;
        log->ptr = p;

        memory_log = log;

        snprintf(memory_log->log, sizeof(memory_log->log), "Allocated = %s, %i, %s, %p[%li]\n", file, line, func, p, size);
        malloc_call_count++;

        return p;
    }
    
    void *p = realloc(ptr, size);

    MemoryLog *log = memory_log;
    while(log != NULL) {
        if (log->ptr == ptr) {
            log->ptr = p;
            snprintf(log->log, sizeof(log->log), "Allocated = %s, %i, %s, %p[%li]\n", file, line, func, p, size);
            break;
        }
        log = log->next;
    }

    return p;
}

void debug_free(void *ptr)
{
    if (memory_log == NULL) fprintf(stderr, "Seems like freeing with no memory allocated...\n");
    else {
        if (memory_log->ptr == ptr) {
            memory_log = memory_log->next;
        } else {
            MemoryLog *curr = memory_log;
            while (curr->next != NULL) {
                if(curr->next->ptr == ptr) {
                    curr->next = curr->next->next;
                    break;
                }
                curr = curr->next;
            }
        }
    }

    free(ptr);
}

void MemoryDebug() {
    if (memory_log == NULL) {
        fprintf(stderr, "\n\nNo memory leaks found.\n");
        fprintf(stderr, "Total number of calls to malloc : %zu.\n", malloc_call_count);
        return;
    }

    fprintf(stderr, "\n\n/!\\ Memory leak detected:\n");
    MemoryLog *curr = memory_log;
    while(curr != NULL) {
        fprintf(stderr, "%s\n", curr->log);
        curr = curr->next;
    }
    fprintf(stderr, "\n\n");
    fprintf(stderr, "Total number of calls to malloc : %zu.\n", malloc_call_count);
}

#else // MEMORY_DEBUG

void MemoryDebug() {}

#endif // MEMORY_DEBUG