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
    WORD baseAddress = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
    //printf("BADDR %d\n", baseAddress);
    for(int it = 2; it <= 13; it++) {
        /*printf("It: %d. EX %d. ADDR %d. ACT %d\n", it, last1+last2,
            baseAddress + it % 0x10000,
            cpu->memory[baseAddress + it % 0x10000]);*/
        klee_assert(memory[(baseAddress + it) % 0x10000] == last1 + last2);
        int temp = last1 + last2;
        last2 = last1;
        last1 = temp;
    }
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
    // Without guidance, klee will check every possible memory address.
    // I am here to guide it, so it must less dump decisions.
    // I manually guide it to "interesting" memory locations.
    BYTE x;
    klee_make_symbolic(&x, sizeof(x), "choice");
    klee_assume(x<=5);
    if(x == 0) {cpu->regBank.registers[6] = 0x00 ; cpu->regBank.registers[7] = 0x00;}
    else if (x == 1) {cpu->regBank.registers[6] = 0x00; cpu->regBank.registers[7] = 0xfc;}
    else if (x == 2) {cpu->regBank.registers[6] = 0xdd; cpu->regBank.registers[7] = 0xe1;}
    else if (x == 3) {cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xde;}
    else if (x == 4) {cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xf4;}
    else {cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xfe;}
    //cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xf2;

    WORD baseAddress = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
    //f("BADDR %d\n", baseAddress);
    // Use register 6,7 as symbolic "base" registers.
    cpu->regBank.registers[4] = cpu->regBank.registers[6];
    cpu->regBank.registers[5] = cpu->regBank.registers[7];
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
    memory[address] = cpu->regBank.registers[1];
    //printf("M[%d]=%d\n",address, cpu->regBank.registers[1]);

    // Increment base address.
    struct ALUByteResult loRes = byte_add_nocarry(cpu->regBank.registers[5], 1);
    cpu->regBank.registers[5] = loRes.result;
    cpu->regBank.registers[4] = byte_add_carry(cpu->regBank.registers[4], 0, loRes.NZVC[C]).result;

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
    memory[address] = cpu->regBank.registers[1];
    //printf("M[%d]=%d\n",address, cpu->regBank.registers[1]);

    // Increment base address.
    struct ALUByteResult loRes = byte_add_nocarry(cpu->regBank.registers[5], 1);
    cpu->regBank.registers[5] = loRes.result;
    cpu->regBank.registers[4] = byte_add_carry(cpu->regBank.registers[4], 0, loRes.NZVC[C]).result;

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
    //printf("Fib[n-2] = %d\n", cpu->memory[nMinus2Address]);
    cpu->regBank.registers[1] = byte_add_nocarry(cpu->regBank.registers[1], memory[nMinus2Address]).result;

    // Store result in Mem[SP]
    cpu->MARA = cpu->regBank.registers[4];
    cpu->MARB = cpu->regBank.registers[5];
    WORD nAddress = getMARWord(cpu);
    memory[nAddress] = cpu->regBank.registers[1];
    //printf("M[%d]=%d\n",nAddress, cpu->regBank.registers[1]);

    // Increment base address.
    struct ALUByteResult BAloRes = byte_add_nocarry(cpu->regBank.registers[5], 1);
    cpu->regBank.registers[5] = BAloRes.result;
    cpu->regBank.registers[4] = byte_add_carry(cpu->regBank.registers[4], 0, BAloRes.NZVC[C]).result;

    // Update microprogram counter 
    cpu->microPC = 6;

    // Termination does not halt here.
    return 0;
}
FLAG compN(struct CPU* cpu)
 {
     WORD baseAddress = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
     WORD offset      = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
     struct ALUWordResult res = word_sub_nocarry(offset, 13 + baseAddress);
     // Will overflow from uint16_t to int_32t, making my day suck.
     if((WORD)(offset - baseAddress)  <= 13 ) {
        cpu->microPC = 5;
     }
     else {
         cpu->microPC = 7;
     }
     return 0;
 }