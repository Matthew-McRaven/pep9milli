#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "cpufunc.h"
#include "klee/klee.h"
#include "memory.h"
#include "model.h"

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 


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

// Increment program counter, decide if instr is unary.
FLAG decode_un(struct VerificationModel* model);

// Determine if PC is even or odd.
FLAG opr_fetch(struct VerificationModel* model);

// Load operand word if PC is even.
FLAG even_opr(struct VerificationModel* model);
// Move MDRE to RB9.
FLAG even_move_rb9(struct VerificationModel* model);
// Move MDREO to RB10.
FLAG even_move_rb10(struct VerificationModel* model);
// Increment PC by 2.
FLAG even_incr_PC2(struct VerificationModel* model);

// Determine if prefetch is valid
FLAG odd_opr(struct VerificationModel* model);
// Increment PC by 2.
FLAG nxtftch(struct VerificationModel* model);
// Fetch MDRE/O at PC.
FLAG odd_memread(struct VerificationModel* model);
// Move MDRE to RB10.
FLAG odd_move_rb10(struct VerificationModel* model);
// Move MDRO to P.
FLAG odd_move_p(struct VerificationModel* model);
// Set the prefetch valid bit to true.
FLAG odd_set_pf(struct VerificationModel* model);

// Halt execution
FLAG end(struct VerificationModel* model);

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
    decode_un,              //09
    opr_fetch,              //10
    even_opr,               //11
    even_move_rb9,          //12
    even_move_rb10,         //13
    even_incr_PC2,          //14
    odd_opr,                //15
    nxtftch,                //16
    odd_memread,            //17
    odd_move_rb10,          //18
    odd_move_p,             //29
    odd_set_pf,             //20
    end,                    //21
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

    // klee_assume(starting_PC%2 == 0);
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
    // Default assume all instructions are unary.
    memset(cpu->is_unary_decoder, 0, 256);
    // Instructions from 0 to 17 are unary.
    // Instructions from 18-37 are non-unary.
    for(int it=18; it <= 37; it++) cpu->is_unary_decoder[it] = 1;
    // Instructions from 38 to 79 are unary.
    for(int it=80; it <= 255; it++) cpu->is_unary_decoder[it] = 1;

}

FLAG test_model(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;
    FLAG result = 1;

    WORD final_address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];

    // Assert that the instruction specifier was loaded.
    klee_assert(cpu->regBank.registers[8] == IS_Value);
    if(cpu->is_unary_decoder[cpu->regBank.registers[8]]) {
        // Assert that program counter was incremented correctly.
        // Must use (WORD), otherwise WORD will widen to int32_t, causing evaluation errors.
        klee_assert((WORD)final_address == (WORD)(starting_PC + 1));
        // Assert that if we started at an even address, the prefetch is now TRUE, and it is the correct value.
        // Clever use of OR used as an implication.
        klee_assert((!starting_PC%2==0) || (cpu->PSNVCbits[P] && cpu->regBank.registers[11] == memory->memory[starting_PC+1]));
    }
    else {
        // Assert that program counter was incremented correctly.
        // Must use (WORD), otherwise WORD will widen to int32_t, causing evaluation errors.
        klee_assert((WORD)final_address == (WORD)(starting_PC + 3));
        klee_assert(cpu->regBank.registers[9] == memory->memory[(WORD)(starting_PC + 1)]);
        klee_assert(cpu->regBank.registers[10] == memory->memory[(WORD)(starting_PC + 2)]); 
        klee_assert(!(starting_PC % 2 == 0) | (cpu->PSNVCbits[P] && cpu->regBank.registers[11] == memory->memory[(WORD)(starting_PC + 3)]));
    }
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

    return cpu_update_UPC(cpu, BRS, 5, 1);
}

// Begin even fetch, load PC from mem
FLAG even_fetch(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 6, 7);
    mem_read_word(cpu, memory, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 2, 2);
}

// Set the prefetch valid bit to true.
FLAG set_prefetch_valid(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_set_prefetch_flag(cpu, 1);

    return cpu_update_UPC(cpu, Unconditional, 3, 3);
}

// Move the value in MDRE to IS.
FLAG even_move_to_IS(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRE, 8, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 4, 4);
}

// Move the value in MDRO to T1.
FLAG even_move_to_T1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRO, 11, 0, 0, 0);
    return cpu_update_UPC(cpu, Unconditional, 9, 9);
}

// Determine if the prefetch should be used or not.
FLAG odd_fetch(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, IsPrefetchValid, 6, 7);
}

// Prefetch is valid, so move T1 to IS.
FLAG PF_Valid(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, 11, 8, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 9, 9);
}

// Prefetch is invalid, so initiate memory fetch.
FLAG PF_Invalid(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 6, 7);
    mem_read_word(cpu, memory, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 8, 8);
}
// Move MDRO to IS.
FLAG odd_move_to_IS(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_byte_ident(cpu, MDRO, 8, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 9, 9);
}

// Increment program counter, decide if instr is unary.
FLAG decode_un(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Increment PC by 1.
    cpu_byte_add_nocarry(cpu, 7, 23, 7, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 6, 22, 6, S, 0, 0, 0, 0, 0, 0);

    // If unary, go to STOP().
    return cpu_update_UPC(cpu, IsUnary, 21, 10);
}

FLAG opr_fetch(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 7, NONE, 0, 0, 0, 0, 0, 1);

    WORD address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];

    // If program counter is odd, follow odd path.
    return cpu_update_UPC(cpu, BRS, 15, 11);
}

FLAG even_opr(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 6, 7);
    mem_read_word(cpu, memory, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 12, 12);
}

// Move MDRE to RB9.
FLAG even_move_rb9(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_byte_ident(cpu, MDRE, 9, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 13, 13);
}

// Move MDREO to RB10.
FLAG even_move_rb10(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_byte_ident(cpu, MDRO, 10, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 14, 14);
}

// Increment PC by 2.
FLAG even_incr_PC2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Increment PC by 2.
    cpu_byte_add_nocarry(cpu, 7, 24, 7, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 6, 22, 6, S, 0, 0, 0, 0, 0, 0);

     return cpu_update_UPC(cpu, Unconditional, 21, 21);
}

// Determine if prefetch is valid
FLAG odd_opr(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    WORD address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
     // Assert that address is odd.
    klee_assert(address % 2 == 1);
    klee_assert(memory->memory[address] == cpu->regBank.registers[11]);
    cpu_byte_ident(cpu, 11, 9, 0, 0, 0);
    klee_assert(memory->memory[address] == cpu->regBank.registers[9]);
    klee_assert(address == (WORD)(starting_PC + 1));
    klee_assert(cpu->regBank.registers[9] == memory->memory[(WORD)(starting_PC + 1)]);

    return cpu_update_UPC(cpu, Unconditional, 16, 16);
}

// Increment PC by 2.
FLAG nxtftch(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Increment PC by 2.
    cpu_byte_add_nocarry(cpu, 7, 24, 7, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 6, 22, 6, S, 0, 0, 0, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 17, 17);
}

// Fetch MDRE/O at PC.
FLAG odd_memread(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 6, 7);
    mem_read_word(cpu, memory, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 18, 18);
}

// Move MDRE to RB10.
FLAG odd_move_rb10(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRE, 10, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 19, 19);
}

// Move MDRO to P.
FLAG odd_move_p(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRO, 11, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 20, 20);
}

// Set the prefetch valid bit to true.
FLAG odd_set_pf(struct VerificationModel* model)
{
        // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_set_prefetch_flag(cpu, 1);

    return cpu_update_UPC(cpu, Unconditional, 21, 21);
}

FLAG end(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Stop, 0, 0);
}
