#ifndef CPU_H
#define CPU_H
#include "defs.h"
enum Registers
{
    A = 0, X = 2, SP = 4, PC = 6, 
    IS = 8, OS = 9, PF = 10,
    MARA = 32, MARB = 33, MDRE = 34, MDRO = 35,
    MICROPC = 36,
    // Choose NONE, if you don't want an operation to be clocked into any register.
    NONE = 100
};
enum StatusBits 
{
    // Control section status bits
    P = 5, 
    // Status bits at the MC level
    S = 4, 
    // Status bits at the assembly level
    N = 3, Z = 2, V = 1, C = 0
};

struct RegisterBank
{
    BYTE registers[32];
};

struct CPU
{

    // Data section
    struct RegisterBank regBank;
    FLAG PSNVCbits[6];
    BYTE MARA, MARB;
    BYTE MDRE, MDRO;

    // Control section
    MCAddress microPC;

    // Indirect branching table units
    FLAG is_unary_decoder[256];
    MCAddress addressing_mode_decoder[256];
};

// Initialize all values in a CPU to zero
void zeroCPU(struct CPU* cpu);
void init_static_regs(struct CPU *cpu);
/*
 * Functions to read state of CPU.
 */
// Read CPU bank registers
WORD getNamedRegisterWord(struct RegisterBank* regBank, enum Registers reg);
BYTE getNamedRegisterByte(struct RegisterBank* regBank, enum Registers reg);
WORD getRegisterWord(struct RegisterBank* regBank, uint8_t regNumber);
BYTE getRegisterByte(struct RegisterBank* regBank, uint8_t regNumber);

// Read memory registers
WORD getMARWord(struct CPU* cpu);
WORD getMDREO(struct CPU* cpu);
BYTE getMDR(struct CPU* cpu, enum Registers which);

// Read status bits
FLAG getFlag(struct CPU* cpu, enum StatusBits flag);


/*
 * Functions to write to the CPU.
 */
// Write CPU bank registers
void setNamedRegisterWord(struct RegisterBank* regBank, enum Registers reg, WORD value);
void setNamedRegisterByte(struct RegisterBank* regBank, enum Registers reg, BYTE value);
void setRegisterWord(struct RegisterBank* regBank, uint8_t regNumber, WORD value);
void setRegisterByte(struct RegisterBank* regBank, uint8_t regNumber, BYTE value);

// Write memory registers
void setMARWord(struct CPU* cpu, WORD value);
void setMDREO(struct CPU* cpu, WORD value);
void setMDR(struct CPU* cpu, enum Registers which, BYTE value);

// Write status bits
void setFlag(struct CPU* cpu, enum StatusBits flag, FLAG value);

#endif