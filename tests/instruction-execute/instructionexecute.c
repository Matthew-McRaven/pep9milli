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

// MOVSPA instruction implementation
FLAG m_movspa(struct VerificationModel* model);

// MOVFLGA instruction implementation
FLAG m_movflga(struct VerificationModel* model);

// MOVAFLG instruction implementation
FLAG m_movaflg(struct VerificationModel* model);

// NOTA instruction implementation
FLAG m_nota(struct VerificationModel* model);
// NOTX instruction implementation
FLAG m_notx(struct VerificationModel* model);

// NEGA instruction implementation
FLAG m_nega(struct VerificationModel* model);
// NEGX instruction implementation
FLAG m_negx(struct VerificationModel* model);


static MicrocodeLine microcodeTable[] = 
{
    determine_instruction,      //00
    end,                        //01
    m_stop,                     //02
    m_movspa,                   //03
    m_movflga,                  //04
    m_movaflg,                  //05
    m_nota,                     //06
    m_notx,                     //07
    m_nega,                     //08
    m_negx,                     //09

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
        else if(instruction_array[it] == i_movspa){cpu->instruction_execute_decoder[it] = 3;}
        else if(instruction_array[it] == i_movflga){cpu->instruction_execute_decoder[it] = 4;}
        else if(instruction_array[it] == i_movaflg){cpu->instruction_execute_decoder[it] = 5;}
        else if(instruction_array[it] == i_nota){cpu->instruction_execute_decoder[it] = 6;}
        else if(instruction_array[it] == i_notx){cpu->instruction_execute_decoder[it] = 7;}
        else if(instruction_array[it] == i_nega){cpu->instruction_execute_decoder[it] = 8;}
        else if(instruction_array[it] == i_negx){cpu->instruction_execute_decoder[it] = 9;}
        else if(instruction_array[it] == 256){cpu->instruction_execute_decoder[it] = 2;}
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
    case i_movspa:
        klee_assert(cpu->regBank.registers[0] == cpu->regBank.registers[4]);
        klee_assert(cpu->regBank.registers[1] == cpu->regBank.registers[5]);

        break;
        break;

    case i_movflga:
        // Bit 4 of A should == N.
        if(cpu->PSNVCbits[N] != 0) klee_assert((cpu->regBank.registers[1] & (1<<3)) != 0);
        else if(cpu->PSNVCbits[N] == 0) klee_assert((cpu->regBank.registers[1] & (1<<3)) == 0);
        // Bit 3 of A should == Z.
        if(cpu->PSNVCbits[Z] != 0) klee_assert((cpu->regBank.registers[1] & (1<<2)) != 0);
        else if(cpu->PSNVCbits[Z] == 0) klee_assert((cpu->regBank.registers[1] & (1<<2)) == 0);
        // Bit 2 of A should == C.
        if(cpu->PSNVCbits[V] != 0) klee_assert((cpu->regBank.registers[1] & (1<<1)) != 0);
        else if(cpu->PSNVCbits[V] == 0) klee_assert((cpu->regBank.registers[1] & (1<<1)) == 0);
        // Bit 1 of A should == C.
        if(cpu->PSNVCbits[C] != 0) klee_assert((cpu->regBank.registers[1] & (1<<0)) != 0);
        else if(cpu->PSNVCbits[C] == 0) klee_assert((cpu->regBank.registers[1] & (1<<0)) == 0);
        break;

    case i_movaflg:
        klee_assert((cpu->regBank.registers[1] & 0x01 ? 1 : 0 ) == cpu->PSNVCbits[C]);
        klee_assert((cpu->regBank.registers[1] & 0x02 ? 1 : 0 ) == cpu->PSNVCbits[V]);
        klee_assert((cpu->regBank.registers[1] & 0x04 ? 1 : 0 ) == cpu->PSNVCbits[Z]);
        klee_assert((cpu->regBank.registers[1] & 0x08 ? 1 : 0 ) == cpu->PSNVCbits[N]);
        break;

    case i_nota:
        klee_assert((WORD)((~cpu_get_pair(&starting_cpu, 0, 1))) ==  cpu_get_pair(cpu, 0, 1));
        // Z is set iff the word is all 0's.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0) == cpu->PSNVCbits[Z]);
        // If the high order register has a leading one, N must be set. Otherwise, N must be 0.
        klee_assert((cpu->regBank.registers[0]>=0x80) == cpu->PSNVCbits[N]);
        break;
    case i_notx:
        klee_assert((WORD)((~cpu_get_pair(&starting_cpu, 2, 3))) ==  cpu_get_pair(cpu, 2, 3));
        // Z is set iff the word is all 0's.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0) == cpu->PSNVCbits[Z]);
        // If the high order register has a leading one, N must be set. Otherwise, N must be 0.
        klee_assert((cpu->regBank.registers[2]>=0x80) == cpu->PSNVCbits[N]);
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

// MOVSPA instruction implementation
FLAG m_movspa(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Move SP to A
    cpu_byte_ident(cpu, 5, 1, 0, 0, 0);
    cpu_byte_ident(cpu, 4, 0, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}


// MOVFLGA instruction implementation
FLAG m_movflga(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_read_flags(cpu, 1);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}

// MOVAFLG instruction implementation
FLAG m_movaflg(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_set_flags(cpu, 1, 1, 0, 1, 1, 1, 0);

    klee_assert((cpu->PSNVCbits[N] ? 1 : 0) == (cpu->regBank.registers[1] & (1<<3) ? 1 : 0));
    klee_assert((cpu->PSNVCbits[Z] ? 1 : 0) == (cpu->regBank.registers[1] & (1<<2) ? 1 : 0));
    klee_assert((cpu->PSNVCbits[V] ? 1 : 0) == (cpu->regBank.registers[1] & (1<<1) ? 1 : 0));
    klee_assert((cpu->PSNVCbits[C] ? 1 : 0) == (cpu->regBank.registers[1] & (1<<0) ? 1 : 0));

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}

// NOTA instruction implementation
FLAG m_nota(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_not(cpu, 1, 1, 0, 0, 1);
    cpu_byte_not(cpu, 0, 0, 1, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}

// NOTX instruction implementation
FLAG m_notx(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_not(cpu, 3, 3, 0, 0, 1);
    cpu_byte_not(cpu, 2, 2, 1, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}

// NEGA instruction implementation
FLAG m_nega(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 22, 1, 1, 0, 0, 1, 0, 1, 1);
    cpu_byte_sub_carry(cpu, 22, 0, 0, S, 1, 1, 1, 1, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}
// NEGX instruction implementation
FLAG m_negx(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 22, 3, 3, 0, 0, 1, 0, 0, 1);
    cpu_byte_sub_carry(cpu, 22, 2, 2, S, 1, 1, 1, 1, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1);
}
