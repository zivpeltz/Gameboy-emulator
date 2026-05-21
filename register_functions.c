#include "registers.h"
#include "memory.h"


#define SWAP_NIBBLES(x) ((((x) >> 4) & 0x0F) | (((x) << 4) & 0xF0))
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

/* Core 8-bit ALU logic. 
 * sub: 1 for SUB/SBC, 0 for ADD/ADC
 * use_carry: 1 for ADC/SBC, 0 for standard ADD/SUB 
 * set_res: 1 for all operations but CP
 */
void ALU8(RegisterIndex dst, 
    uint8_t value, 
    int high, 
    int sub, 
    int use_carry, 
    int set_res) 
    {
    uint8_t curr = get8(dst, high);
    
    /* Fetch the incoming carry state (1 or 0) ONLY if this is an ADC/SBC instruction */
    int carry_in = (use_carry && (get_flag(FLAG_C) == 1)) ? 1 : 0;

    int result;
    int h_flag, c_flag;

    if (sub) {
        /*Mathematical result of SUB / SBC */
        result = curr - value - carry_in;
        
        /* Half-borrow: lower nibble drops below 0 */
        h_flag = ((curr & 0x0F) - (value & 0x0F) - carry_in) < 0;
        
        /* Full-borrow: full byte drops below 0 */
        c_flag = result < 0;
    } else {
        /* Mathematical result of ADD / ADC */
        result = curr + value + carry_in;
        
        /* Half-carry: lower nibble exceeds 15 */
        h_flag = ((curr & 0x0F) + (value & 0x0F) + carry_in) > 0x0F;
        
        /* Full-carry: full byte exceeds 255 */
        c_flag = result > 0xFF;
    }

    /* Apply the truncated 8-bit result */
    if (set_res) set8(dst, high, (uint8_t)result);
    
    
    /* Set Flags */
    set_flag(FLAG_Z, ((uint8_t)result == 0) ? 1 : 0);
    set_flag(FLAG_N, sub);
    set_flag(FLAG_H, h_flag ? 1 : 0);
    set_flag(FLAG_C, c_flag ? 1 : 0);
}

/* ALU operation from immediate/memory to an 8-bit register */
int ALU8_I2R(RegisterIndex dst, uint8_t value, int high , int sub) {
    ALU8(dst,value,high,sub,0,1);
    return 8; 
}

/* ALU operation from memory to an 8-bit register */
int ALU8_M2R(RegisterIndex dst, RegisterIndex addressreg , int high , int sub){
    uint16_t address = get16(addressreg);
    uint8_t value = memory_read(address);

    ALU8(dst, value, high, sub,0,1);

    return 8;
}

/* ALU operation from 8-bit register to an 8-bit register */
int ALU8_R2R(RegisterIndex dst, RegisterIndex src , int dsthigh, int srchigh, int sub){
    uint8_t value = get8(src,srchigh);
    ALU8(dst, value, dsthigh, sub,0,1);

    return 4;
}

/* Adds the value of an 8-bit register and the Carry Flag to A. */
int ADC_R2R(RegisterIndex src, int srchigh) {
    uint8_t value = get8(src, srchigh);
    ALU8(REG_AF, value, 1, 0, 1, 1); 
    
    return 4; 
}

/* Adds an immediate 8-bit value and the Carry Flag to A. */
int ADC_I2R(uint8_t immediate_value) {
    ALU8(REG_AF, immediate_value, 1, 0, 1, 1); 

    return 8; 
}

/* Subtracts the value of an 8-bit register and the Carry Flag from A */
int SBC_R2R(RegisterIndex src, int srchigh) {
    uint8_t value = get8(src, srchigh);
    ALU8(REG_AF, value, 1, 1, 1, 1); 
    
    return 4; 
}

/* Subtracts an immediate 8-bit value and the Carry Flag from A  */
int SBC_I2R(uint8_t immediate_value) {
    ALU8(REG_AF, immediate_value, 1, 1, 1, 1); 
    
    return 8; 
}

/* Performs bitwise AND between A and a register */
int AND_r(RegisterIndex src, int high) {
    uint8_t a = get8(REG_AF, 1);
    uint8_t val = get8(src, high);
    
    a &= val;
    set8(REG_AF, 1, a);
    
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 1); /* Hardware quirk: AND always sets H to 1 */
    set_flag(FLAG_C, 0);
    
    return 4;
}

/* Performs bitwise OR between A and a register */
int OR_r(RegisterIndex src, int high) {
    uint8_t a = get8(REG_AF, 1);
    uint8_t val = get8(src, high);
    
    a |= val;
    set8(REG_AF, 1, a);
    
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, 0);
    
    return 4;
}

/* Performs bitwise XOR between A and a register */
int XOR_r(RegisterIndex src, int high) {
    uint8_t a = get8(REG_AF, 1);
    uint8_t val = get8(src, high);
    
    a ^= val;
    set8(REG_AF, 1, a);
    
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, 0);
    
    return 4;
}

/* Performs bitwise AND between A and memory at (HL) */
int AND_m() {
    uint8_t a = get8(REG_AF, 1);
    uint8_t val = memory_read(get16(REG_HL));
    
    a &= val;
    set8(REG_AF, 1, a);
    
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 1);
    set_flag(FLAG_C, 0);
    
    return 8;
}

/* Performs bitwise OR between A and memory at (HL) */
int OR_m() {
    uint8_t a = get8(REG_AF, 1);
    uint8_t val = memory_read(get16(REG_HL));
    
    a |= val;
    set8(REG_AF, 1, a);
    
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, 0);
    
    return 8;
}

/* Performs bitwise XOR between A and memory at (HL) */
int XOR_m() {
    uint8_t a = get8(REG_AF, 1);
    uint8_t val = memory_read(get16(REG_HL));
    
    a ^= val;
    set8(REG_AF, 1, a);
    
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, 0);
    
    return 8;
}

/* compares A with register src and sets flags (result not saved )*/
int CP_R(RegisterIndex src, int high){
    uint8_t value = get8(src,high);
    ALU8(REG_AF , value, 1, 1 , 0 , 0);
    return 4;
}

/* compares A with value saved at (HL) address and sets flags (result not saved )*/
int CP_M(){
    uint8_t value = memory_read(get16(REG_HL));
    ALU8(REG_AF , value, 1, 1 , 0 , 0);
    return 8;
}

/* performes 8-bit INC operation on register src */
int INC_DEC_8_R(RegisterIndex src, int high, int dec) {
    int curr_carry = get_flag(FLAG_C); /* extract to restore after using ALU8 */
    ALU8(src, 1 , high, dec, 0, 1);
    set_flag(FLAG_C, curr_carry); /* restore since ALU8 may have destoryed prev value*/

    return 4;
}

/* performs 8-bit INC/DEC operation on memory address (HL) */
int INC_DEC_8_M(int dec) {
    uint16_t address = get16(REG_HL);
    uint8_t curr = memory_read(address);
    
    uint8_t result = dec ? (curr - 1) : (curr + 1);
    
    memory_write(address, result);
    
    set_flag(FLAG_Z, (result == 0) ? 1 : 0);
    set_flag(FLAG_N, dec);
    
    if (dec) {
        /* Half-borrow check for decrement */
        set_flag(FLAG_H, ((curr & 0x0F) == 0x00) ? 1 : 0);
    } else {
        /* Half-carry check for increment */
        set_flag(FLAG_H, ((curr & 0x0F) == 0x0F) ? 1 : 0);
    }
    

    return 12;
}

/* Core 16-bit ALU logic, only does addition since there is no SUB for 16-bit  */
void ALU16(RegisterIndex dst, uint16_t value){
    uint16_t curr = get16(dst);

    /* Cast to 32-bit to easily catch the 16-bit overflow */
    uint32_t result = (uint32_t)curr + (uint32_t)value;
    
    set_flag(FLAG_N, 0);

    /* H: Set if there is a carry from bit 11. Mask the lowest 12 bits */
    set_flag(FLAG_H, (((curr & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF) ? 1 : 0);
    
    /* C: Set if there is a carry out of bit 15 (result exceeds 16 bits) */
    set_flag(FLAG_C, (result > 0xFFFF) ? 1 : 0);

    set16(dst,(uint16_t)result);

}

/* ALU ADD operation from 16-bit register to an 16-bit register*/
int ADD_16_R(RegisterIndex dst, RegisterIndex src){
    uint16_t value = get16(src);
    ALU16(dst,value);

    return 8;
}

/* Handles ADD SP, e (where 'e' is a signed 1-byte immediate) */
int ADD_16_SP_OFFSET(uint8_t imm) {
    /* Force the unsigned byte into a signed 8-bit integer */
    int8_t offset = (int8_t)imm; 
    
    uint16_t sp = get16(REG_SP);
    
    uint16_t result = sp + offset;
    
    set16(REG_SP, result);

    /* Flags are evaluated using strictly 8-bit logic rules */
    set_flag(FLAG_Z, 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, (((sp & 0x0F) + (offset & 0x0F)) > 0x0F) ? 1 : 0);
    set_flag(FLAG_C, (((sp & 0xFF) + (offset & 0xFF)) > 0xFF) ? 1 : 0);
    
    return 16;
}


/* 16-bit register increment or decrement*/
int INC_DEC_16(RegisterIndex src, int dec) {
    uint16_t curr = get16(src);
    curr = (dec == 0) ? curr + 1 : curr - 1;
    set16(src ,curr);
    return 8;
}
/*================== MISCELLANEOUS OPERATIONS =======================*/

/* swaps the nibbles of a value in an 8-bit register */
int SWAP_R(RegisterIndex src, int high){
    uint8_t value = get8(src,high);
    value = SWAP_NIBBLES(value);
    set8(src,high, value);
    (value == 0) ? set_flag(FLAG_Z, 1) : set_flag(FLAG_Z, 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, 0);
    return 8;
}

/* swaps the nibbles of a value in (HL) */
int SWAP_M(){
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);
    value = SWAP_NIBBLES(value);
    memory_write(addr,value);
    (value == 0) ? set_flag(FLAG_Z, 1) : set_flag(FLAG_Z, 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, 0);
    return 16;
}

/* Decimal Adjust Accumulator 
 * Corrects the A register into a valid BCD format after an arithmetic operation.
 */
int DAA(){
    uint8_t a = get8(REG_AF, 1); /* get A */
    
    int n_flag = get_flag(FLAG_N);
    int h_flag = get_flag(FLAG_H);
    int c_flag = get_flag(FLAG_C);

    int correction = 0;
    int set_carry = 0; /* Tracks if we need to set the C flag at the end */

    if (n_flag == 0) {
        /*  PREVIOUS OPERATION WAS ADDITION  */
        /* If the lower nibble overflowed the 4-bit boundary (H flag) */
        /* OR if it just mathematically exceeded 9, we must correct the ones column. */
        if (h_flag || (a & 0x0F) > 0x09) {
            correction |= 0x06;
        }
        
        /* If the upper nibble overflowed the 8-bit boundary (C flag) */
        /* OR if it mathematically exceeded 9, we must correct the tens column. */
        if (c_flag || a > 0x99) {
            correction |= 0x60;
            set_carry = 1; 
        }
        
        a += correction; /* Apply positive correction */
        
    } else {
        /* PREVIOUS OPERATION WAS SUBTRACTION */
        
        /* If a half-borrow occurred, the lower nibble is broken, correct it. */
        if (h_flag) {
            correction |= 0x06;
        }
        
        /* If a full borrow occurred, the upper nibble is broken, correct it. */
        if (c_flag) {
            correction |= 0x60;
            set_carry = 1; /* Preserve the carry flag */
        }
        
        a -= correction; /* Apply negative correction */
    }

    /* Apply the corrected value back to A */
    set8(REG_AF, 1, a);

    /* Update Flags */
    set_flag(FLAG_Z, (a == 0) ? 1 : 0);
    set_flag(FLAG_H, 0); /* DAA always hard-resets the H flag to 0 */
    
    /* N flag is STRICTLY PRESERVED. Do not touch it. */
    
    /* C flag is set if the correction pushed it over the edge, otherwise it retains its old state if it was already 1. */
    if (set_carry) {
        set_flag(FLAG_C, 1);
    }
    /* Note: If set_carry is 0, we DO NOT reset C to 0. It must stay 1 if it was already 1 before DAA. */

    return 4; // DAA takes 4 clock cycles
}

/* flips all of A's bits */
int CPL(){
    uint8_t value = get8(REG_AF,1);
    value = ~value;
    set8(REG_AF,1,value);

    set_flag(FLAG_N, 1);
    set_flag(FLAG_H, 1);

    return 4;
}

/* complements carry flag */
int CCF(){
    int curr = get_flag(FLAG_C);
    set_flag(FLAG_C, !curr);

    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);

    return 4;

}

/* sets carry flag */
int SCF(){
    set_flag(FLAG_C, 1);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);

    return 4;

}
/* does nothing */
int NOP(){
    return 4;
}

/* Suspends CPU execution until an interrupt occurs */
int HALT() {
    set_halt(1);
    return 4;
}

/* Disables all interrupts by resetting the IME flag */
int DI() {
    set_ime(0);
    return 4; 
}

/* Enables interrupts, but with a 1-instruction delay */
int EI() {
    schedule_ime();
    return 4;
}

/* Halts the CPU and LCD oscillator until a Joypad press */
int STOP() {
    /* Set the system to stopped */
    set_stop(1);
    
    /* Increment the PC to consume the mandatory 0x00 byte */
    uint16_t pc = get16(REG_PC);
    set16(REG_PC, pc + 1);
    
    return 4; 
}



/*================== STACK POINTER OPERATIONS =======================*/
/* pushes 16-bit register onto stack */
int PUSH(RegisterIndex src){
    uint16_t sp = get16(REG_SP);
    uint8_t low = get8(src, 0);
    uint8_t high = get8(src, 1);
    sp -= 1;
    memory_write(sp, high);
    sp -= 1;
    memory_write(sp, low);
    set16(REG_SP, sp);

    return 16;
}

/* pops 16-bit value from stack */
int POP(RegisterIndex dst){
    uint16_t sp = get16(REG_SP);

    uint8_t low = memory_read(sp);
    sp += 1;

    uint8_t high = memory_read(sp);
    sp += 1;
    /* edge case since lower bits of AF are Read only */
    if (dst == REG_AF) {
        low &= 0xF0; /* Force bits 3, 2, 1, 0 to always stay 0 */
    }

    set8(dst, 0, low);
    set8(dst, 1, high);

    set16(REG_SP, sp);
    
    return 12;
}



/*================== ROTATES\SHIFTS OPERATIONS =======================*/

/* 
note: here on the rotates i set the some of the Z flags to zero unconditonally, this does not match the 
documant im working with but sources online claim that in actual gameboy systems this is the case
*/
/* rotates A left circularly  */
int RLCA() {
    uint8_t value = get8(REG_AF, 1);
    int carry = (value >> 7) & 1;

    value = (value << 1) | carry;

    set8(REG_AF, 1, value);

    set_flag(FLAG_Z, 0); /* Unconditionally reset on real hardware */
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 4;
}

/* rotates A left through carry*/
int RLA() {
    uint8_t value = get8(REG_AF, 1);
    int carry = (value >> 7) & 1;
    int curr_carry = get_flag(FLAG_C);

    value = ((value << 1) & (~1)) | curr_carry;

    set8(REG_AF, 1, value);

    set_flag(FLAG_Z, 0); /* Unconditionally reset on real hardware */
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 4;
}

/* rotates A right circularly  */
int RRCA() {
    uint8_t value = get8(REG_AF, 1);
    int carry = value & 1;

    value = ((value >> 1) & 0x7F) | (carry << 7);

    set8(REG_AF, 1, value);

    set_flag(FLAG_Z, 0); /* Unconditionally reset on real hardware */
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 4;
}

/* rotates A right through carry  */
int RRA() {
    uint8_t value = get8(REG_AF, 1);
    int carry = value & 1;
    int curr_carry = get_flag(FLAG_C);

    value = ((value >> 1) & 0x7F) | (curr_carry << 7);

    set8(REG_AF, 1, value);

    set_flag(FLAG_Z, 0); /* Unconditionally reset on real hardware */
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 4;
}

/* rotates 8-bit register left */
int rotate_left_r(RegisterIndex src , int high) {
    uint8_t value = get8(src, high);
    /* get old bit 7 for carry*/
    int carry = ((value >> 7) & 1);

    value = (value << 1 | carry);

    set8(src, high, value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}

/* rotates 8-bit register left through carry*/
int rotate_left_carry_r(RegisterIndex src , int high) {
    uint8_t value = get8(src, high);
    /* get old bit 7 for carry*/
    int carry = ((value >> 7) & 1);
    int curr_carry = get_flag(FLAG_C);
    value = ((value << 1) & (~1)) | curr_carry;
    set8(src, high, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}

/* rotates 8-bit register right , old bit 0 to carry flag*/
int rotate_right_r(RegisterIndex src , int high){
    uint8_t value = get8(src, high);
    /* get old bit 0 for carry*/
    int carry = value & 1;
    value = ((value >> 1) & 0x7F) | (carry << 7);
    set8(src, high, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}

/* rotate 8-bit register right through carry flag */
int rotate_right_carry_r(RegisterIndex src, int high){
    uint8_t value = get8(src, high);
    /* get old bit 0 for carry*/
    int carry = value & 1;
    int curr_carry = get_flag(FLAG_C);
    value = ((value >> 1) & 0x7F) | (curr_carry << 7);
    set8(src, high, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}

/* rotates value at (HL) left */
int rotate_left_m() {
    uint16_t address = get16(REG_HL);
    uint8_t value = memory_read(address);
    int carry = (value >> 7) & 1;

    value = (value << 1) | carry;

    memory_write(address, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16; 
}

/* rotates value at (HL) left through carry */
int rotate_left_carry_m() {
    uint16_t address = get16(REG_HL);
    uint8_t value = memory_read(address);
    int carry = (value >> 7) & 1;
    int curr_carry = get_flag(FLAG_C);

    value = ((value << 1) & (~1)) | curr_carry;

    memory_write(address, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16;
}

/* rotates value at (HL) right*/
int rotate_right_m() {
    uint16_t address = get16(REG_HL);
    uint8_t value = memory_read(address);
    int carry = value & 1;

    value = ((value >> 1) & 0x7F) | (carry << 7);

    memory_write(address, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16;
}

/* rotates value at (HL) right through carry */
int rotate_right_carry_m() {
    uint16_t address = get16(REG_HL);
    uint8_t value = memory_read(address);
    int carry = value & 1;
    int curr_carry = get_flag(FLAG_C);

    value = ((value >> 1) & 0x7F) | (curr_carry << 7);

    memory_write(address, value);

    set_flag(FLAG_Z, (value == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16;
}
/* shifts 8-bit register left into carry, LSB set to 0 */
int SLA_r(RegisterIndex src, int high){
    uint8_t value = get8(src, high);
    
    /* get old bit 7 for carry */
    int carry = (value >> 7) & 1;

    value = value << 1;

    set8(src, high, value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}

/* shifts 8-bit value stored in (HL) left into carry, LSB set to 0 */
int SLA_m(){
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);
    
    /* get old bit 7 for carry */
    int carry = (value >> 7) & 1;

    value = value << 1;

    memory_write(addr, value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16;
}

/* shifts 8-bit register right into carry, MSB extends */
int SRA_r(RegisterIndex src, int high){
    uint8_t value = get8(src, high);
    /* get old bit 0 for carry*/
    int carry = value & 1;
    value = (value >> 1) | (value & 0x80);

    set8(src, high, value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}


/* shifts 8-bit value saved in (HL) right into carry, MSB extends */
int SRA_m(){
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);

    /* get old bit 0 for carry*/
    int carry = value & 1;
    value = (value >> 1) | (value & 0x80);

    memory_write(addr,value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16;
}

/* shifts 8-bit register right into carry, MSB sets to 0 */
int SRL_r(RegisterIndex src, int high){
    uint8_t value = get8(src, high);
    /* get old bit 0 for carry*/
    int carry = value & 1;
    value = (value >> 1) & 0x7F;

    set8(src, high, value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 8;
}


/* shifts 8-bit value saved in (HL) right into carry, MSB set to 0 */
int SRL_m(){
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);

    /* get old bit 0 for carry*/
    int carry = value & 1;
    value = (value >> 1) & 0x7F;

    memory_write(addr,value);
    
    set_flag(FLAG_Z, (value == 0) ? 1 : 0); 
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 0);
    set_flag(FLAG_C, carry);

    return 16;
}


/*================== BIT OPERATIONS =======================*/

/* Tests bit 'b' (0-7) in an 8-bit register */
int BIT_r(int b, RegisterIndex src, int high) {
    uint8_t value = get8(src, high);
    

    int bit_val = (value >> b) & 1;

    /* Z flag is set if the bit is 0 */
    set_flag(FLAG_Z, (bit_val == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 1);


    return 8; 
}

/* Tests bit 'b' (0-7) in the value stored at memory address (HL) */
int BIT_m(int b) {
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);
    

    int bit_val = (value >> b) & 1;

    set_flag(FLAG_Z, (bit_val == 0) ? 1 : 0);
    set_flag(FLAG_N, 0);
    set_flag(FLAG_H, 1);


    return 12; /* manual claims its 16 but online sources claim otherwise*/ 
}

/* Sets bit 'b' (0-7) in an 8-bit register */
int SET_r(int b, RegisterIndex src, int high) {
    uint8_t value = get8(src, high);

    value |= (1 << b);

    set8(src,high,value);
    return 8; 
}

/* Sets bit 'b' (0-7) in the value stored at memory address (HL) */
int SET_m(int b) {
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);
    

    value |= (1 << b);

    memory_write(addr,value);


    return 16; 
}

/* Resets bit 'b' (0-7) in an 8-bit register */
int RES_r(int b, RegisterIndex src, int high) {
    uint8_t value = get8(src, high);

    value &= ~(1 << b);

    set8(src,high,value);
    return 8; 
}

/* Resets bit 'b' (0-7) in the value stored at memory address (HL) */
int RES_m(int b) {
    uint16_t addr = get16(REG_HL);
    uint8_t value = memory_read(addr);
    

    value &= ~(1 << b);

    memory_write(addr,value);


    return 16; 
}


/*================== JUMP OPERATIONS =======================*/

/* sets PC value to target address */
int JP_addr(uint16_t addr){
    set16(REG_PC, addr);
    return 12;
}

/* Jump absolute if Zero flag is reset (JP NZ, nn) */
int JP_NZ(uint16_t addr) {
    if (get_flag(FLAG_Z) == 0) {
        set16(REG_PC, addr);
        return 16; /* Jump taken */
    }
    return 12; /* Jump not taken */
}

/* Jump absolute if Zero flag is set (JP Z, nn) */
int JP_Z(uint16_t addr) {
    if (get_flag(FLAG_Z) == 1) {
        set16(REG_PC, addr);
        return 16;
    }
    return 12;
}

/* Jump absolute if Carry flag is reset (JP NC, nn) */
int JP_NC(uint16_t addr) {
    if (get_flag(FLAG_C) == 0) {
        set16(REG_PC, addr);
        return 16;
    }
    return 12;
}

/* Jump absolute if Carry flag is set (JP C, nn) */
int JP_C(uint16_t addr) {
    if (get_flag(FLAG_C) == 1) {
        set16(REG_PC, addr);
        return 16;
    }
    return 12;
}

/* Unconditional absolute jump to address in HL (JP (HL)) */
int JP_HL() {
    uint16_t value = get16(REG_HL);
    set16(REG_PC, value);
    return 4; 
}

/* Unconditional relative jump (JR e) */
int JR(int8_t offset) {
    uint16_t pc = get16(REG_PC);
    set16(REG_PC, pc + offset);
    return 12;
}

/* Jump relative if Zero flag is reset (JR NZ, e) */
int JR_NZ(int8_t offset) {
    if (get_flag(FLAG_Z) == 0) {
        uint16_t pc = get16(REG_PC);
        set16(REG_PC, pc + offset);
        return 12; /* Jump taken */
    }
    return 8; /* Jump not taken */
}

/* Jump relative if Zero flag is set (JR Z, e) */
int JR_Z(int8_t offset) {
    if (get_flag(FLAG_Z) == 1) {
        uint16_t pc = get16(REG_PC);
        set16(REG_PC, pc + offset);
        return 12;
    }
    return 8;
}

/* Jump relative if Carry flag is reset (JR NC, e) */
int JR_NC(int8_t offset) {
    if (get_flag(FLAG_C) == 0) {
        uint16_t pc = get16(REG_PC);
        set16(REG_PC, pc + offset);
        return 12;
    }
    return 8;
}

/* Jump relative if Carry flag is set (JR C, e) */
int JR_C(int8_t offset) {
    if (get_flag(FLAG_C) == 1) {
        uint16_t pc = get16(REG_PC);
        set16(REG_PC, pc + offset);
        return 12;
    }
    return 8;
}




/*================== CALL OPERATIONS =======================*/


/* Push address of next instruction onto stack and then jump to address */
int CALL(uint16_t addr) {
    uint16_t return_addr = get16(REG_PC);
    uint8_t high = (return_addr >> 8) & 0xFF;
    uint8_t low  = return_addr & 0xFF;
    
    uint16_t sp = get16(REG_SP);
    
    sp -= 1;
    memory_write(sp, high);
    
    sp -= 1;
    memory_write(sp, low);
    
    set16(REG_SP, sp);
    
    set16(REG_PC, addr);
    
    return 24; 
}

/* Call if Zero flag is reset  */
int CALL_NZ(uint16_t addr) {
    if (get_flag(FLAG_Z) == 0) {
        /* Branch Taken */
        uint16_t return_addr = get16(REG_PC);
        uint8_t high = (return_addr >> 8) & 0xFF;
        uint8_t low  = return_addr & 0xFF;
        
        uint16_t sp = get16(REG_SP);
        sp -= 1;
        memory_write(sp, high);
        sp -= 1;
        memory_write(sp, low);
        set16(REG_SP, sp);
        
        set16(REG_PC, addr);
        return 24;
    }
    /* Branch Not Taken */
    return 12; 
}

/* Call if Zero flag is set */
int CALL_Z(uint16_t addr) {
    if (get_flag(FLAG_Z) == 1) {
        uint16_t return_addr = get16(REG_PC);
        uint8_t high = (return_addr >> 8) & 0xFF;
        uint8_t low  = return_addr & 0xFF;
        
        uint16_t sp = get16(REG_SP);
        sp -= 1;
        memory_write(sp, high);
        sp -= 1;
        memory_write(sp, low);
        set16(REG_SP, sp);
        
        set16(REG_PC, addr);
        return 24;
    }
    return 12;
}

/* Call if Carry flag is reset */
int CALL_NC(uint16_t addr) {
    if (get_flag(FLAG_C) == 0) {
        uint16_t return_addr = get16(REG_PC);
        uint8_t high = (return_addr >> 8) & 0xFF;
        uint8_t low  = return_addr & 0xFF;
        
        uint16_t sp = get16(REG_SP);
        sp -= 1;
        memory_write(sp, high);
        sp -= 1;
        memory_write(sp, low);
        set16(REG_SP, sp);
        
        set16(REG_PC, addr);
        return 24;
    }
    return 12;
}

/* Call if Carry flag is set */
int CALL_C(uint16_t addr) {
    if (get_flag(FLAG_C) == 1) {
        uint16_t return_addr = get16(REG_PC);
        uint8_t high = (return_addr >> 8) & 0xFF;
        uint8_t low  = return_addr & 0xFF;
        
        uint16_t sp = get16(REG_SP);
        sp -= 1;
        memory_write(sp, high);
        sp -= 1;
        memory_write(sp, low);
        set16(REG_SP, sp);
        
        set16(REG_PC, addr);
        return 24;
    }
    return 12;
}



/*================== RESTART OPERATIONS =======================*/

/* push current address onto stack, jump to 0x0000 + imm */
int RST(uint16_t imm){
    uint16_t curr_addr = get16(REG_PC);
    uint8_t high = (curr_addr >> 8) & 0xFF;
    uint8_t low  = curr_addr & 0xFF;
    uint16_t target_addr = 0x0000 + imm;
    uint16_t sp = get16(REG_SP);
    
    sp -= 1;
    memory_write(sp, high);
    
    sp -= 1;
    memory_write(sp, low);
    
    set16(REG_SP, sp);
    
    set16(REG_PC, target_addr);
    
    return 32; 
}


/*================== RETURN OPERATIONS =======================*/

/* pop two bytes from stack and jump to that address */
int RET(){
    uint16_t sp = get16(REG_SP);
    uint8_t low = memory_read(sp);
    sp += 1;
    uint8_t high = memory_read(sp);
    sp += 1;

    set16(REG_SP, sp);

    set16(REG_PC, ((uint16_t) high << 8) | (uint16_t) low );
    
    return 16; /* again this differs from the manual */ 
}

/* Return and Enable Interrupts (RETI) */
int RETI() {
    /* Exact same stack pop as RET */
    uint16_t sp = get16(REG_SP);
    uint8_t low = memory_read(sp);
    sp += 1;
    uint8_t high = memory_read(sp);
    sp += 1;

    set16(REG_SP, sp);
    set16(REG_PC, ((uint16_t)high << 8) | (uint16_t)low);
    
    /* RETI immediately enables interrupts */
    set_ime(1); 
    
    return 16;
}

/* Return if Zero flag is reset (RET NZ) */
int RET_NZ() {
    if (get_flag(FLAG_Z) == 0) {
        uint16_t sp = get16(REG_SP);
        uint8_t low = memory_read(sp);
        sp += 1;
        uint8_t high = memory_read(sp);
        sp += 1;

        set16(REG_SP, sp);
        set16(REG_PC, ((uint16_t)high << 8) | (uint16_t)low);
        return 20; /* Branch Taken */
    }
    return 8; /* Branch Not Taken */
}

/* Return if Zero flag is set (RET Z) */
int RET_Z() {
    if (get_flag(FLAG_Z) == 1) {
        uint16_t sp = get16(REG_SP);
        uint8_t low = memory_read(sp);
        sp += 1;
        uint8_t high = memory_read(sp);
        sp += 1;

        set16(REG_SP, sp);
        set16(REG_PC, ((uint16_t)high << 8) | (uint16_t)low);
        return 20;
    }
    return 8;
}