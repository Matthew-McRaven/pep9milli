#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "cpufunc.h"
#include "klee/klee.h"
#include "memory.h"
#include "model.h"

#include <stdlib.h>


// Algorithm uses A as scratch computation register
FLAG clearA(struct VerificationModel* model);
// Algorithm uses X as scratch address register
FLAG clearX(struct VerificationModel* model);
// Algorithm uses SP as address base register
FLAG clearSP(struct VerificationModel* model);
FLAG stop(struct VerificationModel* model);
// Calculate the first fibonnaci number, and place it in the first address.
FLAG calc0(struct VerificationModel* model);
FLAG calc1(struct VerificationModel* model);
FLAG calcN(struct VerificationModel* model);
FLAG compN(struct VerificationModel* model);

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

void init_model(struct VerificationModel *model)
{
    //zeroCPU(cpu);
    model->cpu->microPC = 0;
    init_static_regs(model->cpu);
}

FLAG test_model(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;
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
        klee_assert(memory->memory[(baseAddress + it) % 0x10000] == last1 + last2);
        int temp = last1 + last2;
        last2 = last1;
        last1 = temp;
    }
    return 1;
}

MicrocodeLine fetch_current_line(struct CPU *cpu)
{
    return microcodeTable[cpu->microPC];
}

FLAG stop(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Stop, 0, 0);
}
// Algorithm uses A as scratch computation register
FLAG clearA(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Do not 0 out RB0, since this contains the "target" number.
    //cpu->regBank.registers[0] = 0;
    cpu_byte_ident(cpu, 22, 1, 0, 0, 0);
    return cpu_update_UPC(cpu, AUTO_INCR, 0, 0);
}
// Algorithm uses X as scratch address register
FLAG clearX(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, 22, 2, 0, 0, 0);
    cpu_byte_ident(cpu, 22, 3, 0, 0, 0);
    return cpu_update_UPC(cpu, AUTO_INCR, 0, 0);
}
// Algorithm uses SP as address base register
FLAG clearSP(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Without guidance, klee will check every possible memory address.
    // I am here to guide it, so it must less dump decisions.
    // I manually guide it to "interesting" memory locations.
    /*BYTE x;
    klee_make_symbolic(&x, sizeof(x), "choice");
    klee_assume(x<=5);
    if(x == 0) {cpu->regBank.registers[6] = 0x00 ; cpu->regBank.registers[7] = 0x00;}
    else if (x == 1) {cpu->regBank.registers[6] = 0x00; cpu->regBank.registers[7] = 0xfc;}
    else if (x == 2) {cpu->regBank.registers[6] = 0xdd; cpu->regBank.registers[7] = 0xe1;}
    else if (x == 3) {cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xde;}
    else if (x == 4) {cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xf4;}
    else {cpu->regBank.registers[6] = 0xff; cpu->regBank.registers[7] = 0xfe;}*/

    WORD baseAddress = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];

    // Copy the base address from RW6 to RW4.
    cpu_byte_ident(cpu, 6, 4, 0, 0, 0);
    cpu_byte_ident(cpu, 7, 5, 0, 0, 0);
    return cpu_update_UPC(cpu, AUTO_INCR, 0, 0);
}

FLAG calc0(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Set accumulator<lo> to 0
    cpu_byte_ident(cpu, 22, 1, 0, 0, 0);

    // Move RW4 to MAR
    cpu_move_to_mar(cpu, 4, 5);
    cpu_byte_ident(cpu, 1, MDRE, 0, 0, 0);
    mem_dbg_write_mdre(cpu, memory);

    // Increment base address.
    cpu_byte_add_nocarry(cpu, 5, 23, 5, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 4, 22, 4, S, 0, 0, 0, 0, 0, 0);

    // Update microprogram counter 
    return cpu_update_UPC(cpu, AUTO_INCR, 0, 0);

}
FLAG calc1(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Set accumulator to 1.
    cpu_byte_ident(cpu, 23, 1, 0, 0, 0);

    // Set memory address, perform memory write bypassing MDR registers.
    cpu_move_to_mar(cpu, 4, 5);
    cpu_byte_ident(cpu, 1, MDRE, 0, 0, 0);
    mem_dbg_write_mdre(cpu, memory);

    // Increment base address.
    cpu_byte_add_nocarry(cpu, 5, 23, 5, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 4, 22, 4, S, 0, 0, 0, 0, 0, 0);

    // Update microprogram counter 
    cpu_update_UPC(cpu, AUTO_INCR, 0, 0);

    // Termination does not halt here.
    return 0;
}
FLAG calcN(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Compute address of Fib[n-2]
    cpu_byte_sub_nocarry(cpu, 5, 24, 3, 0, 0, 0, 0, 0, 1);
    cpu_byte_sub_carry(cpu, 4, 22, 2, S, 0, 0, 0, 0, 0, 0);

    // Fetch Fib[n-2]
    cpu_move_to_mar(cpu, 2, 3);
    mem_dbg_read_into_mdre(cpu, memory);
    cpu_byte_add_nocarry(cpu, MDRE, 1, 1, 0, 0, 0, 0, 0, 0);

    // Store result in Mem[SP]
    cpu_move_to_mar(cpu, 4, 5);
    cpu_byte_ident(cpu, 1, MDRE, 0, 0, 0);
    mem_dbg_write_mdre(cpu, memory);

    // Increment base address.
    cpu_byte_add_nocarry(cpu, 5, 23, 5, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 4, 22, 4, S, 0, 0, 0, 0, 0, 0);

    // Update microprogram counter 
    cpu_update_UPC(cpu, AUTO_INCR, 0, 0);

    // Termination does not halt here.
    return 0;
}
FLAG compN(struct VerificationModel *model)
 {
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

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
