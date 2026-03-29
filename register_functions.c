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


