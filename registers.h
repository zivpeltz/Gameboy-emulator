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

/* Flag Bitmasks for the F Register (Lower 8 bits of AF) */
#define FLAG_Z (1 << 7)
#define FLAG_N (1 << 6)
#define FLAG_H (1 << 5)
#define FLAG_C (1 << 4)

/* 8-bit register operations */
uint8_t get8(RegisterIndex reg, int high);
void set8(RegisterIndex reg, int high, uint8_t value);

/* 16-bit register operations */
uint16_t get16(RegisterIndex reg);
void set16(RegisterIndex reg, uint16_t val);

/* flag operations */
int get_flag(uint8_t flag_mask);
void set_flag(uint8_t flag_mask, int val);

/* CPU State Operations */
void set_halt(int state);
int get_halt(void);

void set_stop(int state);
int get_stop(void);

void set_ime(int state);
int get_ime(void);

void schedule_ime(void);
void update_ime_delay(void);

#endif // REGISTERS_H