#include "registers.h"
#include "memory.h"


/* 
-number returned by every function is the amount of CPU cycles the simulated run of that function takes 

-this section here is basically "core" functions, i.e RISC like operations. more complicated opcodes would be
implemented as pseudo instructions of these core operations.
*/

/*================== 8-BIT LOAD OPERATIONS =======================*/
/* register to register 8-bit LD function*/
int load_R2R(RegisterIndex src, int src_high, RegisterIndex dst, int dst_high) {
    set8(dst, dst_high, get8(src,src_high));
    return 4; 
}

/* immediate to register 8-bit LD function*/
int load_I2R(RegisterIndex dst, int dst_high, uint8_t value) {
    set8(dst, dst_high, value);
    return 8; 
}

/* memory to/from register 8-bit LD function */
int load_MR(RegisterIndex addressreg, RegisterIndex reg, int high,int M2R){
    uint16_t address = get16(addressreg);
    if(M2R) set8(reg, high, memory_read(address));
    else memory_write(address, get8(reg,high));
    return 8;
}

/* load 8-bit immediate to memory address */
int load_MI(RegisterIndex addressreg, uint8_t value){
    uint16_t address = get16(addressreg);
    memory_write(address,value);
    return 12;
}

/* load from 16-bit memory address to 8-bit register */
int load_abs2R(RegisterIndex reg, int high, uint16_t address) {
    set8(reg, high, memory_read(address));
    return 16;
}

/* load from 8-bit register to 16-bit memory address */
int load_R2abs(uint16_t address, RegisterIndex reg, int high) {
    memory_write(address, get8(reg, high));
    return 16;
}

/*================== 16-BIT LOAD OPERATIONS =======================*/

/* register to register 16-bit LD function*/
int load_16_R2R(RegisterIndex src, RegisterIndex dst) {
    set16(dst, get16(src));
    return 4; 
}

/* immediate to register 16-bit LD function*/
int load_16_I2R(RegisterIndex dst, uint16_t value){
    set16(dst, value);
    return 12; 
}


/*================== ARITHMETIC OPERATIONS =======================*/

/* helper function, checks if the result of an addition creates a carry from bit 7 */
int check_for_C_flag(uint16_t val) {
    // Isolate bit 8 (0x0100) and shift it down to return 1 or 0
    return (val & 0x0100) >> 8; 
}

/* helper function, checks if the addition of two 8-bit values creates a half-carry */
int check_for_H_flag(uint8_t a, uint8_t b) {
    // Isolate the lower 4 bits of both operands, add them, and check if bit 4 is set
    return (((a & 0x0F) + (b & 0x0F)) & 0x10) >> 4; 
}

/* Core 8-bit ADD logic. Handles the math and flag setting, completely agnostic of operand source. */
void alu_add8(RegisterIndex dst, uint8_t value, int high) {
    uint8_t curr = get8(dst, high);
    uint16_t result = (uint16_t)curr + (uint16_t)value; 

    set8(dst, high, (uint8_t)result);
    
    /*  Z: Set if the 8-bit result is exactly 0 */
    set_flag('Z', ((uint8_t)result == 0) ? 1 : 0);
    /* N: Always reset for ADD */
    set_flag('N', 0);
    /*  H: Requires the original operands to calculate the lower-nibble overflow */
    set_flag('H', check_for_H_flag(curr, value));
    /* C: Check the 16-bit result for an overflow past 8 bits */
    set_flag('C', check_for_C_flag(result));
}

/* ADD operation from immediate/memory to an 8-bit register */
int add_I2R(RegisterIndex dst, uint8_t value, int high) {
    alu_add8(dst,value,high);
    return 4; 
}

/* ADD operation from memory to an 8-bit register */
int add_M2R(RegisterIndex dst, RegisterIndex addressreg , int high){
    uint16_t address = get16(addressreg);
    uint8_t value = memory_read(address);

    alu_add8(dst, value, high);

    return 8;
}

/* ADD operation from 8-bit register to an 8-bit register */
int add_R2R(RegisterIndex dst, RegisterIndex src , int dsthigh, int srchigh){
    uint8_t value = get8(src,srchigh);
    alu_add8(dst, value, dsthigh);

    return 4;
}







/*================== STACK POINTER OPERATIONS =======================*/
/* pushes 16-bit register onto stack */
int PUSH(RegisterIndex src){
 /*TODO*/
}