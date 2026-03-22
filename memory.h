#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

// The only two functions that may need to be called outside of memory.
uint8_t memory_read(uint16_t address);
void memory_write(uint16_t address, uint8_t value);

#endif // MEMORY_H