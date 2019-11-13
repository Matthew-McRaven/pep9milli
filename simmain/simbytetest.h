#ifndef SIM_H
#define SIM_H
#include "cpu.h"
#include "defs.h"

FLAG checkNeg(struct CPU* cpu);
FLAG invIfNeg(struct CPU* cpu);
FLAG suba1 (struct CPU* cpu);
FLAG seta1 (struct CPU* cpu);
FLAG asla1 (struct CPU* cpu);
FLAG stop (struct CPU* cpu);


static MicrocodeLine microcodeTable[] = 
{
    checkNeg, //0
    invIfNeg, //1
    suba1, //2
    seta1, //3
    asla1, //4
    stop //5
};

void initMCArray();
void initCPU(struct CPU *cpu);
FLAG testCPU(struct CPU *cpu);
#endif
