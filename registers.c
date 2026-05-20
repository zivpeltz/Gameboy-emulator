#include "registers.h"
#include <stdint.h>

#define LOW_MASK 0xFF
#define HIGH_MASK 0xFF00

/* private global register box */
static uint16_t registers[6]; 
/* private global CPU state */
static int is_halted = 0;
static int ime = 0;
static int ime_delay = 0; // Tracks the EI instruction delay
static int is_stopped = 0;





void set_stop(int state) { is_stopped = state; }

int get_stop(void) { return is_stopped; }

void set_ime(int state) { ime = state; }

int get_ime(void) { return ime; }


/* Schedules IME to turn on after the NEXT instruction */
void schedule_ime(void) { ime_delay = 2; }

/* Called by main cpu_step() loop every cycle */
void update_ime_delay(void) {
    if (ime_delay > 0) {
        ime_delay--;
        if (ime_delay == 0) {
            ime = 1;
        }
    }
}

void set_halt(int state) { is_halted = state; }
int get_halt(void) { return is_halted; }

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

/* returns state of requested flag mask in flag register AF */
int get_flag(uint8_t flag_mask){ 
    uint8_t flagR = get8(REG_AF, 0);
    return (flagR & flag_mask) ? 1 : 0;
}

/* sets requested flag using the mask */
void set_flag(uint8_t flag_mask, int val){
    uint8_t flagR = get8(REG_AF, 0);
    if (val) {
        flagR |= flag_mask;  /* Set the bit */
    } else {
        flagR &= ~flag_mask; /* Clear the bit */
    }
    set8(REG_AF, 0, flagR);
}

