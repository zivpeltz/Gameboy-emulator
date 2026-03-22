#include "registers.h"
#include <stdint.h>

#define LOW_MASK 0xFF
#define HIGH_MASK 0xFF00


/* private global register box */
static uint16_t registers[6]; 


/*8-bit register operations*/
uint8_t get8(RegisterIndex reg, int high) {
    uint16_t value = registers[reg];
    return high ? (uint8_t)(value >> 8) : (uint8_t)(value & 0xFF);
}

void set8(RegisterIndex reg, int high, uint8_t value) {
    if (high) registers[reg] = (registers[reg] & LOW_MASK) | ( ((uint16_t) value) << 8);
    else registers[reg] = (registers[reg] & HIGH_MASK) | (uint16_t) value;
}


/* 16-bit register operations */
uint16_t get16(RegisterIndex reg) { return registers[reg];}

void set16(RegisterIndex reg, uint16_t val) { registers[reg] = val;}

/* flag operations */

int get_flag(){ /* to be implemented */
    return 0;
}