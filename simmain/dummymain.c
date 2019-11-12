#include <assert.h>
#include <stdio.h>

#include "alufunc.h"
#include "cpu.h"
#include "defs.h"
#include "klee/klee.h"
#include "sim.h"

FLAG executeSim(struct CPU *cpu)
{
    FLAG finished = 0;
    initCPU(cpu);
    while(!finished) {
        printf("%d",cpu->microPC);
        finished = microcodeTable[cpu->microPC](cpu);
    }
    return testCPU(cpu);
}

int main(int argv, char** argc)
{
    // Declare symbolic registers
    struct CPU cpu;
    klee_make_symbolic(&cpu, sizeof(cpu), "CPU");
    zeroCPU(&cpu);
    executeSim(&cpu);
    return 0;
}