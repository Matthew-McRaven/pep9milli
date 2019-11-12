#include "sim.h"
#include "alufunc.h"
#include "cpu.h"

FLAG suba1 (struct CPU* cpu);
FLAG asla1 (struct CPU* cpu);
FLAG stop (struct CPU* cpu);

MicrocodeLine* microcodeTable =
[
    &add1,
    &asl1,
    &stop
]
void initCPU(struct CPU *cpu)
{
    zeroCPU(cpu);
}

void testCPU(struct CPU *cpu)
{

}

FLAG suba1 (struct CPU* cpu)
{
    WORD* target = &(cpu->regBank[0]);
    *target =  word_sub_carry(*target, 0, 1).result;
    if(*target == 0) {
        cpu->microPC = 1;
    }
    return 0;
}

FLAG asla1 (struct CPU* cpu)
{
    WORD* target = &(cpu->regBank[0]);
    *target =  word_asl(*target).result;
    if(*target == 8) {
       cpu->microPC = 2;
    }
    return 0; 
}
FLAG stop (struct CPU* cpu)
{
    return 1;
}
