#ifndef REGISTER_FUNCTIONS_H
#define REGISTER_FUNCTIONS_H

#include <stdint.h>
#include "registers.h"
#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/*================== 8-BIT LOAD OPERATIONS =======================*/
int load_R2R(RegisterIndex src, int src_high, RegisterIndex dst, int dst_high);
int load_I2R(RegisterIndex dst, int dst_high, uint8_t value);
int load_MR(RegisterIndex addressreg, RegisterIndex reg, int high, int M2R);
int load_MI(RegisterIndex addressreg, uint8_t value);
int load_abs2R(RegisterIndex reg, int high, uint16_t address);
int load_R2abs(uint16_t address, RegisterIndex reg, int high);
int LD_A_C(void);
int LD_C_A(void);
int LDH_A_n(uint8_t imm);
int LDH_n_A(uint8_t imm);

/*================== 16-BIT LOAD OPERATIONS =======================*/
int load_16_R2R(RegisterIndex src, RegisterIndex dst);
int load_16_I2R(RegisterIndex dst, uint16_t value);
int LDHL_SP_n(uint8_t imm);
int LD_SP_HL(void);
int LD_abs_SP(uint16_t addr);

/*================== ARITHMETIC OPERATIONS =======================*/
void ALU8(RegisterIndex dst, uint8_t value, int high, int sub, int use_carry, int set_res);
int ALU8_I2R(RegisterIndex dst, uint8_t value, int high, int sub);
int ALU8_M2R(RegisterIndex dst, RegisterIndex addressreg, int high, int sub);
int ALU8_R2R(RegisterIndex dst, RegisterIndex src, int dsthigh, int srchigh, int sub);
int ADC_R2R(RegisterIndex src, int srchigh);
int ADC_I2R(uint8_t immediate_value);
int SBC_R2R(RegisterIndex src, int srchigh);
int SBC_I2R(uint8_t immediate_value);
int ADC_M(void);
int SBC_M(void);
int AND_r(RegisterIndex src, int high);
int OR_r(RegisterIndex src, int high);
int XOR_r(RegisterIndex src, int high);
int AND_m(void);
int OR_m(void);
int XOR_m(void);
int CP_R(RegisterIndex src, int high);
int CP_M(void);
int AND_I2R(uint8_t imm);
int OR_I2R(uint8_t imm);
int XOR_I2R(uint8_t imm);
int CP_I(uint8_t imm);
int INC_DEC_8_R(RegisterIndex src, int high, int dec);
int INC_DEC_8_M(int dec);
void ALU16(RegisterIndex dst, uint16_t value);
int ADD_16_R(RegisterIndex dst, RegisterIndex src);
int ADD_16_SP_OFFSET(uint8_t imm);
int INC_DEC_16(RegisterIndex src, int dec);

/*================== MISCELLANEOUS OPERATIONS =======================*/
int SWAP_R(RegisterIndex src, int high);
int SWAP_M(void);
int DAA(void);
int CPL(void);
int CCF(void);
int SCF(void);
int NOP(void);
int HALT(void);
int DI(void);
int EI(void);
int STOP(void);

/*================== STACK POINTER OPERATIONS =======================*/
int PUSH(RegisterIndex src);
int POP(RegisterIndex dst);

/*================== ROTATES/SHIFTS OPERATIONS =======================*/
int RLCA(void);
int RLA(void);
int RRCA(void);
int RRA(void);
int rotate_left_r(RegisterIndex src, int high);
int rotate_left_carry_r(RegisterIndex src, int high);
int rotate_right_r(RegisterIndex src, int high);
int rotate_right_carry_r(RegisterIndex src, int high);
int rotate_left_m(void);
int rotate_left_carry_m(void);
int rotate_right_m(void);
int rotate_right_carry_m(void);
int SLA_r(RegisterIndex src, int high);
int SLA_m(void);
int SRA_r(RegisterIndex src, int high);
int SRA_m(void);
int SRL_r(RegisterIndex src, int high);
int SRL_m(void);

/*================== BIT OPERATIONS =======================*/
int BIT_r(int b, RegisterIndex src, int high);
int BIT_m(int b);
int SET_r(int b, RegisterIndex src, int high);
int SET_m(int b);
int RES_r(int b, RegisterIndex src, int high);
int RES_m(int b);

/*================== JUMP OPERATIONS =======================*/
int JP_addr(uint16_t addr);
int JP_NZ(uint16_t addr);
int JP_Z(uint16_t addr);
int JP_NC(uint16_t addr);
int JP_C(uint16_t addr);
int JP_HL(void);
int JR(int8_t offset);
int JR_NZ(int8_t offset);
int JR_Z(int8_t offset);
int JR_NC(int8_t offset);
int JR_C(int8_t offset);

/*================== CALL OPERATIONS =======================*/
int CALL(uint16_t addr);
int CALL_NZ(uint16_t addr);
int CALL_Z(uint16_t addr);
int CALL_NC(uint16_t addr);
int CALL_C(uint16_t addr);

/*================== RESTART OPERATIONS =======================*/
int RST(uint16_t imm);

/*================== RETURN OPERATIONS =======================*/
int RET(void);
int RETI(void);
int RET_NZ(void);
int RET_Z(void);
int RET_NC(void);
int RET_C(void);



#ifdef __cplusplus
}
#endif

#endif /* REGISTER_FUNCTIONS_H */