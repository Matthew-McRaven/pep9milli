#ifndef SIM_H
#define SIM_H
#include "cpu.h"
#include "defs.h"

FLAG suba1 (struct CPU* cpu);
FLAG seta1 (struct CPU* cpu);
FLAG asla1 (struct CPU* cpu);
FLAG stop (struct CPU* cpu);


static MicrocodeLine microcodeTable[] = 
{
    suba1,
    seta1,
    asla1,
    stop
};

void initMCArray();
void initCPU(struct CPU *cpu);
FLAG testCPU(struct CPU *cpu);
#endif
