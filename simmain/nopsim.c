#include "sim.h"
#include "alufunc.h"
#include "cpu.h"

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
