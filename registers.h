#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

/* register box indices using enums for efficient access */
typedef enum {
    REG_BC = 0,
    REG_DE = 1,
    REG_HL = 2,
    REG_SP = 3,
    REG_AF = 4,
    REG_PC = 5
} RegisterIndex;

/* 8-bit register operations */
uint8_t get8(RegisterIndex reg, int high);
void set8(RegisterIndex reg, int high, uint8_t value);

/* 16-bit register operations */
uint16_t get16(RegisterIndex reg);
void set16(RegisterIndex reg, uint16_t val);

/* flag operations */
int get_flag(void); 

#endif // REGISTERS_H