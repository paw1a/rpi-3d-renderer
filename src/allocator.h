#pragma once

#include <cstdint>

void init_allocator(void);
uint8_t *allocate(uint32_t size);
void reset_allocator(void);
