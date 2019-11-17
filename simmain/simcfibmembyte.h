#ifndef SIM_H
#define SIM_H
#include "cpu.h"
#include "defs.h"
// Algorithm uses A as scratch computation register
FLAG clearA(struct CPU* cpu);
// Algorithm uses X as scratch address register
FLAG clearX(struct CPU* cpu);
// Algorithm uses SP as address base register
FLAG clearSP(struct CPU* cpu);
FLAG stop(struct CPU* cpu);
// Calculate the first fibonnaci number, and place it in the first address.
FLAG calc0(struct CPU* cpu);
FLAG calc1(struct CPU* cpu);
FLAG calcN(struct CPU* cpu);
FLAG compN(struct CPU* cpu);


static MicrocodeLine microcodeTable[] = 
{
    clearA,     //00
    clearX,     //01
    clearSP,    //02
    calc0,      //03
    calc1,      //04
    calcN,      //05
    compN,      //06
    stop,       //07
};

void initMCArray();
void initCPU(struct CPU *cpu);
FLAG testCPU(struct CPU *cpu);
#endif
