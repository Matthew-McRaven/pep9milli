#include "simcfibregbyte.h"
#include "alufunc.h"
#include "cpu.h"
#include <stdlib.h>
#include "klee/klee.h"


void initMCArray()
{
    /*microcodeTable = malloc(size*sizeof(MicrocodeLine));
    klee_make_symbolic(microcodeTable, size*sizeof(MicrocodeLine), "ml");
    microcodeTable[0] = &suba1;
    microcodeTable[1] = &seta1;
    microcodeTable[2] = &asla1;
    microcodeTable[3] = &stop;*/
}

void initCPU(struct CPU *cpu)
{
    //zeroCPU(cpu);
    cpu->microPC = 0;
}

FLAG testCPU(struct CPU *cpu)
{
    FLAG result = 1;
    //printf("%d", cpu->regBank.registers[0]);
    // Assert that the fibonnaci sequence is actually computer in registers 3..16.
    int last2 = 0;
    int last1 = 1;
    for(int it = 2; it <= 13; it++) {
        klee_assert(cpu->regBank.registers[it+3] == last1 + last2);
        int temp = last1 + last2;
        last2 = last1;
        last1 = temp;
    }
    // Assert that either register 2 is the largest register, 
    // or it is reg[reg[2]] <= reg[0] <= reg[reg[2] + 1].
    BYTE target = cpu->regBank.registers[0];
    BYTE idx = cpu->regBank.registers[2] + 3;
    //printf("%d  ", target);
    if(idx != 16 
    && !(cpu->regBank.registers[idx] <= target && target <= cpu->regBank.registers[idx+1]))
    {
        //printf("Target %d\n", target);
        //printf("Index %d\n", idx);
        //printf("At %d\n", cpu->regBank.registers[idx+1]);
        //printf("Next %d\n", cpu->regBank.registers[idx]);
        klee_assert(0);
    }
    return 1;
}

FLAG clearX(struct CPU* cpu)
{
    cpu->regBank.registers[2] = 0;
    cpu->regBank.registers[17] = 255;
    cpu->microPC = 1;
    return 0;
}
FLAG stop(struct CPU* cpu)
{
    return 1;
}
FLAG f00(struct CPU* cpu)
{
    int at = 3;
    cpu->regBank.registers[at] = 0;
    cpu->microPC = at - 1;
    return 0;
}
FLAG f01(struct CPU* cpu)
{
    int at = 4;
    cpu->regBank.registers[at] = 1;
    cpu->microPC = at - 1;
    return 0;
}
FLAG f02(struct CPU* cpu)
{
    int at = 5;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f03(struct CPU* cpu)
{
    int at = 6;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f04(struct CPU* cpu)
{
    int at = 7;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f05(struct CPU* cpu)
{
    int at = 8;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f06(struct CPU* cpu)
{
    int at = 9;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f07(struct CPU* cpu)
{
    int at = 10;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f08(struct CPU* cpu)
{
    int at = 11;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f09(struct CPU* cpu)
{
    int at = 12;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f10(struct CPU* cpu)
{
    int at = 13;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f11(struct CPU* cpu)
{
    int at = 14;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f12(struct CPU* cpu)
{
    int at = 15;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f13(struct CPU* cpu)
{
    int at = 16;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = 16; // Jump to stop
    return 0;
}

FLAG c00(struct CPU* cpu)
{
    int at = 3;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    cpu->regBank.registers[2] = 0;
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        //printf("Current %d\n", cpu->regBank.registers[at]);
        //printf("Target %d\n", cpu->regBank.registers[0]);
        //printf("Difference %d\n", byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]).result);
        klee_assert(cpu->regBank.registers[0] == 0);
        cpu->microPC = 15;
    } else {
        klee_assert(cpu->regBank.registers[0] != 0);
        cpu->microPC = at + 14;
    }  
    return 0;
}

FLAG c01(struct CPU* cpu)
{
    int at = 4;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        klee_assert(cpu->regBank.registers[0] == 1);
        cpu->microPC = 15;
    } else {
        klee_assert(cpu->regBank.registers[0] > 1);
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c02(struct CPU* cpu)
{
    int at = 5;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c03(struct CPU* cpu)
{
    int at = 6;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c04(struct CPU* cpu)
{
    int at = 7;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c05(struct CPU* cpu)
{
    int at = 8;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c06(struct CPU* cpu) 
{
    int at = 9;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c07(struct CPU* cpu)
{
    int at = 10;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c08(struct CPU* cpu)
{
    int at = 11;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c09(struct CPU* cpu)
{
    int at = 12;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c10(struct CPU* cpu)
{
    int at = 13;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c11(struct CPU* cpu)
{
    int at = 14;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c12(struct CPU* cpu)
{
    int at = 15;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c13(struct CPU* cpu)
{
    cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
    cpu->microPC = 15;
    return 0;
}