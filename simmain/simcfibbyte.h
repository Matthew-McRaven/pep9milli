#ifndef SIM_H
#define SIM_H
#include "cpu.h"
#include "defs.h"

FLAG clearX(struct CPU* cpu);
FLAG stop(struct CPU* cpu);
FLAG f00(struct CPU* cpu);
FLAG f01(struct CPU* cpu);
FLAG f02(struct CPU* cpu);
FLAG f03(struct CPU* cpu);
FLAG f04(struct CPU* cpu);
FLAG f05(struct CPU* cpu);
FLAG f06(struct CPU* cpu);
FLAG f07(struct CPU* cpu);
FLAG f08(struct CPU* cpu);
FLAG f09(struct CPU* cpu);
FLAG f10(struct CPU* cpu);
FLAG f11(struct CPU* cpu);
FLAG f12(struct CPU* cpu);
FLAG f13(struct CPU* cpu);
FLAG dummy(struct CPU* cpu);
FLAG dummy(struct CPU* cpu);
FLAG dummy(struct CPU* cpu);
FLAG c00(struct CPU* cpu);
FLAG c01(struct CPU* cpu);
FLAG c02(struct CPU* cpu);
FLAG c03(struct CPU* cpu);
FLAG c04(struct CPU* cpu);
FLAG c05(struct CPU* cpu);
FLAG c06(struct CPU* cpu);
FLAG c07(struct CPU* cpu);
FLAG c08(struct CPU* cpu);
FLAG c09(struct CPU* cpu);
FLAG c10(struct CPU* cpu);
FLAG c11(struct CPU* cpu);
FLAG c12(struct CPU* cpu);
FLAG c13(struct CPU* cpu);


static MicrocodeLine microcodeTable[] = 
{
    clearX, //00
    f00, //01
    f01, //02
    f02, //03
    f03, //04
    f04, //05
    f05, //06
    f06, //07
    f07, //08
    f08, //09
    f09, //10
    f10, //11
    f11, //12
    f12, //13
    f13, //14
    stop, //15
    c00, //16
    c01, //17
    c02, //18
    c03, //19
    c04, //20
    c05, //21
    c06, //22
    c07, //23
    c08, //24
    c09, //25
    c10, //26
    c11, //27
    c12, //28
    c13, //29
};

void initMCArray();
void initCPU(struct CPU *cpu);
FLAG testCPU(struct CPU *cpu);
#endif
