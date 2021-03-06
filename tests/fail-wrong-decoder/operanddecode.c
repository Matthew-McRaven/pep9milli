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
FLAG execute(struct VerificationModel* model);

// Begin decoding operand.
FLAG opr_decode(struct VerificationModel* model);

// Immediate Addressing.
FLAG i_mode(struct VerificationModel* model);

// Direct Addressing
FLAG d_mode(struct VerificationModel* model);
FLAG d_det_odd(struct VerificationModel* model);
FLAG d_e_mode(struct VerificationModel* model);
FLAG d_e_move_rb20(struct VerificationModel* model);
FLAG d_e_move_rb21(struct VerificationModel* model);
FLAG d_o_mode(struct VerificationModel* model);
FLAG d_o_move_rb20(struct VerificationModel* model);
FLAG d_o_next_addr(struct VerificationModel* model);
FLAG d_o_memread2(struct VerificationModel* model);
FLAG d_o_move_rb21(struct VerificationModel* model);

// iNdirect Addressing
FLAG n_mode(struct VerificationModel* model);
FLAG n_det_odd(struct VerificationModel* model);
FLAG n1_e_mode(struct VerificationModel* model);
FLAG n1_e_move_rb18(struct VerificationModel* model);
FLAG n1_e_move_rb19(struct VerificationModel* model);
FLAG n1_o_mode(struct VerificationModel* model);
FLAG n1_o_next_addr(struct VerificationModel* model);
FLAG n1_o_move_rb18(struct VerificationModel* model);
FLAG n1_o_memread2(struct VerificationModel* model);
FLAG n1_o_move_rb19(struct VerificationModel* model);
FLAG n2_mode(struct VerificationModel* model);

//Stack relative addressing
FLAG s_mode(struct VerificationModel* model);
FLAG s_add(struct VerificationModel* model);

//indeXed addressing
FLAG x_mode(struct VerificationModel* model);
FLAG x_add(struct VerificationModel* model);

// Stack indeXed addressing
FLAG sx_mode(struct VerificationModel* model);
FLAG sx_add(struct VerificationModel* model);
FLAG sx_shift(struct VerificationModel* model);

//Stack deFerred addressing
FLAG sf_mode(struct VerificationModel* model);
FLAG sf_shift(struct VerificationModel* model);

//Stack deFerred indeXed addressing
FLAG sfx_mode(struct VerificationModel* model);
FLAG sfx_det_odd(struct VerificationModel* model);
FLAG sfx1_e(struct VerificationModel* model);
FLAG sfx1_e_add_rb19(struct VerificationModel* model);
FLAG sfx1_e_add_rb18(struct VerificationModel* model);
FLAG sfx1_o(struct VerificationModel* model);
FLAG sfx1_o_next_addr(struct VerificationModel* model);
FLAG sfx1_o_memread2(struct VerificationModel* model);
FLAG sfx1_o_add_rb19(struct VerificationModel* model);
FLAG sfx1_o_add_rb18(struct VerificationModel* model);

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
    execute,                //21
    opr_decode,             //22
    i_mode,                 //23
    d_mode,                 //24
    d_det_odd,              //25
    d_e_mode,               //26
    d_e_move_rb20,          //27
    d_e_move_rb21,          //28
    d_o_mode,               //29
    d_o_move_rb20,          //30
    d_o_next_addr,          //31
    d_o_memread2,           //32
    d_o_move_rb21,          //33
    n_mode,                 //34
    n_det_odd,              //35
    n1_e_mode,              //36
    n1_e_move_rb18,         //37
    n1_e_move_rb19,         //38
    n1_o_mode,              //39
    n1_o_next_addr,         //40
    n1_o_move_rb18,         //41
    n1_o_memread2,          //42
    n1_o_move_rb19,         //43
    n2_mode,                //44
    s_mode,                 //45
    s_add,                  //46
    x_mode,                 //47
    x_add,                  //48
    sx_mode,                //49
    sx_add,                 //50
    sx_shift,               //51
    sf_mode,                //52
    sf_shift,               //53
    sfx_mode,               //54
    sfx_det_odd,            //55
    sfx1_e,                 //56
    sfx1_e_add_rb19,        //57
    sfx1_e_add_rb18,        //58
    sfx1_o,                 //59
    sfx1_o_next_addr,       //60
    sfx1_o_memread2,        //61
    sfx1_o_add_rb19,        //62
    sfx1_o_add_rb18,        //63
};

WORD starting_PC;
BYTE IS_Value;

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
        if(instr_addr_mode[it]      == i_addr){cpu->addressing_mode_decoder[it] = 23;}
        else if(instr_addr_mode[it] == d_addr){cpu->addressing_mode_decoder[it] = 24;}
        else if(instr_addr_mode[it] == n_addr){cpu->addressing_mode_decoder[it] = 34;}
        else if(instr_addr_mode[it] == s_addr){cpu->addressing_mode_decoder[it] = 45;}
        else if(instr_addr_mode[it] == x_addr){cpu->addressing_mode_decoder[it] = 47;}
        else if(instr_addr_mode[it] == sx_addr){cpu->addressing_mode_decoder[it] = 49;}
        else if(instr_addr_mode[it] == sf_addr){cpu->addressing_mode_decoder[it] = 52;}
        // FAULT INJECTION: SFX will be decoded incorrectly as immediate.
        else if(instr_addr_mode[it] == sfx_addr){cpu->addressing_mode_decoder[it] = 23;}
        else{cpu->addressing_mode_decoder[it] = 21;}
    }


    // Create state variables to compare against at the end of execution.
    klee_make_symbolic(&starting_PC, sizeof(starting_PC), "Starting PC");
    klee_make_symbolic(&IS_Value, sizeof(IS_Value), "IS Value");
    //IS_Value = 80 + 7;
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
    //addressing_mode_decoder[255] = 23;

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
        
        // RW20 == Operand Specifier
        enum AddressingModes mode = instr_addr_mode[cpu->regBank.registers[8]];
        if(mode == i_addr) {
            klee_assert(cpu->regBank.registers[9]  == cpu->regBank.registers[20]);
            klee_assert(cpu->regBank.registers[10] == cpu->regBank.registers[21]);
        } 
        else if(mode == d_addr) {
            klee_assert(cpu->regBank.registers[9]  == cpu->regBank.registers[18]);
            klee_assert(cpu->regBank.registers[10] == cpu->regBank.registers[19]);
            WORD d_address = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            klee_assert(cpu->regBank.registers[20] == memory->memory[d_address]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(d_address + 1)]);
        } 
        else if(mode == n_addr) {
            WORD n_address = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            klee_assert(cpu->regBank.registers[18] == memory->memory[n_address]);
            klee_assert(cpu->regBank.registers[19] == memory->memory[(WORD)(n_address + 1)]);
            WORD n2_address = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
            klee_assert(cpu->regBank.registers[20] == memory->memory[n2_address]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(n2_address + 1)]);
        }
        else if(mode == s_addr) {
            WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
            WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
            klee_assert((WORD)(sp_val + os_val) == rw18_val);
            
            WORD s_address = os_val + sp_val;
            klee_assert(cpu->regBank.registers[20] == memory->memory[s_address]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(s_address + 1)]);
        } 
        else if(mode == x_addr) {
            WORD x_val = (WORD)(((WORD)cpu->regBank.registers[2]) << 8) | cpu->regBank.registers[3];
            WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
            klee_assert((WORD)(x_val + os_val) == rw18_val);
            
            WORD s_address = os_val + x_val;
            klee_assert(cpu->regBank.registers[20] == memory->memory[s_address]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(s_address + 1)]);
        }
        else if(mode == sx_addr) {
            WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
            WORD x_val = (WORD)(((WORD)cpu->regBank.registers[2]) << 8) | cpu->regBank.registers[3];
            WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
            klee_assert((WORD)(sp_val + x_val + os_val) == rw18_val);
            
            WORD s_address = sp_val + os_val + x_val;
            klee_assert(cpu->regBank.registers[20] == memory->memory[s_address]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(s_address + 1)]);
        }
        else if(mode == sf_addr) {
            WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
            WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
            WORD sf_address = sp_val + os_val;
            klee_assert(cpu->regBank.registers[18] == memory->memory[sf_address]);
            klee_assert(cpu->regBank.registers[19] == memory->memory[(WORD)(sf_address + 1)]);
            WORD sf2_address = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
            klee_assert(cpu->regBank.registers[20] == memory->memory[sf2_address]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(sf2_address + 1)]);
        } 
        else if(mode == sfx_addr) {
            WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
            WORD x_val = (WORD)(((WORD)cpu->regBank.registers[2]) << 8) | cpu->regBank.registers[3];
            WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
            WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
            WORD sfx_address = sp_val + os_val;
            WORD sfx1_val = (WORD)(memory->memory[sfx_address]) << 8 | memory->memory[(WORD)(sfx_address + 1)];
            sfx1_val += x_val;
            klee_assert(rw18_val == sfx1_val);

            klee_assert(cpu->regBank.registers[20] == memory->memory[rw18_val]);
            klee_assert(cpu->regBank.registers[21] == memory->memory[(WORD)(rw18_val + 1)]);
        }
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

    WORD address = (WORD)(((WORD)cpu->regBank.registers[6]) << 8) | cpu->regBank.registers[7];
    
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

    return cpu_update_UPC(cpu, Unconditional, 22, 22);
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
    cpu_byte_ident(cpu, 11, 9, 0, 0, 0);

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

    return cpu_update_UPC(cpu, Unconditional, 22, 22);
}

FLAG execute(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Stop, 0, 0);
}

// Begin decoding operand.
FLAG opr_decode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, AddressingModeDecoder, -1, -1);
}

// Immediate Addressing.
FLAG i_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Move operand specifier to RW20
    cpu_byte_ident(cpu, 9, 20, 0, 0, 0);
    cpu_byte_ident(cpu, 10, 21, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 21, 21);
}

//Direct Addressing
FLAG d_mode(struct VerificationModel* model)
{
    //klee_assert(0);
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Move operand specifier to RW18
    cpu_byte_ident(cpu, 9, 18, 0, 0, 0);
    cpu_byte_ident(cpu, 10, 19, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 25, 25);
}

FLAG d_det_odd(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 29, 26);
}

FLAG d_e_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 18, 19);
    mem_read_word(cpu, memory, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 27, 27);
}

FLAG d_e_move_rb20(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRE, 20, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 28, 28);
}

FLAG d_e_move_rb21(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRO, 21, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 21, 21);
}

FLAG d_o_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 18, 19);
    mem_read_word(cpu, memory, 0, 1);

    return cpu_update_UPC(cpu, Unconditional, 30, 30);
}

FLAG d_o_move_rb20(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRO, 20, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 31, 31);
}

FLAG d_o_next_addr(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_add_nocarry(cpu, 19, 23, 17, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 18, 22, 16, S, 0, 0, 0, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 32, 32);
}

FLAG d_o_memread2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 16, 17);
    mem_read_word(cpu, memory, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 33, 33);
}

FLAG d_o_move_rb21(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRE, 21, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 21, 21);
}

// iNdirect Addressing
FLAG n_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Move operand specifier to RW18
    cpu_byte_ident(cpu, 9, 18, 0, 0, 0);
    cpu_byte_ident(cpu, 10, 19, 0, 0, 0);


    return cpu_update_UPC(cpu, Unconditional, 35, 35);
}

FLAG n_det_odd(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 39, 36);
}

FLAG n1_e_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 18, 19);
    mem_read_word(cpu, memory, 1, 1);

    return cpu_update_UPC(cpu, Unconditional, 37, 37);
}

FLAG n1_e_move_rb18(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRE, 18, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 38, 38);
}

FLAG n1_e_move_rb19(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_byte_ident(cpu, MDRO, 19, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 44, 44);
}

FLAG n1_o_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 18, 19);
    mem_read_word(cpu, memory, 0, 1);

    return cpu_update_UPC(cpu, Unconditional, 40, 40);
}

FLAG n1_o_next_addr(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Increment PC by 2.
    cpu_byte_add_nocarry(cpu, 19, 23, 17, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 18, 22, 16, S, 0, 0, 0, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 41, 41);
}

FLAG n1_o_move_rb18(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_ident(cpu, MDRO, 18, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 42, 42);
}



FLAG n1_o_memread2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 16, 17);
    mem_read_word(cpu, memory, 1, 0);

    return cpu_update_UPC(cpu, Unconditional, 43, 43);
}

FLAG n1_o_move_rb19(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_byte_ident(cpu, MDRE, 19, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 44, 44);
}

FLAG n2_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 29, 26);
}

//Stack relative addressing
FLAG s_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Add OS to SP.
    cpu_byte_add_nocarry(cpu, 10, 5, 19, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 9, 4, 18, S, 0, 0, 0, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 46, 46);
}

FLAG s_add(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 29, 26);
}

//indeXed addressing
FLAG x_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Add OS to X.
    cpu_byte_add_nocarry(cpu, 3, 10, 19, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 2, 9, 18, S, 0, 0, 0, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 48, 48);
}

FLAG x_add(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 29, 26);
}

// Stack indeXed addressing
FLAG sx_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Add OS to SP.
    cpu_byte_add_nocarry(cpu, 10, 5, 19, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 9, 4, 18, S, 0, 0, 0, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 50, 50);
}
FLAG sx_add(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    // Add X to RW18.
    cpu_byte_add_nocarry(cpu, 19, 3, 19, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 18, 2, 18, S, 0, 0, 0, 0, 0, 0);
    
    return cpu_update_UPC(cpu, Unconditional, 51, 51);  
}

FLAG sx_shift(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 29, 26);
}

//Stack deFerred addressing
FLAG sf_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    // Add SP to RW18.
    cpu_byte_add_nocarry(cpu, 10, 5, 19, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 9, 4, 18, S, 0, 0, 0, 0, 0, 0);
    
    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19];
    WORD sf_address = sp_val + os_val;
    klee_assert(rw18_val == sf_address);

    return cpu_update_UPC(cpu, Unconditional, 53, 53);  
}

FLAG sf_shift(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 39, 36);
}

//Stack deFerred indeXed addressing
FLAG sfx_mode(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    // Add OS to SP.
    cpu_byte_add_nocarry(cpu, 10, 5, 19, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 9, 4, 18, S, 0, 0, 0, 0, 0, 0);
    
    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
    WORD sfx_address = sp_val + os_val;
    klee_assert(rw18_val == sfx_address);

    return cpu_update_UPC(cpu, Unconditional, 55, 55);
}

FLAG sfx_det_odd(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    cpu_byte_asr(cpu, 19, NONE, 0, 0, 0, 0, 0, 1);

    return cpu_update_UPC(cpu, BRS, 59, 56);
}
FLAG sfx1_e(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 18, 19);
    mem_read_word(cpu, memory, 1, 1);

    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
    WORD sfx_address = sp_val + os_val;
    klee_assert(rw18_val == sfx_address);
    klee_assert(cpu->MDRE == memory->memory[sfx_address]);
    klee_assert(cpu->MDRO == memory->memory[(WORD)(sfx_address+1)]);

    return cpu_update_UPC(cpu, Unconditional, 57, 57);
}
FLAG sfx1_e_add_rb19(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    // Add OS to X.
    cpu_byte_add_nocarry(cpu, MDRO, 3, 19, 0, 0, 0, 0, 0, 1);
    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD sfx_address = sp_val + os_val;
    klee_assert(cpu->MDRE == memory->memory[sfx_address]);
    klee_assert(cpu->MDRO == memory->memory[(WORD)(sfx_address+1)]);
    
    return cpu_update_UPC(cpu, Unconditional, 58, 58);
}

FLAG sfx1_e_add_rb18(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    // Add OS to X.
    cpu_byte_add_carry(cpu, MDRE, 2, 18, S, 0, 0, 0, 0, 0, 0);
    WORD x_val = (WORD)(((WORD)cpu->regBank.registers[2]) << 8) | cpu->regBank.registers[3];
    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
    WORD sfx_address = sp_val + os_val;
    WORD lookup1 = (WORD)(memory->memory[sfx_address]) << 8 | memory->memory[(WORD)(sfx_address + 1)];
    lookup1 += x_val;
    klee_assert(cpu->MDRE == memory->memory[sfx_address]);
    klee_assert(cpu->MDRO == memory->memory[(WORD)(sfx_address+1)]);
    klee_assert(rw18_val == lookup1);

    return cpu_update_UPC(cpu, Unconditional, 44, 44);
}

FLAG sfx1_o(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 18, 19);
    mem_read_word(cpu, memory, 0, 1);

    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
    WORD sfx_address = sp_val + os_val;
    klee_assert(rw18_val == sfx_address);
    klee_assert(cpu->MDRO == memory->memory[sfx_address]);

    return cpu_update_UPC(cpu, Unconditional, 60, 60);
}

FLAG sfx1_o_next_addr(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    // Add OS to SP.
    cpu_byte_add_nocarry(cpu, 19, 23, 17, 0, 0, 0, 0, 0, 1);
    cpu_byte_add_carry(cpu, 18, 22, 16, S, 0, 0, 0, 0, 0, 0);

    return cpu_update_UPC(cpu, Unconditional, 61, 61);
}

FLAG sfx1_o_memread2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory;

    cpu_move_to_mar(cpu, 16, 17);
    mem_read_word(cpu, memory, 1, 0);

    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
    WORD sfx_address = sp_val + os_val;
    klee_assert(rw18_val == sfx_address);
    klee_assert(cpu->MDRO == memory->memory[sfx_address]);
    klee_assert(cpu->MDRE == memory->memory[(WORD)(sfx_address+1)]);

    return cpu_update_UPC(cpu, Unconditional, 62, 62);
}

FLAG sfx1_o_add_rb19(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    // Add OS to X.
    cpu_byte_add_nocarry(cpu, MDRE, 3, 19, 0, 0, 0, 0, 0, 1);
    
    return cpu_update_UPC(cpu, Unconditional, 63, 63);
}
FLAG sfx1_o_add_rb18(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;
    struct MainMemory* memory = model->main_memory; 

    // Add OS to X.
    cpu_byte_add_carry(cpu, MDRO, 2, 18, S, 0, 0, 0, 0, 0, 0);
    WORD x_val = (WORD)(((WORD)cpu->regBank.registers[2]) << 8) | cpu->regBank.registers[3];
    WORD sp_val = (WORD)(((WORD)cpu->regBank.registers[4]) << 8) | cpu->regBank.registers[5];
    WORD os_val = (WORD)(((WORD)cpu->regBank.registers[9]) << 8) | cpu->regBank.registers[10];
    WORD rw18_val = (WORD)(((WORD)cpu->regBank.registers[18]) << 8) | cpu->regBank.registers[19]; 
    WORD sfx_address = sp_val + os_val;
    WORD lookup1 = (WORD)(memory->memory[sfx_address]) << 8 | memory->memory[(WORD)(sfx_address + 1)];
    lookup1 += x_val;
    klee_assert(rw18_val == lookup1);
    
    return cpu_update_UPC(cpu, Unconditional, 44, 44);
}
