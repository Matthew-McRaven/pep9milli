#include "sim.h"
#include "alufunc.h"
#include "cpu.h"
#include <stdlib.h>
#include "klee/klee.h"
FLAG suba1 (struct CPU* cpu);
FLAG asla1 (struct CPU* cpu);
FLAG stop (struct CPU* cpu);


void initMCArray()
{
    /*klee_make_symbolic(&size, sizeof(size), "size");
    size = 4;
    microcodeTable = malloc(size*sizeof(MicrocodeLine));
    microcodeTable[0] = suba1;
    microcodeTable[1] = asla1;
    microcodeTable[2] = stop;*/
}
void initCPU(struct CPU *cpu)
{
    zeroCPU(cpu);
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
    cpu->microPC = 3;
    return 1;
}

FLAG suba1 (struct CPU* cpu)
{
    BYTE* target = &(cpu->regBank.registers[0]);
    *target =  byte_asr(*target).result;
    if(*target == 0) {
        cpu->microPC = 1;
    }
    else {
        cpu->microPC = 0;
    }
    return 0;
}
FLAG seta1 (struct CPU* cpu)
{
    cpu->regBank.registers[0] =  byte_add_nocarry(cpu->regBank.registers[0], 1).result;
    cpu->microPC = 2;
    return 0;
}

FLAG asla1 (struct CPU* cpu)
{
    cpu->regBank.registers[0] =  byte_asl(cpu->regBank.registers[0]).result;
    if(cpu->regBank.registers[0] == 8) {
       cpu->microPC = 3;
    }
    else {
        cpu->microPC = 2;
    }
    return 0; 
}
