#ifndef SIM_H
#define SIM_H
#include "cpu.h"
#include "defs.h"

void initMCArray();
void initCPU(struct CPU *cpu);
FLAG testCPU(struct CPU *cpu);
// Return the next line of executable microcode.
MicrocodeLine fetch_current_line(struct CPU *cpu);
#endif
