#ifndef REGISTER_FUNCTIONS_H
#define REGISTER_FUNCTIONS_H

#include <stdint.h>
#include "registers.h"
#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Example prototypes - add any others from register_functions.c as needed */
int load_R2R(RegisterIndex src, int src_high, RegisterIndex dst, int dst_high);
int load_I2R(RegisterIndex dst, int dst_high, uint8_t value);
int load_MR(RegisterIndex addressreg, RegisterIndex reg, int high,int M2R);
int load_MI(RegisterIndex addressreg, uint8_t value);
int load_abs2R(RegisterIndex reg, int high, uint16_t address);
int load_R2abs(uint16_t address, RegisterIndex reg, int high);

int ALU8_I2R(RegisterIndex dst, uint8_t value, int high , int sub);
int ALU8_M2R(RegisterIndex dst, RegisterIndex addressreg , int high , int sub);
int ALU8_R2R(RegisterIndex dst, RegisterIndex src , int dsthigh, int srchigh, int sub);
void ALU8(RegisterIndex dst, uint8_t value, int high, int sub, int use_carry, int set_res);

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
int AND_I2R(uint8_t imm);
int OR_I2R(uint8_t imm);
int XOR_I2R(uint8_t imm);

int CP_R(RegisterIndex src, int high);
int CP_M(void);
int CP_I(uint8_t imm);

/* High RAM / IO helpers */
int LD_A_C(void);
int LD_C_A(void);
int LDH_A_n(uint8_t imm);
int LDH_n_A(uint8_t imm);

/* Stack/16-bit helpers */
int LDHL_SP_n(uint8_t imm);
int LD_SP_HL(void);
int LD_abs_SP(uint16_t addr);

/* Conditional returns */
int RET_NC(void);
int RET_C(void);



#ifdef __cplusplus
}
#endif

#endif /* REGISTER_FUNCTIONS_H */