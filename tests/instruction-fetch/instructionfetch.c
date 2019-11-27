#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "cpufunc.h"
#include "klee/klee.h"
#include "memory.h"
#include "model.h"

#include <stdlib.h>


// Determine whether to take even or odd instruction fetch path.
FLAG determine_even_odd(struct VerificationModel* model);
// Begin even fetch, load PC from mem
FLAG even_fetch(struct VerificationModel* model);
// Set the prefetch valid bit to true.
FLAG set_prefetch_valid(struct VerificationModel* model);
// Move the value in MDRE to IS.
FLAG even_move_to_IS(struct VerificationModel* model);
// Move the value in MDRO to T1.
FLAG even_move_to_T1(struct VerificationModel* model);

// Determine if the prefetch should be used or not.
FLAG odd_fetch(struct VerificationModel* model);
// Prefetch is valid, so move T1 to IS.
FLAG PF_Valid(struct VerificationModel* model);

// Prefetch is invalid, so initiate memory fetch.
FLAG PF_Invalid(struct VerificationModel* model);
// Move MDRO to IS.
FLAG odd_move_to_IS(struct VerificationModel* model);

// Increment program counter.
FLAG end(struct VerificationModel* model);

// Halt execution
FLAG stop(struct VerificationModel* model);

static MicrocodeLine microcodeTable[] = 
{
    determine_even_odd,     //00
    even_fetch,             //01
    set_prefetch_valid,     //02
    even_move_to_IS,        //03
    even_move_to_T1,        //04
    odd_fetch,              //05
    PF_Valid,               //06
    PF_Invalid,             //07
    odd_move_to_IS,         //08
    end,                    //09
    stop,                   //10
};

WORD starting_PC;
BYTE IS_Value;

void init_model(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Create state variables to compare against at the end of execution.
    klee_make_symbolic(&starting_PC, sizeof(starting_PC), "Starting PC");
    klee_make_symbolic(&IS_Value, sizeof(IS_Value), "IS Value");

    // Initialize constant registers 22-31.
    init_static_regs(model->cpu);

    // Set the PC to some (random) initial value.
    cpu->regBank.registers[6] = (BYTE) (starting_PC >> 8) & 0xff;
    cpu->regBank.registers[7] = (BYTE) (starting_PC & 0xff);

    //klee_assume(starting_PC%2 == 1);
    // Fuse the PC into a single word address.
    WORD baseAddress = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
    // Assign memory address at PC some comparable value.
    
    memory->memory[starting_PC] = IS_Value;

    // We may now assume that the address has the correct value loaded.
    klee_assert(memory->memory[starting_PC] == IS_Value);

    // If our prefetch is valid, then initialize prefetch correctly.
    if(cpu->PSNVCbits[P]) {
        cpu->regBank.registers[11] = IS_Value;
    }
    // Tell klee that either the 
    /*klee_assume(
        (!cpu->PSNVCbits[P]) || (cpu->PSNVCbits[P] && cpu->regBank.registers[11] == memory->memory[baseAddress])
    );*/
}

FLAG test_model(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;
    FLAG result = 1;

    WORD final_address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];

    // Assert that program counter was incremented correctly.
    // Must use (WORD), otherwise WORD will widen to int32_t, causing evaluation errors.
    klee_assert((WORD)final_address == (WORD)(starting_PC + 1));

    // Assert that the instruction specifier was loaded.
    klee_assert(cpu->regBank.registers[8] == IS_Value);
    
    // Assert that if we started at an even address, the prefetch is now TRUE, and it is the correct value.
    // Clever use of OR used as an implication.
    klee_assert((!starting_PC%2==0) || (cpu->PSNVCbits[P] && cpu->regBank.registers[11] == memory->memory[starting_PC+1]));
    return 1;
}

MicrocodeLine fetch_current_line(struct CPU *cpu)
{
    return microcodeTable[cpu->microPC];
}

// Determine whether to take even or odd instruction fetch path.
FLAG determine_even_odd(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 7, NONE, 0, 0, 0, 0, 0, 1);

    WORD address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
    // If program counter is odd, follow odd path.
    if(cpu->PSNVCbits[S]) {
        // Assert that address is odd.
        klee_assert(address % 2 == 1);
        cpu->microPC = 5;
    }
    // Otherwise follow even path.
    else {
        // Assert that address is even.
        klee_assert(address % 2 == 0);
        cpu->microPC = 1;
    }
    return 0;
}

// Begin even fetch, load PC from mem
FLAG even_fetch(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 6, 7);
    mem_read_word(cpu, memory, 1, 1);

    cpu->microPC = 2;
    return 0;
}

// Set the prefetch valid bit to true.
FLAG set_prefetch_valid(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_set_prefetch_flag(cpu, 1);

    cpu->microPC = 3;

    return 0;
}
// Move the value in MDRE to IS.
FLAG even_move_to_IS(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRE, 8, 0, 0, 0);

    cpu->microPC = 4;

    return 0;
}

// Move the value in MDRO to T1.
FLAG even_move_to_T1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRO, 11, 0, 0, 0);
    cpu->microPC = 9;

    return 0;
}

// Determine if the prefetch should be used or not.
FLAG odd_fetch(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // If prefetch is valid
    if(cpu->PSNVCbits[P]) {
        cpu->microPC = 6;
    }
    // If prefetch is invalid
    else {
        cpu->microPC = 7;
    }

    return 0;
}

// Prefetch is valid, so move T1 to IS.
FLAG PF_Valid(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, 11, 8, 0, 0, 0);

    cpu->microPC = 9;

    return 0;
}

// Prefetch is invalid, so initiate memory fetch.
FLAG PF_Invalid(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 6, 7);
    mem_read_word(cpu, memory, 1, 1);

    cpu->microPC = 8;

    return 0;
}
// Move MDRO to IS.
FLAG odd_move_to_IS(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_byte_ident(cpu, MDRO, 8, 0, 0, 0);
    cpu->microPC = 9;

    return 0;
}

// Increment program counter.
FLAG end(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Increment PC by 1.
    cpu_byte_add_nocarry(cpu, 7, 23, 7, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 6, 22, 6, S, 0, 0, 0, 0, 0, 0);

    cpu->microPC = 10;

    return 0;
}

FLAG stop(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Stop, 0, 0);
}