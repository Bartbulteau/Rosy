#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "../memory_debug/memory_debug.h"

#define ARENA_DEFAULT_ALIGNMENT (2*sizeof(void*))
#define ARENA_KB(n) (size_t)(n*1024)
#define ARENA_MB(n) (size_t)(n*1024*1024)
#define ARENA_GB(n) (size_t)(n*1024*1024*1024)

bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);

typedef struct ArenaRegion_t {
    struct ArenaRegion_t* next;
    unsigned char *buff;
    size_t buff_len;
    size_t prev_offset;
    size_t curr_offset;
} ArenaRegion;

ArenaRegion *ArenaRegion_New(size_t buff_len);
void ArenaRegion_Reset(ArenaRegion *r);
void ArenaRegion_Free(ArenaRegion *r);

void *ArenaRegion_AllocAlign(ArenaRegion *r, size_t size, size_t align);
void *ArenaRegion_Alloc(ArenaRegion *r, size_t size);

typedef struct Arena_t {
    ArenaRegion* regions;
    size_t default_region_size;
} Arena;

void ArenaInit(Arena *a, size_t default_region_size);

void *ArenaAlloc(Arena *a, size_t size);
void *ArenaRealloc(Arena *a, void *old_ptr, size_t old_size, size_t new_size);
void ArenaReset(Arena *a);
void ArenaFree(Arena *a);
void ArenaPrint(Arena *a);

#endif //ARENA_H