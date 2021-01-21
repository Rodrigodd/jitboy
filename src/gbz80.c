#include <SDL.h>

#include "gbz80.h"

static gbz80_inst inst_table[] = {
    /* clang-format off */
/* CODE     OPCODE ARG1    ARG2    PTR/ADDR BYTES CYCLES  FLAGS */
  [0x00] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x01] = {LD16,  REG_BC, IMM16,  0, 0,   3,     3, 3,   0},
  [0x02] = {LD,    MEM_BC, REG_A,  0, 0,   1,     2, 2,   0},
  [0x03] = {INC16, REG_BC, NONE,   0, 0,   1,     2, 2,   0},
  [0x04] = {INC,   REG_B,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x05] = {DEC,   REG_B,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x06] = {LD,    REG_B,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x07] = {RLCA,  REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x08] = {LD16,  MEM_16, REG_SP, 0, 0,   3,     5, 5,   0},
  [0x09] = {ADD16, REG_HL, REG_BC, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x0a] = {LD,    REG_A,  MEM_BC, 0, 0,   1,     2, 2,   0},
  [0x0b] = {DEC16, REG_BC, NONE,   0, 0,   1,     2, 2,   0},
  [0x0c] = {INC,   REG_C,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x0d] = {DEC,   REG_C,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x0e] = {LD,    REG_C,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x0f] = {RRCA,  REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x10] = {STOP,  NONE,   NONE,   0, 0,   2,     1, 1,   INST_FLAG_ENDS_BLOCK},
  [0x11] = {LD16,  REG_DE, IMM16,  0, 0,   3,     3, 3,   0},
  [0x12] = {LD,    MEM_DE, REG_A,  0, 0,   1,     2, 2,   0},
  [0x13] = {INC16, REG_DE, NONE,   0, 0,   1,     2, 2,   0},
  [0x14] = {INC,   REG_D,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x15] = {DEC,   REG_D,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x16] = {LD,    REG_D,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x17] = {RLA,   REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x18] = {JR,    NONE,   IMM8,   0, 0,   2,     3, 3,   INST_FLAG_ENDS_BLOCK},
  [0x19] = {ADD16, REG_HL, REG_DE, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x1a] = {LD,    REG_A,  MEM_DE, 0, 0,   1,     2, 2,   0},
  [0x1b] = {DEC16, REG_DE, NONE,   0, 0,   1,     2, 2,   0},
  [0x1c] = {INC,   REG_E,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1d] = {DEC,   REG_E,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1e] = {LD,    REG_E,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x1f] = {RRA,   REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x20] = {JR,    CC_NZ,  IMM8,   0, 0,   2,     3, 2,   INST_FLAG_USES_CC},
  [0x21] = {LD16,  REG_HL, IMM16,  0, 0,   3,     3, 3,   0},
  [0x22] = {LD,    MEM_INC_HL, REG_A, 0,0, 1,     2, 2,   0},
  [0x23] = {INC16, REG_HL, NONE,   0, 0,   1,     2, 2,   0},
  [0x24] = {INC,   REG_H,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x25] = {DEC,   REG_H,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x26] = {LD,    REG_H,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x27] = {DAA,   NONE,   NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x28] = {JR,    CC_Z,   IMM8,   0, 0,   2,     3, 2,   INST_FLAG_USES_CC},
  [0x29] = {ADD16, REG_HL, REG_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x2a] = {LD,    REG_A,  MEM_INC_HL, 0,0,1,     2, 2,   0},
  [0x2b] = {DEC16, REG_HL, NONE,   0, 0,   1,     2, 2,   0},
  [0x2c] = {INC,   REG_L,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x2d] = {DEC,   REG_L,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x2e] = {LD,    REG_L,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x2f] = {CPL,   REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x30] = {JR,    CC_NC,  IMM8,   0, 0,   2,     3, 2,   INST_FLAG_USES_CC},
  [0x31] = {LD16,  REG_SP, IMM16,  0, 0,   3,     3, 3,   0},
  [0x32] = {LD,    MEM_DEC_HL, REG_A, 0,0, 1,     2, 2,   0},
  [0x33] = {INC16, REG_SP, NONE,   0, 0,   1,     2, 2,   0},
  [0x34] = {INC,   MEM_HL, NONE,   0, 0,   1,     3, 3,   INST_FLAG_AFFECTS_CC},
  [0x35] = {DEC,   MEM_HL, NONE,   0, 0,   1,     3, 3,   INST_FLAG_AFFECTS_CC},
  [0x36] = {LD,    MEM_HL, IMM8,   0, 0,   2,     3, 3,   0},
  [0x37] = {SCF,   NONE,   NONE,   0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x38] = {JR,    CC_C,   IMM8,   0, 0,   2,     3, 2,   INST_FLAG_USES_CC},
  [0x39] = {ADD16, REG_HL, REG_SP, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x3a] = {LD,    REG_A,  MEM_DEC_HL, 0,0,1,     2, 2,   0},
  [0x3b] = {DEC16, REG_SP, NONE,   0, 0,   1,     2, 2,   0},
  [0x3c] = {INC,   REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x3d] = {DEC,   REG_A,  NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x3e] = {LD,    REG_A,  IMM8,   0, 0,   2,     2, 2,   0},
  [0x3f] = {CCF,   NONE,   NONE,   0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x40] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x41] = {LD,    REG_B,  REG_C,  0, 0,   1,     1, 1,   0},
  [0x42] = {LD,    REG_B,  REG_D,  0, 0,   1,     1, 1,   0},
  [0x43] = {LD,    REG_B,  REG_E,  0, 0,   1,     1, 1,   0},
  [0x44] = {LD,    REG_B,  REG_H,  0, 0,   1,     1, 1,   0},
  [0x45] = {LD,    REG_B,  REG_L,  0, 0,   1,     1, 1,   0},
  [0x46] = {LD,    REG_B,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x47] = {LD,    REG_B,  REG_A,  0, 0,   1,     1, 1,   0},
  [0x48] = {LD,    REG_C,  REG_B,  0, 0,   1,     1, 1,   0},
  [0x49] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x4a] = {LD,    REG_C,  REG_D,  0, 0,   1,     1, 1,   0},
  [0x4b] = {LD,    REG_C,  REG_E,  0, 0,   1,     1, 1,   0},
  [0x4c] = {LD,    REG_C,  REG_H,  0, 0,   1,     1, 1,   0},
  [0x4d] = {LD,    REG_C,  REG_L,  0, 0,   1,     1, 1,   0},
  [0x4e] = {LD,    REG_C,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x4f] = {LD,    REG_C,  REG_A,  0, 0,   1,     1, 1,   0},
  [0x50] = {LD,    REG_D,  REG_B,  0, 0,   1,     1, 1,   0},
  [0x51] = {LD,    REG_D,  REG_C,  0, 0,   1,     1, 1,   0},
  [0x52] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x53] = {LD,    REG_D,  REG_E,  0, 0,   1,     1, 1,   0},
  [0x54] = {LD,    REG_D,  REG_H,  0, 0,   1,     1, 1,   0},
  [0x55] = {LD,    REG_D,  REG_L,  0, 0,   1,     1, 1,   0},
  [0x56] = {LD,    REG_D,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x57] = {LD,    REG_D,  REG_A,  0, 0,   1,     1, 1,   0},
  [0x58] = {LD,    REG_E,  REG_B,  0, 0,   1,     1, 1,   0},
  [0x59] = {LD,    REG_E,  REG_C,  0, 0,   1,     1, 1,   0},
  [0x5a] = {LD,    REG_E,  REG_D,  0, 0,   1,     1, 1,   0},
  [0x5b] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x5c] = {LD,    REG_E,  REG_H,  0, 0,   1,     1, 1,   0},
  [0x5d] = {LD,    REG_E,  REG_L,  0, 0,   1,     1, 1,   0},
  [0x5e] = {LD,    REG_E,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x5f] = {LD,    REG_E,  REG_A,  0, 0,   1,     1, 1,   0},
  [0x60] = {LD,    REG_H,  REG_B,  0, 0,   1,     1, 1,   0},
  [0x61] = {LD,    REG_H,  REG_C,  0, 0,   1,     1, 1,   0},
  [0x62] = {LD,    REG_H,  REG_D,  0, 0,   1,     1, 1,   0},
  [0x63] = {LD,    REG_H,  REG_E,  0, 0,   1,     1, 1,   0},
  [0x64] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x65] = {LD,    REG_H,  REG_L,  0, 0,   1,     1, 1,   0},
  [0x66] = {LD,    REG_H,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x67] = {LD,    REG_H,  REG_A,  0, 0,   1,     1, 1,   0},
  [0x68] = {LD,    REG_L,  REG_B,  0, 0,   1,     1, 1,   0},
  [0x69] = {LD,    REG_L,  REG_C,  0, 0,   1,     1, 1,   0},
  [0x6a] = {LD,    REG_L,  REG_D,  0, 0,   1,     1, 1,   0},
  [0x6b] = {LD,    REG_L,  REG_E,  0, 0,   1,     1, 1,   0},
  [0x6c] = {LD,    REG_L,  REG_H,  0, 0,   1,     1, 1,   0},
  [0x6d] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x6e] = {LD,    REG_L,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x6f] = {LD,    REG_L,  REG_A,  0, 0,   1,     1, 1,   0},
  [0x70] = {LD,    MEM_HL, REG_B,  0, 0,   1,     1, 1,   0},
  [0x71] = {LD,    MEM_HL, REG_C,  0, 0,   1,     1, 1,   0},
  [0x72] = {LD,    MEM_HL, REG_D,  0, 0,   1,     1, 1,   0},
  [0x73] = {LD,    MEM_HL, REG_E,  0, 0,   1,     1, 1,   0},
  [0x74] = {LD,    MEM_HL, REG_H,  0, 0,   1,     1, 1,   0},
  [0x75] = {LD,    MEM_HL, REG_L,  0, 0,   1,     1, 1,   0},
  [0x76] = {HALT,  NONE,   NONE,   0, 0,   1,     1, 1,   INST_FLAG_ENDS_BLOCK},
  [0x77] = {LD,    MEM_HL, REG_A,  0, 0,   1,     1, 1,   0},
  [0x78] = {LD,    REG_A,  REG_B,  0, 0,   1,     1, 1,   0},
  [0x79] = {LD,    REG_A,  REG_C,  0, 0,   1,     1, 1,   0},
  [0x7a] = {LD,    REG_A,  REG_D,  0, 0,   1,     1, 1,   0},
  [0x7b] = {LD,    REG_A,  REG_E,  0, 0,   1,     1, 1,   0},
  [0x7c] = {LD,    REG_A,  REG_H,  0, 0,   1,     1, 1,   0},
  [0x7d] = {LD,    REG_A,  REG_L,  0, 0,   1,     1, 1,   0},
  [0x7e] = {LD,    REG_A,  MEM_HL, 0, 0,   1,     2, 2,   0},
  [0x7f] = {NOP,   NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0x80] = {ADD,   REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x81] = {ADD,   REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x82] = {ADD,   REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x83] = {ADD,   REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x84] = {ADD,   REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x85] = {ADD,   REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x86] = {ADD,   REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x87] = {ADD,   REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x88] = {ADC,   REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x89] = {ADC,   REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x8a] = {ADC,   REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x8b] = {ADC,   REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x8c] = {ADC,   REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x8d] = {ADC,   REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x8e] = {ADC,   REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x8f] = {ADC,   REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x90] = {SUB,   REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x91] = {SUB,   REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x92] = {SUB,   REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x93] = {SUB,   REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x94] = {SUB,   REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x95] = {SUB,   REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x96] = {SUB,   REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x97] = {SUB,   REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0x98] = {SBC,   REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x99] = {SBC,   REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x9a] = {SBC,   REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x9b] = {SBC,   REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x9c] = {SBC,   REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x9d] = {SBC,   REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x9e] = {SBC,   REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x9f] = {SBC,   REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0xa0] = {AND,   REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa1] = {AND,   REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa2] = {AND,   REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa3] = {AND,   REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa4] = {AND,   REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa5] = {AND,   REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa6] = {AND,   REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xa7] = {AND,   REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa8] = {XOR,   REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xa9] = {XOR,   REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xaa] = {XOR,   REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xab] = {XOR,   REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xac] = {XOR,   REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xad] = {XOR,   REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xae] = {XOR,   REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xaf] = {XOR,   REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb0] = {OR,    REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb1] = {OR,    REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb2] = {OR,    REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb3] = {OR,    REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb4] = {OR,    REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb5] = {OR,    REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb6] = {OR,    REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xb7] = {OR,    REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb8] = {CP,    REG_A,  REG_B,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xb9] = {CP,    REG_A,  REG_C,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xba] = {CP,    REG_A,  REG_D,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xbb] = {CP,    REG_A,  REG_E,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xbc] = {CP,    REG_A,  REG_H,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xbd] = {CP,    REG_A,  REG_L,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xbe] = {CP,    REG_A,  MEM_HL, 0, 0,   1,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xbf] = {CP,    REG_A,  REG_A,  0, 0,   1,     1, 1,   INST_FLAG_AFFECTS_CC},
  [0xc0] = {RET,   CC_NZ,  NONE,   0, 0,   1,     5, 2,   INST_FLAG_USES_CC},
  [0xc1] = {POP,   REG_BC, NONE,   0, 0,   1,     3, 3,   0},
  [0xc2] = {JP,    CC_NZ,  IMM16,  0, 0,   3,     4, 3,   INST_FLAG_USES_CC},
  [0xc3] = {JP,    NONE,   IMM16,  0, 0,   3,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xc4] = {CALL,  CC_NZ,  IMM16,  0, 0,   3,     6, 3,   INST_FLAG_USES_CC},
  [0xc5] = {PUSH,  REG_BC, NONE,   0, 0,   1,     4, 4,   0},
  [0xc6] = {ADD,   REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xc7] = {RST,   NONE,   MEM_0x00, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xc8] = {RET,   CC_Z,   NONE,   0, 0,   1,     5, 2,   INST_FLAG_USES_CC},
  [0xc9] = {RET,   NONE,   NONE,   0, 0,   1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xca] = {JP,    CC_Z,   IMM16,  0, 0,   3,     4, 3,   INST_FLAG_USES_CC},
  [0xcb] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xcc] = {CALL,  CC_Z,   IMM16,  0, 0,   3,     6, 3,   INST_FLAG_USES_CC},
  [0xcd] = {CALL,  NONE,   IMM16,  0, 0,   3,     6, 6,   INST_FLAG_ENDS_BLOCK},
  [0xce] = {ADC,   REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0xcf] = {RST,   NONE,   MEM_0x08, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xd0] = {RET,   CC_NC,  NONE,   0, 0,   1,     5, 2,   INST_FLAG_USES_CC},
  [0xd1] = {POP,   REG_DE, NONE,   0, 0,   1,     3, 3,   0},
  [0xd2] = {JP,    CC_NC,  IMM16,  0, 0,   3,     4, 3,   INST_FLAG_USES_CC},
  [0xd3] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xd4] = {CALL,  CC_NC,  IMM16,  0, 0,   3,     6, 3,   INST_FLAG_USES_CC},
  [0xd5] = {PUSH,  REG_DE, NONE,   0, 0,   1,     4, 4,   0},
  [0xd6] = {SUB,   REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xd7] = {RST,   NONE,   MEM_0x10, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xd8] = {RET,   CC_C,   NONE,   0, 0,   1,     5, 2,   INST_FLAG_USES_CC},
  [0xd9] = {RETI,  NONE,   NONE,   0, 0,   1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xda] = {JP,    CC_C,   IMM16,  0, 0,   3,     4, 3,   INST_FLAG_USES_CC},
  [0xdb] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xdc] = {CALL,  CC_C,   IMM16,  0, 0,   3,     6, 3,   INST_FLAG_USES_CC},
  [0xdd] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xde] = {SBC,   REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0xdf] = {RST,   NONE,   MEM_0x18, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xe0] = {LD,    MEM_8,  REG_A,  0, 0,   2,     3, 3,   0},
  [0xe1] = {POP,   REG_HL, NONE,   0, 0,   1,     3, 3,   0},
  [0xe2] = {LD,    MEM_C,  REG_A,  0, 0,   1,     2, 2,   0},
  [0xe3] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xe4] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xe5] = {PUSH,  REG_HL, NONE,   0, 0,   1,     4, 4,   0},
  [0xe6] = {AND,   REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xe7] = {RST,   NONE,   MEM_0x20, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xe8] = {ADD16, REG_SP, IMM8,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0xe9] = {JP,    NONE,   MEM_HL, 0, 0,   1,     1, 1,   INST_FLAG_ENDS_BLOCK},
  [0xea] = {LD,    MEM_16, REG_A,  0, 0,   3,     4, 4,   0},
  [0xeb] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xec] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xed] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xee] = {XOR,   REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xef] = {RST,   NONE,   MEM_0x28, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xf0] = {LD,    REG_A,  MEM_8,  0, 0,   2,     3, 3,   0},
  [0xf1] = {POP,   REG_AF, NONE,   0, 0,   1,     3, 3,   INST_FLAG_AFFECTS_CC},
  [0xf2] = {LD,    REG_A,  MEM_C,  0, 0,   1,     2, 2,   0},
  [0xf3] = {DI,    NONE,   NONE,   0, 0,   1,     1, 1,   0},
  [0xf4] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xf5] = {PUSH,  REG_AF, NONE,   0, 0,   1,     4, 4,   INST_FLAG_USES_CC},
  [0xf6] = {OR,    REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xf7] = {RST,   NONE,   MEM_0x30, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK},
  [0xf8] = {LD16,  REG_HL, MEM_8,  0, 0,   2,     3, 3,   INST_FLAG_AFFECTS_CC},
  [0xf9] = {LD16,  REG_SP, REG_HL, 0, 0,   1,     2, 2,   0},
  [0xfa] = {LD,    REG_A,  MEM_16, 0, 0,   3,     4, 4,   0},
  [0xfb] = {EI,    NONE,   NONE,   0, 0,   1,     1, 1,   INST_FLAG_ENDS_BLOCK},
#ifdef INSTRUCTION_TEST
  /* To do code generating for flag setting and flag reloading which are 
   * required for the instruction tester. Opcodes `0xfc` and `0xfd` which 
   * are invalid opcodes in original Game Boy are diverted for the purpose */
  [0xfc] = {SET_F, NONE,   NONE,   0, 0,   0,     0, 0,   INST_FLAG_AFFECTS_CC},
  [0xfd] = {LD_F,  NONE,   NONE,   0, 0,   0,     0, 0,   INST_FLAG_USES_CC},
#else
  [0xfc] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
  [0xfd] = {ERROR, NONE,   NONE,   0, 0,   0,     0, 0,   0},
#endif
  [0xfe] = {CP,    REG_A,  IMM8,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0xff] = {RST,   NONE,   MEM_0x38, 0, 0, 1,     4, 4,   INST_FLAG_ENDS_BLOCK}
    /* clang-format off */
};

static gbz80_inst cb_table[] = {
    /* clang-format off */
/* CODE     OPCODE ARG1    ARG2    ARGPTR  BYTES  CYCLES  FLAGS */
  [0x00] = {RLC,   REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x01] = {RLC,   REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x02] = {RLC,   REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x03] = {RLC,   REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x04] = {RLC,   REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x05] = {RLC,   REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x06] = {RLC,   MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x07] = {RLC,   REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x08] = {RRC,   REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x09] = {RRC,   REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x0a] = {RRC,   REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x0b] = {RRC,   REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x0c] = {RRC,   REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x0d] = {RRC,   REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x0e] = {RRC,   MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x0f] = {RRC,   REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x10] = {RL,    REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x11] = {RL,    REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x12] = {RL,    REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x13] = {RL,    REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x14] = {RL,    REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x15] = {RL,    REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x16] = {RL,    MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x17] = {RL,    REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x18] = {RR,    REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x19] = {RR,    REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1a] = {RR,    REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1b] = {RR,    REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1c] = {RR,    REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1d] = {RR,    REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1e] = {RR,    MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x1f] = {RR,    REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_USES_CC|INST_FLAG_AFFECTS_CC},
  [0x20] = {SLA,   REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x21] = {SLA,   REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x22] = {SLA,   REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x23] = {SLA,   REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x24] = {SLA,   REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x25] = {SLA,   REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x26] = {SLA,   MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x27] = {SLA,   REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x28] = {SRA,   REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x29] = {SRA,   REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x2a] = {SRA,   REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x2b] = {SRA,   REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x2c] = {SRA,   REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x2d] = {SRA,   REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x2e] = {SRA,   MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x2f] = {SRA,   REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x30] = {SWAP,  REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x31] = {SWAP,  REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x32] = {SWAP,  REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x33] = {SWAP,  REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x34] = {SWAP,  REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x35] = {SWAP,  REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x36] = {SWAP,  MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x37] = {SWAP,  REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x38] = {SRL,   REG_B,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x39] = {SRL,   REG_C,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x3a] = {SRL,   REG_D,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x3b] = {SRL,   REG_E,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x3c] = {SRL,   REG_H,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x3d] = {SRL,   REG_L,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x3e] = {SRL,   MEM_HL, NONE,   0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x3f] = {SRL,   REG_A,  NONE,   0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x40] = {BIT,   REG_B,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x41] = {BIT,   REG_C,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x42] = {BIT,   REG_D,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x43] = {BIT,   REG_E,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x44] = {BIT,   REG_H,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x45] = {BIT,   REG_L,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x46] = {BIT,   MEM_HL, BIT_0,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x47] = {BIT,   REG_A,  BIT_0,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x48] = {BIT,   REG_B,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x49] = {BIT,   REG_C,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x4a] = {BIT,   REG_D,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x4b] = {BIT,   REG_E,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x4c] = {BIT,   REG_H,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x4d] = {BIT,   REG_L,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x4e] = {BIT,   MEM_HL, BIT_1,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x4f] = {BIT,   REG_A,  BIT_1,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x50] = {BIT,   REG_B,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x51] = {BIT,   REG_C,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x52] = {BIT,   REG_D,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x53] = {BIT,   REG_E,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x54] = {BIT,   REG_H,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x55] = {BIT,   REG_L,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x56] = {BIT,   MEM_HL, BIT_2,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x57] = {BIT,   REG_A,  BIT_2,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x58] = {BIT,   REG_B,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x59] = {BIT,   REG_C,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x5a] = {BIT,   REG_D,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x5b] = {BIT,   REG_E,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x5c] = {BIT,   REG_H,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x5d] = {BIT,   REG_L,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x5e] = {BIT,   MEM_HL, BIT_3,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x5f] = {BIT,   REG_A,  BIT_3,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x60] = {BIT,   REG_B,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x61] = {BIT,   REG_C,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x62] = {BIT,   REG_D,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x63] = {BIT,   REG_E,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x64] = {BIT,   REG_H,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x65] = {BIT,   REG_L,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x66] = {BIT,   MEM_HL, BIT_4,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x67] = {BIT,   REG_A,  BIT_4,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x68] = {BIT,   REG_B,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x69] = {BIT,   REG_C,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x6a] = {BIT,   REG_D,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x6b] = {BIT,   REG_E,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x6c] = {BIT,   REG_H,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x6d] = {BIT,   REG_L,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x6e] = {BIT,   MEM_HL, BIT_5,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x6f] = {BIT,   REG_A,  BIT_5,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x70] = {BIT,   REG_B,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x71] = {BIT,   REG_C,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x72] = {BIT,   REG_D,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x73] = {BIT,   REG_E,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x74] = {BIT,   REG_H,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x75] = {BIT,   REG_L,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x76] = {BIT,   MEM_HL, BIT_6,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x77] = {BIT,   REG_A,  BIT_6,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x78] = {BIT,   REG_B,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x79] = {BIT,   REG_C,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x7a] = {BIT,   REG_D,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x7b] = {BIT,   REG_E,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x7c] = {BIT,   REG_H,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x7d] = {BIT,   REG_L,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x7e] = {BIT,   MEM_HL, BIT_7,  0, 0,   2,     4, 4,   INST_FLAG_AFFECTS_CC},
  [0x7f] = {BIT,   REG_A,  BIT_7,  0, 0,   2,     2, 2,   INST_FLAG_AFFECTS_CC},
  [0x80] = {RES,   REG_B,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x81] = {RES,   REG_C,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x82] = {RES,   REG_D,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x83] = {RES,   REG_E,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x84] = {RES,   REG_H,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x85] = {RES,   REG_L,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x86] = {RES,   MEM_HL, BIT_0,  0, 0,   2,     4, 4,   0},
  [0x87] = {RES,   REG_A,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0x88] = {RES,   REG_B,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x89] = {RES,   REG_C,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x8a] = {RES,   REG_D,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x8b] = {RES,   REG_E,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x8c] = {RES,   REG_H,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x8d] = {RES,   REG_L,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x8e] = {RES,   MEM_HL, BIT_1,  0, 0,   2,     4, 4,   0},
  [0x8f] = {RES,   REG_A,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0x90] = {RES,   REG_B,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x91] = {RES,   REG_C,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x92] = {RES,   REG_D,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x93] = {RES,   REG_E,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x94] = {RES,   REG_H,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x95] = {RES,   REG_L,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x96] = {RES,   MEM_HL, BIT_2,  0, 0,   2,     4, 4,   0},
  [0x97] = {RES,   REG_A,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0x98] = {RES,   REG_B,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0x99] = {RES,   REG_C,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0x9a] = {RES,   REG_D,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0x9b] = {RES,   REG_E,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0x9c] = {RES,   REG_H,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0x9d] = {RES,   REG_L,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0x9e] = {RES,   MEM_HL, BIT_3,  0, 0,   2,     4, 4,   0},
  [0x9f] = {RES,   REG_A,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xa0] = {RES,   REG_B,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa1] = {RES,   REG_C,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa2] = {RES,   REG_D,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa3] = {RES,   REG_E,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa4] = {RES,   REG_H,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa5] = {RES,   REG_L,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa6] = {RES,   MEM_HL, BIT_4,  0, 0,   2,     4, 4,   0},
  [0xa7] = {RES,   REG_A,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xa8] = {RES,   REG_B,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xa9] = {RES,   REG_C,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xaa] = {RES,   REG_D,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xab] = {RES,   REG_E,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xac] = {RES,   REG_H,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xad] = {RES,   REG_L,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xae] = {RES,   MEM_HL, BIT_5,  0, 0,   2,     4, 4,   0},
  [0xaf] = {RES,   REG_A,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xb0] = {RES,   REG_B,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb1] = {RES,   REG_C,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb2] = {RES,   REG_D,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb3] = {RES,   REG_E,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb4] = {RES,   REG_H,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb5] = {RES,   REG_L,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb6] = {RES,   MEM_HL, BIT_6,  0, 0,   2,     4, 4,   0},
  [0xb7] = {RES,   REG_A,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xb8] = {RES,   REG_B,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xb9] = {RES,   REG_C,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xba] = {RES,   REG_D,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xbb] = {RES,   REG_E,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xbc] = {RES,   REG_H,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xbd] = {RES,   REG_L,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xbe] = {RES,   MEM_HL, BIT_7,  0, 0,   2,     4, 4,   0},
  [0xbf] = {RES,   REG_A,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xc0] = {SET,   REG_B,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc1] = {SET,   REG_C,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc2] = {SET,   REG_D,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc3] = {SET,   REG_E,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc4] = {SET,   REG_H,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc5] = {SET,   REG_L,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc6] = {SET,   MEM_HL, BIT_0,  0, 0,   2,     4, 4,   0},
  [0xc7] = {SET,   REG_A,  BIT_0,  0, 0,   2,     2, 2,   0},
  [0xc8] = {SET,   REG_B,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xc9] = {SET,   REG_C,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xca] = {SET,   REG_D,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xcb] = {SET,   REG_E,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xcc] = {SET,   REG_H,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xcd] = {SET,   REG_L,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xce] = {SET,   MEM_HL, BIT_1,  0, 0,   2,     4, 4,   0},
  [0xcf] = {SET,   REG_A,  BIT_1,  0, 0,   2,     2, 2,   0},
  [0xd0] = {SET,   REG_B,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd1] = {SET,   REG_C,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd2] = {SET,   REG_D,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd3] = {SET,   REG_E,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd4] = {SET,   REG_H,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd5] = {SET,   REG_L,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd6] = {SET,   MEM_HL, BIT_2,  0, 0,   2,     4, 4,   0},
  [0xd7] = {SET,   REG_A,  BIT_2,  0, 0,   2,     2, 2,   0},
  [0xd8] = {SET,   REG_B,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xd9] = {SET,   REG_C,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xda] = {SET,   REG_D,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xdb] = {SET,   REG_E,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xdc] = {SET,   REG_H,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xdd] = {SET,   REG_L,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xde] = {SET,   MEM_HL, BIT_3,  0, 0,   2,     4, 4,   0},
  [0xdf] = {SET,   REG_A,  BIT_3,  0, 0,   2,     2, 2,   0},
  [0xe0] = {SET,   REG_B,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe1] = {SET,   REG_C,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe2] = {SET,   REG_D,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe3] = {SET,   REG_E,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe4] = {SET,   REG_H,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe5] = {SET,   REG_L,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe6] = {SET,   MEM_HL, BIT_4,  0, 0,   2,     4, 4,   0},
  [0xe7] = {SET,   REG_A,  BIT_4,  0, 0,   2,     2, 2,   0},
  [0xe8] = {SET,   REG_B,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xe9] = {SET,   REG_C,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xea] = {SET,   REG_D,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xeb] = {SET,   REG_E,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xec] = {SET,   REG_H,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xed] = {SET,   REG_L,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xee] = {SET,   MEM_HL, BIT_5,  0, 0,   2,     4, 4,   0},
  [0xef] = {SET,   REG_A,  BIT_5,  0, 0,   2,     2, 2,   0},
  [0xf0] = {SET,   REG_B,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf1] = {SET,   REG_C,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf2] = {SET,   REG_D,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf3] = {SET,   REG_E,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf4] = {SET,   REG_H,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf5] = {SET,   REG_L,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf6] = {SET,   MEM_HL, BIT_6,  0, 0,   2,     4, 4,   0},
  [0xf7] = {SET,   REG_A,  BIT_6,  0, 0,   2,     2, 2,   0},
  [0xf8] = {SET,   REG_B,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xf9] = {SET,   REG_C,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xfa] = {SET,   REG_D,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xfb] = {SET,   REG_E,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xfc] = {SET,   REG_H,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xfd] = {SET,   REG_L,  BIT_7,  0, 0,   2,     2, 2,   0},
  [0xfe] = {SET,   MEM_HL, BIT_7,  0, 0,   2,     4, 4,   0},
  [0xff] = {SET,   REG_A,  BIT_7,  0, 0,   2,     2, 2,   0},
    /* clang-format on */
};

static bool optimize_cc(GList *inst)
{
    for (; inst; inst = inst->next) {
        if (DATA(inst)->flags & INST_FLAG_AFFECTS_CC)
            DATA(inst)->flags |= INST_FLAG_SAVE_CC;

        if (DATA(inst)->flags & INST_FLAG_USES_CC)
            DATA(inst)->flags |= INST_FLAG_RESTORE_CC;
    }

    return true;
}

/* compiles block starting at start_address to gb_block */
bool compile(gb_block *block,
             gb_memory *mem,
             uint16_t start_address,
             int opt_level)
{
    LOG_DEBUG("compile new block @%#x\n", start_address);

    GList *instructions = NULL;

    uint16_t i = start_address;
    for (;;) {
        gbz80_inst *inst = g_new(gbz80_inst, 1);

        uint8_t opcode = mem->mem[i];
        if (opcode != 0xcb) {
            *inst = inst_table[opcode];
        } else {
            opcode = mem->mem[i + 1];
            *inst = cb_table[opcode];
        }

        inst->args = mem->mem + i;
        inst->address = i;
        i += inst->bytes;

        if (inst->opcode == ERROR) {
            LOG_ERROR("Invalid Opcode! (%#x)\n", opcode);
            return false;
        }
        LOG_DEBUG("inst: %i @%#x\n", inst->opcode, inst->address);

        instructions = g_list_prepend(instructions, inst);

        if (inst->flags & INST_FLAG_ENDS_BLOCK)
            break;
    }

    instructions = g_list_reverse(instructions);

    if (!optimize_block(&instructions, opt_level))
        return false;

    if (!optimize_cc(instructions))
        return false;

    bool result = emit(block, instructions);

    g_list_free_full(instructions, g_free);

    return result;
}

#ifdef INSTRUCTION_TEST
struct instr_info compile_and_run(gb_block *block,
                                  gb_memory *mem,
                                  uint16_t pc,
                                  uint8_t *flag_args)
{
    GList *instructions = NULL;

    /* run the fake instruction for flag setting */
    gbz80_inst *inst_before = g_new(gbz80_inst, 1);
    *inst_before = inst_table[0xfc];
    inst_before->args = flag_args;
    inst_before->address = -1;
    instructions = g_list_prepend(instructions, inst_before);

    gbz80_inst *inst = g_new(gbz80_inst, 1);

    uint8_t opcode = mem->mem[pc];
    bool is_cb = false;
    if (opcode != 0xcb) {
        *inst = inst_table[opcode];
    } else {
        is_cb = true;
        opcode = mem->mem[pc + 1];
        *inst = cb_table[opcode];
    }

    inst->args = mem->mem + pc;
    inst->address = pc;
    uint8_t bytes = inst->bytes;
    int cycles = inst->cycles;

    if (inst->opcode == ERROR) {
        LOG_ERROR("Invalid Opcode! (%#x)\n", opcode);
        exit(1);
    }

    instructions = g_list_prepend(instructions, inst);
    instructions = g_list_reverse(instructions);

    if (!optimize_cc(instructions)) {
        exit(1);
    }

    bool result = emit(block, instructions);
    g_list_free_full(instructions, g_free);

    if (result == false) {
        LOG_ERROR("Fail to compile instruction\n");
        exit(1);
    }

    return (struct instr_info){is_cb, opcode, bytes, cycles};
}

/* build_load_flag_block is used for generating JIT codes to load status flag.
 * Because the JIT codes for flag loading doesn't need any arguments, so we can
 * build it at the init stage and then cache it for reusing to avoid spending
 * time on avoidable codegen.
 */
void build_load_flag_block(gb_block *block)
{
    GList *instructions = NULL;

    /* run the fake instruction for flag loading */
    gbz80_inst *inst = g_new(gbz80_inst, 1);
    *inst = inst_table[0xfd];
    inst->args = NULL;
    inst->address = -1;
    instructions = g_list_prepend(instructions, inst);
    instructions = g_list_reverse(instructions);

    if (!optimize_cc(instructions)) {
        exit(1);
    }

    bool result = emit(block, instructions);
    g_list_free_full(instructions, g_free);

    if (result == false) {
        LOG_ERROR("Fail to compile instruction\n");
        exit(1);
    }
}
#endif
