#include "sim.h"
#include "alufunc.h"
#include "cpu.h"

void initMCArray()
{
    klee_make_symbolic(&size, sizeof(size), "size");
    size = 4;
    microcodeTable = malloc(size*sizeof(MicrocodeLine));
    microcodeTable[0] = nop0;
    microcodeTable[1] = nop1;
    microcodeTable[2] = stop;
}
void initCPU(struct CPU *cpu)
{
    zeroCPU(cpu);
    cpu->microPC = 0;
}

FLAG testCPU(struct CPU *cpu)
{
    return 1;
}

FLAG nop0 (struct CPU* cpu)
{
    cpu->microPC = 1;
    return 0;
}

FLAG nop1 (struct CPU* cpu)
{
    cpu->microPC = 2;
    return 0; 
}
FLAG stop (struct CPU* cpu)
{
    return 1;
}
