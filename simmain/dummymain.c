#include <assert.h>
#include <stdio.h>

#include "alufunc.h"
#include "cpu.h"
#include "defs.h"
#include "klee/klee.h"
//#include "simbytetest.h"
//#include "simcfibbyte.h"
#include "sim.h"

FLAG executeSim(struct CPU *cpu)
{
    FLAG finished = 0;
    initCPU(cpu);
    cpu->microPC = 0;
    while(!finished) {
        finished = fetch_current_line(cpu)(cpu);
    }
    return testCPU(cpu);
}

int main(int argv, char** argc)
{
    // Declare symbolic registers
    struct CPU cpu;
    klee_make_symbolic(&cpu, sizeof(cpu), "CPU");
    //zeroCPU(&cpu);
    initMCArray();
    executeSim(&cpu);
    return 0;
}