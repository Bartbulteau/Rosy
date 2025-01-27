#include "arena.h"

bool is_power_of_two(const uintptr_t x) {
    return (x & (x-1)) == 0;
}

uintptr_t align_forward(const uintptr_t ptr, const size_t align) {
    assert(is_power_of_two(align));

    uintptr_t p = ptr;
    const uintptr_t a = (uintptr_t) align;
    const uintptr_t modulo = p & (a - 1);
    if (modulo != 0) {
        p += a - modulo;
    }
    return p;
}

ArenaRegion *ArenaRegion_New(const size_t buff_len) {

    ArenaRegion *region = (ArenaRegion *) MALLOC(sizeof(ArenaRegion));
    assert(region);
    region->buff = (unsigned char *) MALLOC(buff_len);
    assert(region->buff);

    region->next = NULL;
    region->buff_len = buff_len;
    region->prev_offset = 0;
    region->curr_offset = 0;

    return region;
}

void ArenaRegion_Reset(ArenaRegion *r) {
    r->curr_offset= 0;
    r->prev_offset = 0;
}

void ArenaRegion_Free(ArenaRegion *r) {
    FREE(r->buff);
    FREE(r);
}

void * ArenaRegion_AllocAlign(ArenaRegion *r, size_t size, const size_t align) {
    const uintptr_t curr_ptr = (uintptr_t) r->buff + (uintptr_t) r->curr_offset;
    uintptr_t offset = align_forward(curr_ptr, align);
    offset -= (uintptr_t) r->buff;

    if (offset + size <= r->buff_len) {
        void *ptr = &r->buff[offset];
        r->prev_offset = offset;
        r->curr_offset = offset + size;

        memset(ptr, 0, size);
        return ptr;
    }

    return NULL;
}

void *ArenaRegion_Alloc(ArenaRegion *r, const size_t size) {
    return ArenaRegion_AllocAlign(r, size, ARENA_DEFAULT_ALIGNMENT);
}

void ArenaInit(Arena *a, const size_t default_region_size) {
    a->regions = NULL;
    a->default_region_size = default_region_size;
}

void *ArenaAlloc(Arena *a, const size_t size) {
    for (ArenaRegion *r = a->regions; r != NULL; r = r->next) {
        void *ptr = ArenaRegion_AllocAlign(r, size, ARENA_DEFAULT_ALIGNMENT);
        if (ptr != NULL) return ptr;
    }

    size_t region_size = size > a->default_region_size ? size : a->default_region_size;
    ArenaRegion *new_region = ArenaRegion_New(region_size);
    void *ptr = ArenaRegion_AllocAlign(new_region, size, ARENA_DEFAULT_ALIGNMENT);
    new_region->next = a->regions;
    a->regions = new_region;
    return ptr;
}

void *ArenaRealloc(Arena *a, void *old_ptr, const size_t old_size, const size_t new_size) {
    if (new_size <= old_size) return old_ptr;
    return ArenaAlloc(a, new_size);
}

void ArenaReset(Arena *a) {
    for (ArenaRegion *r = a->regions; r != NULL; r = r->next) {
        ArenaRegion_Reset(r);
    }
}

void ArenaFree(Arena *a) {
    #ifdef MEMORY_DEBUG
        // ArenaPrint(a);
    #endif 
    ArenaRegion *r = a->regions;
    while (r != NULL) {
        ArenaRegion *tmp = r;
        r = r->next;
        ArenaRegion_Free(tmp);
    }
}

void ArenaPrint(Arena *a) {
    size_t i = 1;
    size_t total_size = 0;
    size_t total_occupied = 0;
    fprintf(stderr, "=================================================\n");
    fprintf(stderr, "=================================================\n");
    fprintf(stderr, "||                Arena summary                ||\n");
    fprintf(stderr, "=================================================\n");
    fprintf(stderr, "=================================================\n");
    for (ArenaRegion *r = a->regions; r != NULL; r = r->next) {
        fprintf(stderr, "Region n°\t\t%zu\n", i);
        fprintf(stderr, "Capacity: \t\t%.4f KB\n", r->buff_len/1024.0);
        fprintf(stderr, "Occupied: \t\t%.4f KB\n", r->curr_offset/1024.0);
        fprintf(stderr, "=================================================\n");
        i++;
        total_size += r->buff_len;
        total_occupied += r->curr_offset;
    }
    fprintf(stderr, "=================================================\n");
    fprintf(stderr, "Total size:\t\t%.4f KB\n", total_size/1024.0);
    fprintf(stderr, "Total occupied:\t\t%.4f KB\n", total_occupied/1024.0);
    fprintf(stderr, "=================================================\n");
    fprintf(stderr, "=================================================\n\n");
}
