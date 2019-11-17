#include "simcfibmembyte.h"
#include "alufunc.h"
#include "cpu.h"
#include <stdlib.h>
#include "klee/klee.h"


void initMCArray()
{
    /*microcodeTable = malloc(size*sizeof(MicrocodeLine));
    klee_make_symbolic(microcodeTable, size*sizeof(MicrocodeLine), "ml");
    microcodeTable[0] = &suba1;
    microcodeTable[1] = &seta1;
    microcodeTable[2] = &asla1;
    microcodeTable[3] = &stop;*/
}

void initCPU(struct CPU *cpu)
{
    //zeroCPU(cpu);
    cpu->microPC = 0;
}

FLAG testCPU(struct CPU *cpu)
{
    FLAG result = 1;
    //printf("%d", cpu->regBank.registers[0]);
    // Assert that the fibonnaci sequence is actually computer in registers 3..16.
    int last2 = 0;
    int last1 = 1;
    for(int it = 2; it <= 13; it++) {
        klee_assert(cpu->memory[it] == last1 + last2);
        int temp = last1 + last2;
        last2 = last1;
        last1 = temp;
    }
    // Assert that either register 2 is the largest register, 
    // or it is reg[reg[2]] <= reg[0] <= reg[reg[2] + 1].
    /*BYTE target = cpu->regBank.registers[0];
    BYTE idx = cpu->regBank.registers[2] + 3;
    //printf("%d  ", target);
    if(idx != 16 
    && !(cpu->regBank.registers[idx] <= target && target <= cpu->regBank.registers[idx+1]))
    {
        klee_assert(0);
    }*/
    return 1;
}

FLAG stop(struct CPU* cpu)
{
    return 1;
}
// Algorithm uses A as scratch computation register
FLAG clearA(struct CPU* cpu)
{
    // Do not 0 out RB0, since this contains the "target" number.
    //cpu->regBank.registers[0] = 0;
    cpu->regBank.registers[1] = 0;
    cpu->microPC = 1;
    return 0;
}
// Algorithm uses X as scratch address register
FLAG clearX(struct CPU* cpu)
{
    cpu->regBank.registers[2] = 0;
    cpu->regBank.registers[3] = 0;
    cpu->microPC = 2;
    return 0;
}
// Algorithm uses SP as address base register
FLAG clearSP(struct CPU* cpu)
{
    cpu->regBank.registers[4] = 0;
    cpu->regBank.registers[5] = 0;
    cpu->microPC = 3;
    return 0;
}

FLAG calc0(struct CPU* cpu)
{
    // Set accumulator to 0
    cpu->regBank.registers[1] = 0;

    // Set memory address, perform memory write bypassing MDR registers.
    cpu->MARA = cpu->regBank.registers[4];
    cpu->MARB = cpu->regBank.registers[5];
    WORD address = getMARWord(cpu);
    cpu->memory[address] = cpu->regBank.registers[1];

    // Don't worry about overflow into RB4, we only compute 13 values. 
    cpu->regBank.registers[5] = byte_add_nocarry(cpu->regBank.registers[5], 1).result;

    // Update microprogram counter 
    cpu->microPC = 4;

    // Termination does not halt here.
    return 0;
}
FLAG calc1(struct CPU* cpu)
{
    // Set accumulator to 1.
    cpu->regBank.registers[1] = 1;

    // Set memory address, perform memory write bypassing MDR registers.
    cpu->MARA = cpu->regBank.registers[4];
    cpu->MARB = cpu->regBank.registers[5];
    WORD address = getMARWord(cpu);
    cpu->memory[address] = cpu->regBank.registers[1];

    // Don't worry about overflow into RB4, we only compute 13 values. 
    cpu->regBank.registers[5] = byte_add_nocarry(cpu->regBank.registers[5], 1).result;

    // Update microprogram counter 
    cpu->microPC = 5;

    // Termination does not halt here.
    return 0;
}
FLAG calcN(struct CPU* cpu)
{
    // Compute address of Fib[n-2]
    struct ALUByteResult loRes = byte_sub_nocarry(cpu->regBank.registers[5],2);
    cpu->regBank.registers[3] = loRes.result;
    cpu->regBank.registers[2] = byte_sub_carry(cpu->regBank.registers[4],0, loRes.NZVC[C]).result;

    // Fetch Fib[n-2]
    cpu->MARA = cpu->regBank.registers[2];
    cpu->MARB = cpu->regBank.registers[3];
    WORD nMinus2Address = getMARWord(cpu);

    // Add Fib[n-2] to Fib[n-1] (which is in RB1).
    cpu->regBank.registers[1] = byte_add_nocarry(cpu->regBank.registers[1], cpu->memory[nMinus2Address]).result;

    // Store result in Mem[SP]
    cpu->MARA = cpu->regBank.registers[4];
    cpu->MARB = cpu->regBank.registers[5];
    WORD nAddress = getMARWord(cpu);
    cpu->memory[nAddress] = cpu->regBank.registers[1];

    // Don't worry about overflow into RB4, we only compute 13 values. 
    cpu->regBank.registers[5] = byte_add_nocarry(cpu->regBank.registers[5], 1).result;

    // Update microprogram counter 
    cpu->microPC = 6;

    // Termination does not halt here.
    return 0;
}
FLAG compN(struct CPU* cpu)
 {
     if(cpu->regBank.registers[5] < 14) {
        cpu->microPC = 5;
     }
     else {
         cpu->microPC = 7;
     }
     return 0;
 }