#include "sim.h"
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
    klee_assert(cpu->regBank.registers[0] == 8);
    return 1;
}

FLAG stop (struct CPU* cpu)
{
    return 1;
}

FLAG checkNeg(struct CPU* cpu)
{
    if(byte_ident(cpu->regBank.registers[0]).NZVC[N]){
        //klee_assert(cpu->regBank.registers[0] >= 128);
        cpu->microPC = 1;
    }
    else {
        //klee_assert(cpu->regBank.registers[0] <= 127);
        cpu->microPC = 2;
    }
    return 0;
}
FLAG invIfNeg(struct CPU* cpu)
{
    BYTE* target = &(cpu->regBank.registers[0]);
    //klee_assert(cpu->regBank.registers[0] >= 128);
    *target =  byte_and(*target, 127).result;
    //klee_assert(cpu->regBank.registers[0] <= 127);
    cpu->microPC = 2;
    return 0;
}

FLAG suba1 (struct CPU* cpu)
{
    BYTE* target = &(cpu->regBank.registers[0]);
    //klee_assert(cpu->regBank.registers[0] <= 127);
    *target =  byte_asr(*target).result;
    if(*target == 0) {
        //klee_assert(cpu->regBank.registers[0] == 0);
        cpu->microPC = 3;
    }
    else {
        //klee_assert(cpu->regBank.registers[0] != 0);
        cpu->microPC = 2;
    }
    return 0;
}
FLAG seta1 (struct CPU* cpu)
{
    cpu->regBank.registers[0] =  byte_add_nocarry(cpu->regBank.registers[0], 1).result;
    //klee_assert(cpu->regBank.registers[0] == 1);
    cpu->microPC = 4;
    return 0;
}

FLAG asla1 (struct CPU* cpu)
{
    //klee_assert(cpu->regBank.registers[0] < 8);
    cpu->regBank.registers[0] =  byte_asl(cpu->regBank.registers[0]).result;

    if(cpu->regBank.registers[0] == 8) {
        //klee_assert(cpu->regBank.registers[0] == 8);
        cpu->microPC = 5;
    }
    else {
        cpu->microPC = 4;
    }
    return 0; 
}
