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
int flag_idx(char F){
      switch (F) {
        case 'C':
            return 4;
        case 'H':
            return 5;
        case 'N':
            return 6;
        case 'Z':
            return 7;
        default:
            return 0;
    }
}

/* returns state of requested flag in flag register AF, returns -1 for invalid input */
int get_flag(char F){ 
    uint8_t flagR = get8(REG_AF,0);
    int idx = flag_idx(F);
    if (!idx) return -1;
    return (flagR >> idx) & 1;
}

/* sets requested flag value, return -1 on invalid input and 1 on successful operation*/
int set_flag(char F, int val){
    if (val > 1 || val < 0) return -1; /* val should be 0 or 1 */

    uint8_t flagR = get8(REG_AF,0);
    int idx = flag_idx(F);

    if (!idx) return -1; 

    flagR = (~(1 << idx)) & flagR;
    set8(REG_AF,0,(flagR | (val << idx)));
    return 1;
}

