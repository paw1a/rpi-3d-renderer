#include "allocator.h"

#define BUFFER_SIZE (80 * 1024)
static uint8_t buffer[BUFFER_SIZE];
static uint8_t *base = buffer;
static uint8_t *limit = buffer + BUFFER_SIZE;
static uint8_t *marker = base;

uint8_t *allocate(uint32_t size) {
    if (marker + size > limit) {
        return nullptr;
    }

    uint8_t *ptr = marker;
    marker += size;

    return ptr;
}

void reset_allocator(void) {
    marker = base;
}
