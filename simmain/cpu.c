#include "cpu.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// Initialize all values in a CPU to zero
void zeroCPU(struct CPU* cpu)
{
    cpu->MARA = 0;
    cpu->MARB = 0;
    cpu->MDRE = 0;
    cpu->MDRO = 0;
    cpu->microPC = 0;
    //
    memset(cpu->regBank.registers, 0 ,sizeof(cpu->regBank.registers));
    memset(cpu->PSNVCbits, 0 ,sizeof(cpu->PSNVCbits));
}

void zeroMemory(struct CPU* cpu)
{
    memset(cpu->memory, 0 ,sizeof(cpu->memory));
}

/*
 * Functions to read state of CPU.
 */
// Read CPU bank registers
WORD getNamedRegisterWord(struct RegisterBank* regBank, enum Registers reg)
{
    return getRegisterWord(regBank, reg);
}

BYTE getNamedRegisterByte(struct RegisterBank* regBank, enum Registers reg)
{
    return getRegisterByte(regBank, reg);
}

WORD getRegisterWord(struct RegisterBank* regBank, uint8_t regNumber)
{
    assert(regNumber<=30);
    BYTE* address = &regBank->registers[regNumber];
    return *((WORD*) address);
}

BYTE getRegisterByte(struct RegisterBank* regBank, uint8_t regNumber)
{
    assert(regNumber<=31);
    BYTE* address = &regBank->registers[regNumber];
    return *address;
}

// Read memory registers
WORD getMARWord(struct CPU* cpu)
{
    return (WORD) (cpu->MARA << 8 | cpu->MARB);
}

WORD getMDREO(struct CPU* cpu)
{
    return (WORD)(cpu->MDRE << 8 | cpu->MDRO);
}
BYTE getMDR(struct CPU* cpu, enum Registers which)
{
    switch (which)
    {
    case MDRE:
        return cpu->MDRE;
        break;
    case MDRO:
        return cpu->MDRO;
        break;
    default:
        assert(0);
    }
}

// Read status bits
FLAG getFlag(struct CPU* cpu, enum StatusBits flag)
{
    switch (flag)
    {
    case P:
    case S:
    case N:
    case Z:
    case V:
    case C:
        return cpu->PSNVCbits[flag];
        break;
    default:
        assert(0);
    }
}


/*
 * Functions to write to the CPU.
 */
// Write CPU bank registers
void setNamedRegisterWord(struct RegisterBank* regBank, enum Registers reg, WORD value)
{
   setRegisterWord(regBank, reg, value);
}

void setNamedRegisterByte(struct RegisterBank* regBank, enum Registers reg, BYTE value)
{
   setRegisterByte(regBank, reg, value);
}

void setRegisterWord(struct RegisterBank* regBank, uint8_t regNumber, WORD value)
{
    assert(regNumber<=30);
    BYTE* address = &regBank->registers[regNumber];
    *((WORD*) address) = value; 
}

void setRegisterByte(struct RegisterBank* regBank, uint8_t regNumber, BYTE value)
{
    assert(regNumber<=31);
    BYTE* address = &regBank->registers[regNumber];
    *address = value;
}

// Write memory registers
void setMARWord(struct CPU* cpu, WORD value)
{
    //BYTE splitMARA = (BYTE) (value >> 8);
    //BYTE splitMARB = (BYTE) value;
    cpu->MARA = (BYTE) (value >> 8);
    cpu->MARB = (BYTE) value;
}
void setMDREO(struct CPU* cpu, WORD value)
{
    //BYTE splitMDRE = (BYTE) (value >> 8);
    //BYTE splitMDRO = (BYTE) value;
    cpu->MDRE = (BYTE) (value >> 8);
    cpu->MDRO = (BYTE) value;
}
void setMDR(struct CPU* cpu, enum Registers which, BYTE value) {
    switch (which)
    {
    case MDRE:
        cpu->MDRE = value;
        break;
    case MDRO:
        cpu->MDRO = value;
        break;
    default:
        assert(0);
    }
}

// Write status bits
void setFlag(struct CPU* cpu, enum StatusBits flag, FLAG value)
{
    switch (flag)
    {
    case P:
    case S:
    case N:
    case Z:
    case V:
    case C:
        cpu->PSNVCbits[flag] = value ? 1 : 0;
        break;
    default:
        assert(0);
    }
}