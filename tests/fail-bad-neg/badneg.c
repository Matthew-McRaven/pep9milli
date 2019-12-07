#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "cpufunc.h"
#include "instructions.h"
#include "klee/klee.h"
#include "memory.h"
#include "model.h"

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 


// Determine which instruction is to be executed.
FLAG determine_instruction(struct VerificationModel* model);

// Redirect to STOP().
FLAG end(struct VerificationModel* model);

// Stop CPU execution.
FLAG m_stop(struct VerificationModel* model);

// NEGA instruction implementation
FLAG m_nega(struct VerificationModel* model);
// NEGX instruction implementation
FLAG m_negx(struct VerificationModel* model);


static MicrocodeLine microcodeTable[] = 
{
    determine_instruction,      //00
    end,                        //01
    m_stop,                     //02
    m_nega,                     //03
    m_negx,                     //04

};

// Address in memory of T6.
struct CPU starting_cpu;

void init_model(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Default assume all instructions are unary.
    memset(cpu->is_unary_decoder, 1, 256);
    // Instructions from 0 to 17 are unary.
    // Instructions from 18-37 are non-unary.
    for(int it=18; it <= 37; it += 2) {
        cpu->is_unary_decoder[it] = 0; cpu->is_unary_decoder[it + 1] = 0;
        instr_addr_mode[it] = i_addr;  instr_addr_mode[it + 1] = x_addr;
    }
    // Instructions from 38 to 79 are unary.
    // Instructions from 80 to 255 are nonunary.
    // Assign correct addressing modes
    for(int it=80; it <= 255; it += 8) {
        cpu->is_unary_decoder[it    ] = 0; cpu->is_unary_decoder[it + 1] = 0;
        cpu->is_unary_decoder[it + 2] = 0; cpu->is_unary_decoder[it + 3] = 0;
        cpu->is_unary_decoder[it + 4] = 0; cpu->is_unary_decoder[it + 5] = 0;
        cpu->is_unary_decoder[it + 6] = 0; cpu->is_unary_decoder[it + 7] = 0;
        instr_addr_mode[it    ] = i_addr;  instr_addr_mode[it + 1] = d_addr;
        instr_addr_mode[it + 2] = n_addr;  instr_addr_mode[it + 3] = s_addr;
        instr_addr_mode[it + 4] = sf_addr; instr_addr_mode[it + 5] = x_addr;
        instr_addr_mode[it + 6] = sx_addr; instr_addr_mode[it + 7] = sfx_addr;
    }

    for(int it=0; it <= 255; it++) {
        if(instruction_array[it] ==      i_stop){cpu->instruction_execute_decoder[it] = 2;}
        else if(instruction_array[it] == i_nega){cpu->instruction_execute_decoder[it] = 3;}
        else if(instruction_array[it] == i_negx){cpu->instruction_execute_decoder[it] = 4;}
        else {cpu->instruction_execute_decoder[it] = 2;}        
    }

    // Create state variables to compare against at the end of execution.
    klee_make_symbolic(&starting_cpu, sizeof(starting_cpu), "Starting CPU");
    // Initialize constant registers 22-31.
    init_static_regs(&starting_cpu);

    // Copy starting registers to current registers
    for(int it=0; it<=31; it++) {
        cpu->regBank.registers[it] = starting_cpu.regBank.registers[it];
    }
    // TODO: Copy over other CPU fields
}

FLAG test_model(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;
    FLAG result = 1;

    WORD final_address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
    // Assert that the instruction specifier was loaded.
    BYTE IS_value = cpu->regBank.registers[8];
    switch(IS_value) {
    case i_stop:
        break;
    case i_nega:
        // Must cast bitwise negation to WORD, otherwise it is upcast to INT, and the addition is performed over INTs.
        klee_assert((WORD)(((WORD)(~cpu_get_pair(&starting_cpu, 0, 1)))+1) == cpu_get_pair(cpu, 0, 1));
        // Z is set iff the word is all 0's.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0) == cpu->PSNVCbits[Z]);
        // If the high order register has a leading one, N must be set. Otherwise, N must be 0.
        klee_assert((cpu->regBank.registers[0]>=0x80) == cpu->PSNVCbits[N]);
        // Signed overflow may only occur when the word is 0x8000.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0x8000) == cpu->PSNVCbits[V]);
        break;
    case i_negx:
        // Must cast bitwise negation to WORD, otherwise it is upcast to INT, and the addition is performed over INTs.
        klee_assert((WORD)(((WORD)(~cpu_get_pair(&starting_cpu, 2, 3)))+1) == cpu_get_pair(cpu, 2, 3));
        // Z is set iff the word is all 0's.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0) == cpu->PSNVCbits[Z]);
        // If the high order register has a leading one, N must be set. Otherwise, N must be 0.
        klee_assert((cpu->regBank.registers[2]>=0x80) == cpu->PSNVCbits[N]);
        // Signed overflow may only occur when the word is 0x8000.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0x8000) == cpu->PSNVCbits[V]);
        break;
    default:
        break;
    }
    return 1;
}

MicrocodeLine fetch_current_line(struct CPU *cpu)
{
    return microcodeTable[cpu->microPC];
}

FLAG determine_instruction(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, InstructionSpecifierDecoder, 0, 0);
}

// Redirect to STOP().
FLAG end(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Unconditional, 2, 2);
}

// Stop CPU execution.
FLAG m_stop(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Stop, 0, 0 );
}

// NEGA instruction implementation
FLAG m_nega(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 22, 1, 1, 0, 0, 1, 0, 0, 0);
    cpu_byte_sub_carry(cpu, 22, 0, 0, S, 1, 1, 1, 1, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}

// NEGX instruction implementation
FLAG m_negx(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 22, 3, 3, 0, 0, 1, 0, 0, 1);
    cpu_byte_sub_carry(cpu, 22, 2, 2, S, 1, 0, 1, 1, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}
