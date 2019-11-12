#ifndef SIM_H
#define SIM_H
#include "cpu.h"
#include "defs.h"

FLAG nop0 (struct CPU* cpu);
FLAG nop1 (struct CPU* cpu);
FLAG stop (struct CPU* cpu);

static MicrocodeLine microcodeTable[3] = 
{
    nop0,
    nop1,
    stop
};
void initCPU(struct CPU *cpu);
FLAG testCPU(struct CPU *cpu);
#endif
