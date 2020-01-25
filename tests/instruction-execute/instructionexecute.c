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

// Unary no operation
FLAG m_nop0(struct VerificationModel* model);

// Unary Shift & Rotate Instructions
// ASLA instruction
FLAG m_asla1(struct VerificationModel* model);
FLAG m_asla2(struct VerificationModel* model);
// ASLX instruction
FLAG m_aslx1(struct VerificationModel* model);
FLAG m_aslx2(struct VerificationModel* model);
// ASRA instruction
FLAG m_asra1(struct VerificationModel* model);
FLAG m_asra2(struct VerificationModel* model);
// ASRX instruction
FLAG m_asrx1(struct VerificationModel* model);
FLAG m_asrx2(struct VerificationModel* model);

// ROLA instruction
FLAG m_rola1(struct VerificationModel* model);
FLAG m_rola2(struct VerificationModel* model);
// ROLX instruction
FLAG m_rolx1(struct VerificationModel* model);
FLAG m_rolx2(struct VerificationModel* model);
// RORA instruction
FLAG m_rora1(struct VerificationModel* model);
FLAG m_rora2(struct VerificationModel* model);
// RORX instruction
FLAG m_rorx1(struct VerificationModel* model);
FLAG m_rorx2(struct VerificationModel* model);

// Branch instructions.
// BR
FLAG m_br1(struct VerificationModel* model);
FLAG m_br2(struct VerificationModel* model);

FLAG m_brle(struct VerificationModel* model);
FLAG m_brlt(struct VerificationModel* model);
FLAG m_breq(struct VerificationModel* model);
FLAG m_brne(struct VerificationModel* model);
FLAG m_brge(struct VerificationModel* model);
FLAG m_brgt(struct VerificationModel* model);
FLAG m_brv(struct VerificationModel* model);
FLAG m_brc(struct VerificationModel* model);

// Non-Unary Arithmetic Instruction
FLAG m_addsp(struct VerificationModel* model);
FLAG m_subsp(struct VerificationModel* model);
FLAG m_adda(struct VerificationModel* model);
FLAG m_addx(struct VerificationModel* model);
FLAG m_suba(struct VerificationModel* model);
FLAG m_subx(struct VerificationModel* model);

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
    m_nop0,                     //10
    m_asla1,                    //11
    m_asla2,                    //12
    m_aslx1,                    //13
    m_aslx2,                    //14
    m_asra1,                    //15
    m_asra2,                    //16
    m_asrx1,                    //17
    m_asrx2,                    //18
    m_rola1,                    //19
    m_rola2,                    //20
    m_rolx1,                    //21
    m_rolx2,                    //22
    m_rora1,                    //23
    m_rora2,                    //24
    m_rorx1,                    //25
    m_rorx2,                    //26
    m_br1,                      //27
    m_br2,                      //28
    m_brle,                     //29
    m_brlt,                     //30
    m_breq,                     //31
    m_brne,                     //32
    m_brge,                     //33
    m_brgt,                     //34
    m_brv,                      //35
    m_brc,                      //36
    m_addsp,                    //37
    m_subsp,                    //38
    m_adda,                     //49
    m_addx,                     //30
    m_suba,                     //41
    m_subx,                     //42

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
        else if(instruction_array[it] == i_nop0){cpu->instruction_execute_decoder[it] = 10;}
        else if(instruction_array[it] == i_asla){cpu->instruction_execute_decoder[it] = 11;}
        else if(instruction_array[it] == i_aslx){cpu->instruction_execute_decoder[it] = 13;}
        else if(instruction_array[it] == i_asra){cpu->instruction_execute_decoder[it] = 15;}
        else if(instruction_array[it] == i_asrx){cpu->instruction_execute_decoder[it] = 17;}
        else if(instruction_array[it] == i_rola){cpu->instruction_execute_decoder[it] = 19;}
        else if(instruction_array[it] == i_rolx){cpu->instruction_execute_decoder[it] = 21;}
        else if(instruction_array[it] == i_rora){cpu->instruction_execute_decoder[it] = 23;}
        else if(instruction_array[it] == i_rorx){cpu->instruction_execute_decoder[it] = 25;}
        else if(instruction_array[it] == i_br){cpu->instruction_execute_decoder[it] = 27;}
        else if(instruction_array[it] == i_brle){cpu->instruction_execute_decoder[it] = 29;}
        else if(instruction_array[it] == i_brlt){cpu->instruction_execute_decoder[it] = 30;}
        else if(instruction_array[it] == i_breq){cpu->instruction_execute_decoder[it] = 31;}
        else if(instruction_array[it] == i_brne){cpu->instruction_execute_decoder[it] = 32;}
        else if(instruction_array[it] == i_brge){cpu->instruction_execute_decoder[it] = 33;}
        else if(instruction_array[it] == i_brgt){cpu->instruction_execute_decoder[it] = 34;}
        else if(instruction_array[it] == i_brv){cpu->instruction_execute_decoder[it] = 35;}
        else if(instruction_array[it] == i_brc){cpu->instruction_execute_decoder[it] = 36;}
        else if(instruction_array[it] == i_addsp){cpu->instruction_execute_decoder[it] = 37;}
        else if(instruction_array[it] == i_subsp){cpu->instruction_execute_decoder[it] = 38;}
        else if(instruction_array[it] == i_adda){cpu->instruction_execute_decoder[it] = 39;}
        else if(instruction_array[it] == i_addx){cpu->instruction_execute_decoder[it] = 40;}
        else if(instruction_array[it] == i_suba){cpu->instruction_execute_decoder[it] = 41;}
        else if(instruction_array[it] == i_subx){cpu->instruction_execute_decoder[it] = 42;}
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
    for(int it=0; it <= sizeof(cpu->PSNVCbits); it++) {
        cpu->PSNVCbits[it] = starting_cpu.PSNVCbits[it];
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
    case i_nop0:
        for(int it = 0; it<= 31; it++) { klee_assert(cpu->regBank.registers[it] == starting_cpu.regBank.registers[it]);}
        break;
    case i_asla:
        klee_assert((WORD)(cpu_get_pair(&starting_cpu, 0, 1) << 1) == cpu_get_pair(cpu, 0, 1));
        // If the starting A value has a high order 1 bit, then there was a carry out.
        klee_assert((starting_cpu.regBank.registers[0] >= 0x80 ? 1 : 0)==(cpu->PSNVCbits[C] ? 1 : 0));
        // If the ending A value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[0] >= 0x80 ? 1 : 0)==(cpu->PSNVCbits[N] ? 1 : 0));
        // If A is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));
        // Overflow occurs when signs of and output differ.
        klee_assert(((cpu->regBank.registers[0] >= 0x80) != (starting_cpu.regBank.registers[0] >= 0x80)) == (cpu->PSNVCbits[V] ? 1 : 0));
        break;
    case i_aslx:
        klee_assert((WORD)(cpu_get_pair(&starting_cpu, 2, 3) << 1) == cpu_get_pair(cpu, 2, 3));
        // If the starting X value has a high order 1 bit, then there was a carry out.
        klee_assert((starting_cpu.regBank.registers[2] >= 0x80 ? 1 : 0)==(cpu->PSNVCbits[C] ? 1 : 0));
        // If the ending X value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[2] >= 0x80 ? 1 : 0)==(cpu->PSNVCbits[N] ? 1 : 0));
        // If X is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));
        // Overflow occurs when signs of and output differ.
        klee_assert(((cpu->regBank.registers[2] >= 0x80) != (starting_cpu.regBank.registers[2] >= 0x80)) == (cpu->PSNVCbits[V] ? 1 : 0));
        break;
    case i_asra:
        // The result is the starting register pair, shifted right by one.
        // Additionally, the input is sign extended, which is represented by the | in the rhs.
        klee_assert((WORD)((cpu_get_pair(&starting_cpu, 0, 1) >> 1) | (cpu_get_pair(&starting_cpu, 0, 1) >= 0x8000?0x8000:0)) 
        == cpu_get_pair(cpu, 0 , 1));
        // If the ending A value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[0] >= 0x80 ? 1 : 0) == (cpu->PSNVCbits[N] ? 1 : 0));
        // Carry out if starting lowest order bit is 1.
        klee_assert((starting_cpu.regBank.registers[1] & 0x1 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        // If A is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));
        break;
    case i_asrx:
        // The result is the starting register pair, shifted right by one.
        // Additionally, the input is sign extended, which is represented by the | in the rhs.
        klee_assert((WORD)((cpu_get_pair(&starting_cpu, 2, 3) >> 1) | (cpu_get_pair(&starting_cpu, 2, 3) >= 0x8000?0x8000:0)) 
        == cpu_get_pair(cpu, 2 , 3));
        // If the ending X value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[2] >= 0x80 ? 1 : 0) == (cpu->PSNVCbits[N] ? 1 : 0));
        // Carry out if starting lowest order bit is 1.
        klee_assert((starting_cpu.regBank.registers[3] & 0x1 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        // If X is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));
        break;
    case i_rola:
        // Shift the starting accumulator right by one, and set the low order bit to the carry.
        klee_assert((WORD)((cpu_get_pair(&starting_cpu, 0, 1) << 1) | (starting_cpu.PSNVCbits[C] ? 1 : 0)) 
        == cpu_get_pair(cpu, 0 , 1));
        // If the starting A value has a high order 1 bit, then there was a carry out.
        klee_assert((starting_cpu.regBank.registers[0] >= 0x80 ? 1 : 0)==(cpu->PSNVCbits[C] ? 1 : 0));
        break;
    case i_rolx:
            // Shift the starting accumulator right by one, and set the low order bit to the carry.
        klee_assert((WORD)((cpu_get_pair(&starting_cpu, 2, 3) << 1) | (starting_cpu.PSNVCbits[C] ? 1 : 0)) 
        == cpu_get_pair(cpu, 2 , 3));
        // If the starting X value has a high order 1 bit, then there was a carry out.
        klee_assert((starting_cpu.regBank.registers[2] >= 0x80 ? 1 : 0)==(cpu->PSNVCbits[C] ? 1 : 0));
        break;
    case i_rora:
        // The result is the starting register pair, shifted right by one.
        // Additionally, the input "shifts in" the carry.
        klee_assert((WORD)((cpu_get_pair(&starting_cpu, 0, 1) >> 1) | (starting_cpu.PSNVCbits[C]?0x8000:0)) 
        == cpu_get_pair(cpu, 0 , 1));
        // Carry out if starting lowest order bit is 1.
        klee_assert((starting_cpu.regBank.registers[1] & 0x1 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        break;
    case i_rorx:
        // The result is the starting register pair, shifted right by one.
        // Additionally, the input "shifts in" the carry.
        klee_assert((WORD)((cpu_get_pair(&starting_cpu, 2, 3) >> 1) | (starting_cpu.PSNVCbits[C]?0x8000:0)) 
        == cpu_get_pair(cpu, 2 , 3));
        // Carry out if starting lowest order bit is 1.
        klee_assert((starting_cpu.regBank.registers[3] & 0x1 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        break;
    case i_br:
        klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21));
        klee_assert(cpu->PSNVCbits[P] == 0);
        break;
    case i_brle:
        if(cpu->PSNVCbits[N] || cpu->PSNVCbits[Z]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_brlt:
        if(cpu->PSNVCbits[N]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_breq:
        if(cpu->PSNVCbits[Z]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_brne:
        if(!cpu->PSNVCbits[Z]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_brge:
        if(!cpu->PSNVCbits[N]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_brgt:
        if(!cpu->PSNVCbits[N] && !cpu->PSNVCbits[Z]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_brv:
        if(cpu->PSNVCbits[V])klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else { klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7)); }
        break;
    case i_brc:
        if(cpu->PSNVCbits[C]) klee_assert((cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 20, 21)) && cpu->PSNVCbits[P] == 0);
        else klee_assert(cpu_get_pair(cpu, 6, 7) == cpu_get_pair(&starting_cpu, 6, 7));
        break;
    case i_addsp:
        klee_assert(cpu_get_pair(cpu, 4, 5) == (WORD)(cpu_get_pair(&starting_cpu, 4, 5) + cpu_get_pair(&starting_cpu, 20, 21)));
        break;
    case i_subsp:
        klee_assert(cpu_get_pair(cpu, 4, 5) == (WORD)(cpu_get_pair(&starting_cpu, 4, 5) + (WORD)(~cpu_get_pair(&starting_cpu, 20, 21)) + 1));
        break;
    case i_adda:
        klee_assert(cpu_get_pair(cpu, 0, 1) == (WORD)(cpu_get_pair(&starting_cpu, 0, 1) + cpu_get_pair(&starting_cpu, 20, 21)));
        // If the ending A value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[0] >= 0x80 ? 1 : 0) == (cpu->PSNVCbits[N] ? 1 : 0));
        // If A is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0)); 
        // If A needs extra bits to be represented, then there was carry out.
        klee_assert((((uint32_t)cpu_get_pair(&starting_cpu, 0, 1) + (uint32_t)cpu_get_pair(&starting_cpu, 20, 21))>=0x10000 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        // Check if high order bytes are different, which checks for signed overflow.
        if((starting_cpu.regBank.registers[20] ^ starting_cpu.regBank.registers[0]) & 0x80) {
            // If high order differs, then V bit must be 0.
            klee_assert(cpu->PSNVCbits[V] == 0); 
        }
        else {
            // Otherwise the V bit is eq to the XOR of the high order bits.
            klee_assert(((starting_cpu.regBank.registers[20] ^ cpu->regBank.registers[0]) & 0x80) ? 1 : 0 == (cpu->PSNVCbits[V] ? 1 : 0));
        }
        break;
    case i_addx:
        klee_assert(cpu_get_pair(cpu, 2, 3) == (WORD)(cpu_get_pair(&starting_cpu, 2, 3) + cpu_get_pair(&starting_cpu, 20, 21))); 
        // If the ending X value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[2] >= 0x80 ? 1 : 0) == (cpu->PSNVCbits[N] ? 1 : 0));
        // If X is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));   
        // If X needs extra bits to be represented, then there was carry out.
        klee_assert((((uint32_t)cpu_get_pair(&starting_cpu, 2, 3) + (uint32_t)cpu_get_pair(&starting_cpu, 20, 21))>=0x10000 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        // Check if high order bytes are different, which checks for signed overflow.
        if((starting_cpu.regBank.registers[20] ^ starting_cpu.regBank.registers[2]) & 0x80) {
            // If high order differs, then V bit must be 0.
            klee_assert(cpu->PSNVCbits[V] == 0); 
        }
        else {
            // Otherwise the V bit is eq to the XOR of the high order bits.
            klee_assert(((starting_cpu.regBank.registers[20] ^ cpu->regBank.registers[2]) & 0x80) ? 1 : 0 == (cpu->PSNVCbits[V] ? 1 : 0));
        } 
        break;
    case i_suba:
        // Assert that A is equal to original A, plus the 2's complement of the operand value.
        klee_assert(cpu_get_pair(cpu, 0, 1) == (WORD)(cpu_get_pair(&starting_cpu, 0, 1) + (WORD)(~cpu_get_pair(&starting_cpu, 20, 21)) + 1));
        // If the ending A value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[0] >= 0x80 ? 1 : 0) == (cpu->PSNVCbits[N] ? 1 : 0));
        // If A is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 0, 1) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));  
        // If A needs extra bits to be represented, then there was carry out.
        klee_assert((((uint32_t)cpu_get_pair(&starting_cpu, 0, 1) + (uint32_t)((WORD)(((WORD)~cpu_get_pair(&starting_cpu, 20, 21)))+1))>=0x10000 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        // Check if high order bytes are different, which checks for signed overflow.
        // TODO: Signed Overflow.

        break;
    case i_subx:
        // Assert that X is equal to original X, plus the 2's complement of the operand value.
        klee_assert(cpu_get_pair(cpu, 2, 3) == (WORD)(cpu_get_pair(&starting_cpu, 2, 3) + (WORD)(~cpu_get_pair(&starting_cpu, 20, 21)) + 1)); 
        // If the ending X value has a high order 1 bit, then it should be negative.
        klee_assert((cpu->regBank.registers[2] >= 0x80 ? 1 : 0) == (cpu->PSNVCbits[N] ? 1 : 0));
        // If X is 0, then Z should be 1, else 0.
        klee_assert((cpu_get_pair(cpu, 2, 3) == 0) == (cpu->PSNVCbits[Z] ? 1 : 0));  
        // If X needs extra bits to be represented, then there was carry out.
        klee_assert((((uint32_t)cpu_get_pair(&starting_cpu, 2, 3) + (uint32_t)((WORD)(((WORD)~cpu_get_pair(&starting_cpu, 20, 21)))+1))>=0x10000 ? 1 : 0) == (cpu->PSNVCbits[C] ? 1 : 0));
        // Check if high order bytes are different, which checks for signed overflow.
        // TODO: Signed Overflow.

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

// Unary no operation
FLAG m_nop0(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// Unary Shift & Rotate Instructions
// ASLA instruction
FLAG m_asla1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asl(cpu, 1, 1, 0, 0, 1, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_asla2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_rol(cpu, 0, 0, S, 1, 1, 1, 1, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// ASLX instruction
FLAG m_aslx1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

   cpu_byte_asl(cpu, 3, 3, 0, 0, 1, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_aslx2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_rol(cpu, 2, 2, S, 1, 1, 1, 1, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// ASRA instruction
FLAG m_asra1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 0, 0, 1, 0, 1, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_asra2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ror(cpu, 1, 1, S, 0, 1, 1, 0, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// ASRX instruction
FLAG m_asrx1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 2, 2, 1, 0, 1, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_asrx2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ror(cpu, 3, 3, S, 0, 1, 1, 0, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// ROLA instruction
FLAG m_rola1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_rol(cpu, 1, 1, C, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_rola2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_rol(cpu, 0, 0, S, 0, 0, 0, 0, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// ROLX instruction
FLAG m_rolx1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_rol(cpu, 3, 3, C, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_rolx2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_rol(cpu, 2, 2, S, 0, 0, 0, 0, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// RORA instruction
FLAG m_rora1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ror(cpu, 0, 0, C, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}

FLAG m_rora2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ror(cpu, 1, 1, S, 0, 0, 0, 0, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// RORX instruction
FLAG m_rorx1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ror(cpu, 2, 2, C, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG m_rorx2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ror(cpu, 3, 3, S, 0, 0, 0, 0, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

// Branch instructions.
// BR
FLAG m_br1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, 20, 6, 0, 0, 0);
    cpu_byte_ident(cpu, 21, 7, 0, 0, 0);

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}

FLAG m_br2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_set_prefetch_flag(cpu, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

FLAG m_brle(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRLE, 27, 1); 
}
FLAG m_brlt(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRLT, 27, 1); 
}

FLAG m_breq(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BREQ, 27, 1); 
}

FLAG m_brne(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRNE, 27, 1); 
}

FLAG m_brge(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRGE, 27, 1); 
}

FLAG m_brgt(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRGT, 27, 1); 
}

FLAG m_brv(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRV, 27, 1); 
}

FLAG m_brc(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, BRC, 27, 1); 
}

// Non-Unary Arithmetic Instruction
FLAG m_addsp(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_add_nocarry(cpu, 5, 21, 5, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 4, 20, 4, S, 0, 0, 0, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

FLAG m_subsp(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 5, 21, 5, 0, 0, 0, 0, 0, 1);

    cpu_byte_sub_carry(cpu, 4, 20, 4, S, 0, 0, 0, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

FLAG m_adda(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_add_nocarry(cpu, 1, 21, 1, 0, 0, 1, 0, 0, 1);
    cpu_byte_add_carry(cpu, 0, 20, 0, S, 1, 1, 1, 1, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

FLAG m_addx(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_add_nocarry(cpu, 3, 21, 3, 0, 0, 1, 0, 0, 1);
    cpu_byte_add_carry(cpu, 2, 20, 2, S, 1, 1, 1, 1, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}

FLAG m_suba(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 1, 21, 1, 0, 0, 1, 0, 0, 1);
    cpu_byte_sub_carry(cpu, 0, 20, 0, S, 1, 1, 1, 1, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}
FLAG m_subx(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_sub_nocarry(cpu, 3, 21, 3, 0, 0, 1, 0, 0, 1);
    cpu_byte_sub_carry(cpu, 2, 20, 2, S, 1, 1, 1, 1, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 1, 1); 
}