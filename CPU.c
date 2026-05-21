#include "registers.h"
#include "memory.h"
#include "register_functions.h"
#include <stdint.h>

/*
This file will handle the actual fetch - decode - execute cycle


*/

/* counts total cycles (may be used for save states)*/
extern uint64_t total_cycles; 

/* This tracks cycles for the current video frame */
static int32_t frame_cycles = 0;


/* fetches next inctuction from PC, increments PC */

uint8_t FETCH() {
    uint8_t instruction = memory_read(get16(REG_PC));

    uint16_t pc = get16(REG_PC);
    set16(REG_PC, pc + 1);
    total_cycles += 4;
    return instruction;
}


/* 
This function acts as both the DECODE and the EXECUTE part of the cycle,
the massive switch case block when compiled should create a big optimized jump table
*/
void cpu_step() {
    if(!get_stop()){ 
        uint8_t opcode = FETCH(); 
        switch (opcode) {
            /* 0x00 NOP */ case 0x00: total_cycles += NOP(); break;
            /* 0x01 LD BC,d16 */ case 0x01: {
                uint8_t low = FETCH(); 
                uint8_t high = FETCH();
                uint16_t imm = ((uint16_t)high << 8) | low;
                
                total_cycles += load_16_I2R(REG_BC, imm);
                break;
            } 
            /* 0x02 LD (BC),A */ case 0x02: {
                total_cycles += load_MR(REG_BC, REG_AF, 1, 0);
                break;
            }
            /* 0x03 INC BC */ case 0x03: {
                total_cycles += INC_DEC_16(REG_BC, 0 );
                break;
            } 
            /* 0x04 INC B */ case 0x04: {
                total_cycles += INC_DEC_8_R(REG_BC,1,0);
                break;
            }
            /* 0x05 DEC B */ case 0x05: {
                total_cycles += INC_DEC_8_R(REG_BC,1,1);
                break;
            }
            /* 0x06 LD B,d8 */ case 0x06: {
                
                uint8_t imm = FETCH(); 
                total_cycles += load_I2R(REG_BC, 1, imm); 
                break;
            }
            /* 0x07 RLCA */ case 0x07: {
                total_cycles += RLCA();
                break;
            }
            /* 0x08 LD (a16),SP */ case 0x08: {
                uint8_t low = FETCH(); 
                uint8_t high = FETCH();
                uint16_t addr = ((uint16_t)high << 8) | low;
                total_cycles += LD_abs_SP(addr);
                break;
            }
            /* 0x09 ADD HL,BC */ case 0x09: {
                total_cycles += ADD_16_R(REG_HL, REG_BC);
                break; 
            }
            /* 0x0A LD A,(BC) */ case 0x0A: {
                total_cycles += load_MR(REG_BC, REG_AF, 1, 1);
                break;
            }
            /* 0x0B DEC BC */ case 0x0B: {
                total_cycles += INC_DEC_16(REG_BC, 1);
                break;
            }
            /* 0x0C INC C */ case 0x0C: {
                total_cycles += INC_DEC_8_R(REG_BC, 0, 0);
                break;
            }
            /* 0x0D DEC C */ case 0x0D: {
                total_cycles += INC_DEC_8_R(REG_BC, 0, 1);
                break;
            }
            /* 0x0E LD C,d8 */ case 0x0E: {
                uint8_t imm = FETCH();
                total_cycles += load_I2R(REG_BC, 0, imm);
                break;
            }
            /* 0x11 LD DE,d16 */ case 0x11: {
                uint8_t low = FETCH();
                uint8_t high = FETCH();
                uint16_t imm = ((uint16_t)high << 8) | low;
                total_cycles += load_16_I2R(REG_DE, imm);
                break;
            }
            /* 0x12 LD (DE),A */ case 0x12: {
                total_cycles += load_MR(REG_DE, REG_AF, 1, 0);
                break;
            }
            /* 0x13 INC DE */ case 0x13: {
                total_cycles += INC_DEC_16(REG_DE, 0);
                break;
            }
            /* 0x14 INC D */ case 0x14: {
                total_cycles += INC_DEC_8_R(REG_DE, 1, 0);
                break;
            }
            /* 0x15 DEC D */ case 0x15: {
                total_cycles += INC_DEC_8_R(REG_DE, 1, 1);
                break;
            }
            /* 0x16 LD D,d8 */ case 0x16: {
                uint8_t imm = FETCH();
                total_cycles += load_I2R(REG_DE, 1, imm);
                break;
            }
            /* 0x19 ADD HL,DE */ case 0x19: /* TODO: implement */ break;
            /* 0x1A LD A,(DE) */ case 0x1A: {
                total_cycles += load_MR(REG_DE, REG_AF, 1, 1);
                break;
            }
            /* 0x1B DEC DE */ case 0x1B: {
                total_cycles += INC_DEC_16(REG_DE, 1);
                break;
            }
            /* 0x1C INC E */ case 0x1C: {
                total_cycles += INC_DEC_8_R(REG_DE, 0, 0);
                break;
            }
            /* 0x1D DEC E */ case 0x1D: {
                total_cycles += INC_DEC_8_R(REG_DE, 0, 1);
                break;
            }
            /* 0x1E LD E,d8 */ case 0x1E: {
                uint8_t imm = FETCH();
                total_cycles += load_I2R(REG_DE, 0, imm);
                break;
            }
            /* 0x21 LD HL,d16 */ case 0x21: {
                uint8_t low = FETCH();
                uint8_t high = FETCH();
                uint16_t imm = ((uint16_t)high << 8) | low;
                total_cycles += load_16_I2R(REG_HL, imm);
                break;
            }
            /* 0x22 LD (HL+),A */ case 0x22: {
                total_cycles += load_MR(REG_HL, REG_AF, 1, 0);
                set16(REG_HL, get16(REG_HL) + 1);
                break;
            }
            /* 0x23 INC HL */ case 0x23: {
                total_cycles += INC_DEC_16(REG_HL, 0);
                break;
            }
            /* 0x24 INC H */ case 0x24: {
                total_cycles += INC_DEC_8_R(REG_HL, 1, 0);
                break;
            }
            /* 0x25 DEC H */ case 0x25: {
                total_cycles += INC_DEC_8_R(REG_HL, 1, 1);
                break;
            }
            /* 0x26 LD H,d8 */ case 0x26: {
                uint8_t imm = FETCH();
                total_cycles += load_I2R(REG_HL, 1, imm);
                break;
            }
            /* 0x29 ADD HL,HL */ case 0x29: /* TODO: implement */ break;
            /* 0x2A LD A,(HL+) */ case 0x2A: {
                total_cycles += load_MR(REG_HL, REG_AF, 1, 1);
                set16(REG_HL, get16(REG_HL) + 1);
                break;
            }
            /* 0x2B DEC HL */ case 0x2B: {
                total_cycles += INC_DEC_16(REG_HL, 1);
                break;
            }
            /* 0x2C INC L */ case 0x2C: {
                total_cycles += INC_DEC_8_R(REG_HL, 0, 0);
                break;
            }
            /* 0x2D DEC L */ case 0x2D: {
                total_cycles += INC_DEC_8_R(REG_HL, 0, 1);
                break;
            }
            /* 0x2E LD L,d8 */ case 0x2E: {
                uint8_t imm = FETCH();
                total_cycles += load_I2R(REG_HL, 0, imm);
                break;
            }
            /* 0x31 LD SP,d16 */ case 0x31: {
                uint8_t low = FETCH();
                uint8_t high = FETCH();
                uint16_t imm = ((uint16_t)high << 8) | low;
                total_cycles += load_16_I2R(REG_SP, imm);
                break;
            }
            /* 0x32 LD (HL-),A */ case 0x32: {
                total_cycles += load_MR(REG_HL, REG_AF, 1, 0);
                set16(REG_HL, get16(REG_HL) - 1);
                break;
            }
            /* 0x33 INC SP */ case 0x33: {
                total_cycles += INC_DEC_16(REG_SP, 0);
                break;
            }
            /* 0x34 INC (HL) */ case 0x34: {
                total_cycles += INC_DEC_8_M(0);
                break;
            }
            /* 0x35 DEC (HL) */ case 0x35: {
                total_cycles += INC_DEC_8_M(1);
                break;
            }
            /* 0x36 LD (HL),d8 */ case 0x36: {
                uint8_t imm = FETCH();
                total_cycles += load_MI(REG_HL, imm);
                break;
            }
            /* 0x39 ADD HL,SP */ case 0x39: /* TODO: implement */ break;
            /* 0x3A LD A,(HL-) */ case 0x3A: {
                total_cycles += load_MR(REG_HL, REG_AF, 1, 1);
                set16(REG_HL, get16(REG_HL) - 1);
                break;
            }
            /* 0x3B DEC SP */ case 0x3B: {
                total_cycles += INC_DEC_16(REG_SP, 1);
                break;
            }
            /* 0x3C INC A */ case 0x3C: {
                total_cycles += INC_DEC_8_R(REG_AF, 1, 0);
                break;
            }
            /* 0x3D DEC A */ case 0x3D: {
                total_cycles += INC_DEC_8_R(REG_AF, 1, 1);
                break;
            }
            /* 0x3E LD A,d8 */ case 0x3E: {
                uint8_t imm = FETCH();
                total_cycles += load_I2R(REG_AF, 1, imm);
                break;
            }
            /* 0x40 LD B,B */ case 0x40: { total_cycles += load_R2R(REG_BC, 1, REG_BC, 1); break; }
            /* 0x41 LD B,C */ case 0x41: { total_cycles += load_R2R(REG_BC, 0, REG_BC, 1); break; }
            /* 0x42 LD B,D */ case 0x42: { total_cycles += load_R2R(REG_DE, 1, REG_BC, 1); break; }
            /* 0x43 LD B,E */ case 0x43: { total_cycles += load_R2R(REG_DE, 0, REG_BC, 1); break; }
            /* 0x44 LD B,H */ case 0x44: { total_cycles += load_R2R(REG_HL, 1, REG_BC, 1); break; }
            /* 0x45 LD B,L */ case 0x45: { total_cycles += load_R2R(REG_HL, 0, REG_BC, 1); break; }
            /* 0x46 LD B,(HL) */ case 0x46: { total_cycles += load_MR(REG_HL, REG_BC, 1, 1); break; }
            /* 0x47 LD B,A */ case 0x47: { total_cycles += load_R2R(REG_AF, 1, REG_BC, 1); break; }
            /* 0x48 LD C,B */ case 0x48: { total_cycles += load_R2R(REG_BC, 1, REG_BC, 0); break; }
            /* 0x49 LD C,C */ case 0x49: { total_cycles += load_R2R(REG_BC, 0, REG_BC, 0); break; }
            /* 0x4A LD C,D */ case 0x4A: { total_cycles += load_R2R(REG_DE, 1, REG_BC, 0); break; }
            /* 0x4B LD C,E */ case 0x4B: { total_cycles += load_R2R(REG_DE, 0, REG_BC, 0); break; }
            /* 0x4C LD C,H */ case 0x4C: { total_cycles += load_R2R(REG_HL, 1, REG_BC, 0); break; }
            /* 0x4D LD C,L */ case 0x4D: { total_cycles += load_R2R(REG_HL, 0, REG_BC, 0); break; }
            /* 0x4E LD C,(HL) */ case 0x4E: { total_cycles += load_MR(REG_HL, REG_BC, 0, 1); break; }
            /* 0x4F LD C,A */ case 0x4F: { total_cycles += load_R2R(REG_AF, 1, REG_BC, 0); break; }
            /* 0x50 LD D,B */ case 0x50: { total_cycles += load_R2R(REG_BC, 1, REG_DE, 1); break; }
            /* 0x51 LD D,C */ case 0x51: { total_cycles += load_R2R(REG_BC, 0, REG_DE, 1); break; }
            /* 0x52 LD D,D */ case 0x52: { total_cycles += load_R2R(REG_DE, 1, REG_DE, 1); break; }
            /* 0x53 LD D,E */ case 0x53: { total_cycles += load_R2R(REG_DE, 0, REG_DE, 1); break; }
            /* 0x54 LD D,H */ case 0x54: { total_cycles += load_R2R(REG_HL, 1, REG_DE, 1); break; }
            /* 0x55 LD D,L */ case 0x55: { total_cycles += load_R2R(REG_HL, 0, REG_DE, 1); break; }
            /* 0x56 LD D,(HL) */ case 0x56: { total_cycles += load_MR(REG_HL, REG_DE, 1, 1); break; }
            /* 0x57 LD D,A */ case 0x57: { total_cycles += load_R2R(REG_AF, 1, REG_DE, 1); break; }
            /* 0x58 LD E,B */ case 0x58: { total_cycles += load_R2R(REG_BC, 1, REG_DE, 0); break; }
            /* 0x59 LD E,C */ case 0x59: { total_cycles += load_R2R(REG_BC, 0, REG_DE, 0); break; }
            /* 0x5A LD E,D */ case 0x5A: { total_cycles += load_R2R(REG_DE, 1, REG_DE, 0); break; }
            /* 0x5B LD E,E */ case 0x5B: { total_cycles += load_R2R(REG_DE, 0, REG_DE, 0); break; }
            /* 0x5C LD E,H */ case 0x5C: { total_cycles += load_R2R(REG_HL, 1, REG_DE, 0); break; }
            /* 0x5D LD E,L */ case 0x5D: { total_cycles += load_R2R(REG_HL, 0, REG_DE, 0); break; }
            /* 0x5E LD E,(HL) */ case 0x5E: { total_cycles += load_MR(REG_HL, REG_DE, 0, 1); break; }
            /* 0x5F LD E,A */ case 0x5F: { total_cycles += load_R2R(REG_AF, 1, REG_DE, 0); break; }
            /* 0x60 LD H,B */ case 0x60: { total_cycles += load_R2R(REG_BC, 1, REG_HL, 1); break; }
            /* 0x61 LD H,C */ case 0x61: { total_cycles += load_R2R(REG_BC, 0, REG_HL, 1); break; }
            /* 0x62 LD H,D */ case 0x62: { total_cycles += load_R2R(REG_DE, 1, REG_HL, 1); break; }
            /* 0x63 LD H,E */ case 0x63: { total_cycles += load_R2R(REG_DE, 0, REG_HL, 1); break; }
            /* 0x64 LD H,H */ case 0x64: { total_cycles += load_R2R(REG_HL, 1, REG_HL, 1); break; }
            /* 0x65 LD H,L */ case 0x65: { total_cycles += load_R2R(REG_HL, 0, REG_HL, 1); break; }
            /* 0x66 LD H,(HL) */ case 0x66: { total_cycles += load_MR(REG_HL, REG_HL, 1, 1); break; }
            /* 0x67 LD H,A */ case 0x67: { total_cycles += load_R2R(REG_AF, 1, REG_HL, 1); break; }
            /* 0x68 LD L,B */ case 0x68: { total_cycles += load_R2R(REG_BC, 1, REG_HL, 0); break; }
            /* 0x69 LD L,C */ case 0x69: { total_cycles += load_R2R(REG_BC, 0, REG_HL, 0); break; }
            /* 0x6A LD L,D */ case 0x6A: { total_cycles += load_R2R(REG_DE, 1, REG_HL, 0); break; }
            /* 0x6B LD L,E */ case 0x6B: { total_cycles += load_R2R(REG_DE, 0, REG_HL, 0); break; }
            /* 0x6C LD L,H */ case 0x6C: { total_cycles += load_R2R(REG_HL, 1, REG_HL, 0); break; }
            /* 0x6D LD L,L */ case 0x6D: { total_cycles += load_R2R(REG_HL, 0, REG_HL, 0); break; }
            /* 0x6E LD L,(HL) */ case 0x6E: { total_cycles += load_MR(REG_HL, REG_HL, 0, 1); break; }
            /* 0x6F LD L,A */ case 0x6F: { total_cycles += load_R2R(REG_AF, 1, REG_HL, 0); break; }
            /* 0x70 LD (HL),B */ case 0x70: { total_cycles += load_MR(REG_HL, REG_BC, 1, 0); break; }
            /* 0x71 LD (HL),C */ case 0x71: { total_cycles += load_MR(REG_HL, REG_BC, 0, 0); break; }
            /* 0x72 LD (HL),D */ case 0x72: { total_cycles += load_MR(REG_HL, REG_DE, 1, 0); break; }
            /* 0x73 LD (HL),E */ case 0x73: { total_cycles += load_MR(REG_HL, REG_DE, 0, 0); break; }
            /* 0x74 LD (HL),H */ case 0x74: { total_cycles += load_MR(REG_HL, REG_HL, 1, 0); break; }
            /* 0x75 LD (HL),L */ case 0x75: { total_cycles += load_MR(REG_HL, REG_HL, 0, 0); break; }
            /* 0x77 LD (HL),A */ case 0x77: { total_cycles += load_MR(REG_HL, REG_AF, 1, 0); break; }
            /* 0x78 LD A,B */ case 0x78: { total_cycles += load_R2R(REG_BC, 1, REG_AF, 1); break; }
            /* 0x79 LD A,C */ case 0x79: { total_cycles += load_R2R(REG_BC, 0, REG_AF, 1); break; }
            /* 0x7A LD A,D */ case 0x7A: { total_cycles += load_R2R(REG_DE, 1, REG_AF, 1); break; }
            /* 0x7B LD A,E */ case 0x7B: { total_cycles += load_R2R(REG_DE, 0, REG_AF, 1); break; }
            /* 0x7C LD A,H */ case 0x7C: { total_cycles += load_R2R(REG_HL, 1, REG_AF, 1); break; }
            /* 0x7D LD A,L */ case 0x7D: { total_cycles += load_R2R(REG_HL, 0, REG_AF, 1); break; }
            /* 0x7E LD A,(HL) */ case 0x7E: { total_cycles += load_MR(REG_HL, REG_AF, 1, 1); break; }
            /* 0x7F LD A,A */ case 0x7F: { total_cycles += load_R2R(REG_AF, 1, REG_AF, 1); break; }
            /* 0x80 ADD A,B */ case 0x80: { total_cycles += ALU8_R2R(REG_AF, REG_BC, 1, 1, 0); break; }
            /* 0x81 ADD A,C */ case 0x81: { total_cycles += ALU8_R2R(REG_AF, REG_BC, 1, 0, 0); break; }
            /* 0x82 ADD A,D */ case 0x82: { total_cycles += ALU8_R2R(REG_AF, REG_DE, 1, 1, 0); break; }
            /* 0x83 ADD A,E */ case 0x83: { total_cycles += ALU8_R2R(REG_AF, REG_DE, 1, 0, 0); break; }
            /* 0x84 ADD A,H */ case 0x84: { total_cycles += ALU8_R2R(REG_AF, REG_HL, 1, 1, 0); break; }
            /* 0x85 ADD A,L */ case 0x85: { total_cycles += ALU8_R2R(REG_AF, REG_HL, 1, 0, 0); break; }
            /* 0x86 ADD A,(HL) */ case 0x86: { total_cycles += ALU8_M2R(REG_AF, REG_HL, 1, 0); break; }
            /* 0x87 ADD A,A */ case 0x87: { total_cycles += ALU8_R2R(REG_AF, REG_AF, 1, 1, 0); break; }
            /* 0x88 ADC A,B */ case 0x88: { total_cycles += ADC_R2R(REG_BC, 1); break; }
            /* 0x89 ADC A,C */ case 0x89: { total_cycles += ADC_R2R(REG_BC, 0); break; }
            /* 0x8A ADC A,D */ case 0x8A: { total_cycles += ADC_R2R(REG_DE, 1); break; }
            /* 0x8B ADC A,E */ case 0x8B: { total_cycles += ADC_R2R(REG_DE, 0); break; }
            /* 0x8C ADC A,H */ case 0x8C: { total_cycles += ADC_R2R(REG_HL, 1); break; }
            /* 0x8D ADC A,L */ case 0x8D: { total_cycles += ADC_R2R(REG_HL, 0); break; }
            /* 0x8E ADC A,(HL) */ case 0x8E: { total_cycles += ADC_M(); break; }
            /* 0x8F ADC A,A */ case 0x8F: { total_cycles += ADC_R2R(REG_AF, 1); break; }
            /* 0x90 SUB B */ case 0x90: { total_cycles += ALU8_R2R(REG_AF, REG_BC, 1, 1, 1); break; }
            /* 0x91 SUB C */ case 0x91: { total_cycles += ALU8_R2R(REG_AF, REG_BC, 1, 0, 1); break; }
            /* 0x92 SUB D */ case 0x92: { total_cycles += ALU8_R2R(REG_AF, REG_DE, 1, 1, 1); break; }
            /* 0x93 SUB E */ case 0x93: { total_cycles += ALU8_R2R(REG_AF, REG_DE, 1, 0, 1); break; }
            /* 0x94 SUB H */ case 0x94: { total_cycles += ALU8_R2R(REG_AF, REG_HL, 1, 1, 1); break; }
            /* 0x95 SUB L */ case 0x95: { total_cycles += ALU8_R2R(REG_AF, REG_HL, 1, 0, 1); break; }
            /* 0x96 SUB (HL) */ case 0x96: { total_cycles += ALU8_M2R(REG_AF, REG_HL, 1, 1); break; }
            /* 0x97 SUB A */ case 0x97: { total_cycles += ALU8_R2R(REG_AF, REG_AF, 1, 1, 1); break; }
            /* 0x98 SBC A,B */ case 0x98: { total_cycles += SBC_R2R(REG_BC, 1); break; }
            /* 0x99 SBC A,C */ case 0x99: { total_cycles += SBC_R2R(REG_BC, 0); break; }
            /* 0x9A SBC A,D */ case 0x9A: { total_cycles += SBC_R2R(REG_DE, 1); break; }
            /* 0x9B SBC A,E */ case 0x9B: { total_cycles += SBC_R2R(REG_DE, 0); break; }
            /* 0x9C SBC A,H */ case 0x9C: { total_cycles += SBC_R2R(REG_HL, 1); break; }
            /* 0x9D SBC A,L */ case 0x9D: { total_cycles += SBC_R2R(REG_HL, 0); break; }
            /* 0x9E SBC A,(HL) */ case 0x9E: { total_cycles += SBC_M(); break; }
            /* 0x9F SBC A,A */ case 0x9F: { total_cycles += SBC_R2R(REG_AF, 1); break; }
            /* 0xA0 AND B */ case 0xA0: { total_cycles += AND_r(REG_BC, 1); break; }
            /* 0xA1 AND C */ case 0xA1: { total_cycles += AND_r(REG_BC, 0); break; }
            /* 0xA2 AND D */ case 0xA2: { total_cycles += AND_r(REG_DE, 1); break; }
            /* 0xA3 AND E */ case 0xA3: { total_cycles += AND_r(REG_DE, 0); break; }
            /* 0xA4 AND H */ case 0xA4: { total_cycles += AND_r(REG_HL, 1); break; }
            /* 0xA5 AND L */ case 0xA5: { total_cycles += AND_r(REG_HL, 0); break; }
            /* 0xA6 AND (HL) */ case 0xA6: { total_cycles += AND_m(); break; }
            /* 0xA7 AND A */ case 0xA7: { total_cycles += AND_r(REG_AF, 1); break; }
            /* 0xA8 XOR B */ case 0xA8: { total_cycles += XOR_r(REG_BC, 1); break; }
            /* 0xA9 XOR C */ case 0xA9: { total_cycles += XOR_r(REG_BC, 0); break; }
            /* 0xAA XOR D */ case 0xAA: { total_cycles += XOR_r(REG_DE, 1); break; }
            /* 0xAB XOR E */ case 0xAB: { total_cycles += XOR_r(REG_DE, 0); break; }
            /* 0xAC XOR H */ case 0xAC: { total_cycles += XOR_r(REG_HL, 1); break; }
            /* 0xAD XOR L */ case 0xAD: { total_cycles += XOR_r(REG_HL, 0); break; }
            /* 0xAE XOR (HL) */ case 0xAE: { total_cycles += XOR_m(); break; }
            /* 0xAF XOR A */ case 0xAF: { total_cycles += XOR_r(REG_AF, 1); break; }
            /* 0xB0 OR B */ case 0xB0: { total_cycles += OR_r(REG_BC, 1); break; }
            /* 0xB1 OR C */ case 0xB1: { total_cycles += OR_r(REG_BC, 0); break; }
            /* 0xB2 OR D */ case 0xB2: { total_cycles += OR_r(REG_DE, 1); break; }
            /* 0xB3 OR E */ case 0xB3: { total_cycles += OR_r(REG_DE, 0); break; }
            /* 0xB4 OR H */ case 0xB4: { total_cycles += OR_r(REG_HL, 1); break; }
            /* 0xB5 OR L */ case 0xB5: { total_cycles += OR_r(REG_HL, 0); break; }
            /* 0xB6 OR (HL) */ case 0xB6: { total_cycles += OR_m(); break; }
            /* 0xB7 OR A */ case 0xB7: { total_cycles += OR_r(REG_AF, 1); break; }
            /* 0xB8 CP B */ case 0xB8: { total_cycles += CP_R(REG_BC, 1); break; }
            /* 0xB9 CP C */ case 0xB9: { total_cycles += CP_R(REG_BC, 0); break; }
            /* 0xBA CP D */ case 0xBA: { total_cycles += CP_R(REG_DE, 1); break; }
            /* 0xBB CP E */ case 0xBB: { total_cycles += CP_R(REG_DE, 0); break; }
            /* 0xBC CP H */ case 0xBC: { total_cycles += CP_R(REG_HL, 1); break; }
            /* 0xBD CP L */ case 0xBD: { total_cycles += CP_R(REG_HL, 0); break; }
            /* 0xBE CP (HL) */ case 0xBE: { total_cycles += CP_M(); break; }
            /* 0xBF CP A */ case 0xBF: { total_cycles += CP_R(REG_AF, 1); break; }
            /* 0xC2 JP NZ,a16 */ case 0xC2: /* TODO: implement */ break;
            /* 0xC3 JP a16 */ case 0xC3: /* TODO: implement */ break;
            /* 0xC4 CALL NZ,a16 */ case 0xC4: /* TODO: implement */ break;
            /* 0xC5 PUSH BC */ case 0xC5: /* TODO: implement */ break;
            /* 0xC6 ADD A,d8 */ case 0xC6: /* TODO: implement */ break;
            /* 0xC7 RST 00H */ case 0xC7: /* TODO: implement */ break;
            /* 0xC8 RET Z */ case 0xC8: /* TODO: implement */ break;
            /* 0xC9 RET */ case 0xC9: /* TODO: implement */ break;
            /* 0xCA JP Z,a16 */ case 0xCA: /* TODO: implement */ break;
            /* 0xCB */ case 0xCB: execute_cb_instruction(); break;
            /* 0xCC CALL Z,a16 */ case 0xCC: /* TODO: implement */ break;
            /* 0xCD CALL a16 */ case 0xCD: /* TODO: implement */ break;
            /* 0xCE ADC A,d8 */ case 0xCE: /* TODO: implement */ break;
            /* 0xCF RST 08H */ case 0xCF: /* TODO: implement */ break;
            /* 0xD0 RET NC */ case 0xD0: /* TODO: implement */ break;
            /* 0xD1 POP DE */ case 0xD1: /* TODO: implement */ break;
            /* 0xD2 JP NC,a16 */ case 0xD2: /* TODO: implement */ break;
            /* 0xD3 UNUSED */ case 0xD3: /* TODO: implement */ break;
            /* 0xD4 CALL NC,a16 */ case 0xD4: /* TODO: implement */ break;
            /* 0xD5 PUSH DE */ case 0xD5: /* TODO: implement */ break;
            /* 0xD6 SUB d8 */ case 0xD6: /* TODO: implement */ break;
            /* 0xD7 RST 10H */ case 0xD7: /* TODO: implement */ break;
            /* 0xD8 RET C */ case 0xD8: /* TODO: implement */ break;
            /* 0xD9 RETI */ case 0xD9: /* TODO: implement */ break;
            /* 0xDA JP C,a16 */ case 0xDA: /* TODO: implement */ break;
            /* 0xDB UNUSED */ case 0xDB: /* TODO: implement */ break;
            /* 0xDC CALL C,a16 */ case 0xDC: /* TODO: implement */ break;
            /* 0xDD UNUSED */ case 0xDD: /* TODO: implement */ break;
            /* 0xDE SBC A,d8 */ case 0xDE: /* TODO: implement */ break;
            /* 0xDF RST 18H */ case 0xDF: /* TODO: implement */ break;
            /* 0xE0 LDH (a8),A */ case 0xE0: /* TODO: implement */ break;
            /* 0xE1 POP HL */ case 0xE1: /* TODO: implement */ break;
            /* 0xE2 LD (C),A */ case 0xE2: /* TODO: implement */ break;
            /* 0xE3 UNUSED */ case 0xE3: /* TODO: implement */ break;
            /* 0xE4 UNUSED */ case 0xE4: /* TODO: implement */ break;
            /* 0xE5 PUSH HL */ case 0xE5: /* TODO: implement */ break;
            /* 0xE6 AND d8 */ case 0xE6: /* TODO: implement */ break;
            /* 0xE7 RST 20H */ case 0xE7: /* TODO: implement */ break;
            /* 0xE8 ADD SP,r8 */ case 0xE8: /* TODO: implement */ break;
            /* 0xE9 JP (HL) */ case 0xE9: /* TODO: implement */ break;
            /* 0xEA LD (a16),A */ case 0xEA: /* TODO: implement */ break;
            /* 0xEB UNUSED */ case 0xEB: /* TODO: implement */ break;
            /* 0xEC UNUSED */ case 0xEC: /* TODO: implement */ break;
            /* 0xED UNUSED */ case 0xED: /* TODO: implement */ break;
            /* 0xEE XOR d8 */ case 0xEE: /* TODO: implement */ break;
            /* 0xEF RST 28H */ case 0xEF: /* TODO: implement */ break;
            /* 0xF0 LDH A,(a8) */ case 0xF0: /* TODO: implement */ break;
            /* 0xF1 POP AF */ case 0xF1: /* TODO: implement */ break;
            /* 0xF2 LD A,(C) */ case 0xF2: /* TODO: implement */ break;
            /* 0xF3 DI */ case 0xF3: /* TODO: implement */ break;
            /* 0xF4 UNUSED */ case 0xF4: /* TODO: implement */ break;
            /* 0xF5 PUSH AF */ case 0xF5: /* TODO: implement */ break;
            /* 0xF6 OR d8 */ case 0xF6: /* TODO: implement */ break;
            /* 0xF7 RST 30H */ case 0xF7: /* TODO: implement */ break;
            /* 0xF8 LD HL,SP+r8 */ case 0xF8: /* TODO: implement */ break;
            /* 0xF9 LD SP,HL */ case 0xF9: /* TODO: implement */ break;
            /* 0xFA LD A,(a16) */ case 0xFA: /* TODO: implement */ break;
            /* 0xFB EI */ case 0xFB: /* TODO: implement */ break;
            /* 0xFC UNUSED */ case 0xFC: /* TODO: implement */ break;
            /* 0xFD UNUSED */ case 0xFD: /* TODO: implement */ break;
            /* 0xFE CP d8 */ case 0xFE: /* TODO: implement */ break;
            /* 0xFF RST 38H */ case 0xFF: /* TODO: implement */ break;
        }
    }
}
