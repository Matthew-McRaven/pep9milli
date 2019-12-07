#include "cpufunc.h"
#include "cpu.h"
#include "alufunc.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>

FLAG cpu_update_UPC(struct CPU *cpu, enum BRANCH_TYPE type, MCAddress trueTarget, MCAddress falseTarget)
{
    switch(type) {
        case Unconditional:
            cpu->microPC = trueTarget;
            break;
        case BRGT:
            if(!cpu->PSNVCbits[N] && !cpu->PSNVCbits[Z]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRGE:
            if(!cpu->PSNVCbits[N]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BREQ:
            if(cpu->PSNVCbits[Z]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRLE:
            if(cpu->PSNVCbits[N] || cpu->PSNVCbits[Z]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRLT:
            if(cpu->PSNVCbits[N]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRNE:
            if(!cpu->PSNVCbits[Z]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRV:
            if(cpu->PSNVCbits[V]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRC:
            if(cpu->PSNVCbits[C]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case BRS:
            if(cpu->PSNVCbits[S]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case IsPrefetchValid:
            if(cpu->PSNVCbits[P]){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            break;
        case IsPCEven:
            if(cpu->regBank.registers[8] % 2 == 0){
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }
            
            break;
        case IsUnary:
            if(cpu->is_unary_decoder[cpu->regBank.registers[8]]) {
                cpu->microPC = trueTarget;
            }
            else {
                cpu->microPC = falseTarget;
            }

            break;
        case AddressingModeDecoder:
            cpu->microPC = cpu->addressing_mode_decoder[cpu->regBank.registers[8]];
            break;
        case InstructionSpecifierDecoder:
            cpu->microPC = cpu->instruction_execute_decoder[cpu->regBank.registers[8]];
            break;
            
        case Stop:
            return 1;
            break;
        case AUTO_INCR:
            cpu->microPC += 1;
            break;  
    }
    return 0;
}
void cpu_set_prefetch_flag(struct CPU *cpu, FLAG value)
{
    cpu->PSNVCbits[P] = value;
}

void cpu_move_to_mar(struct CPU *cpu, REGNUM arn, REGNUM brn)
{
    // A bus value may only be from register bank.
    assert(arn>=0 && arn<=31);
    // B bus value may only be from register bank.
    assert(brn>=0 && brn<=31);

    // Move A, B to MARA, MARB.
    cpu->MARA = cpu->regBank.registers[arn];
    cpu->MARB = cpu->regBank.registers[brn];
}

void cpu_store_c(struct CPU* cpu, struct ALUByteResult* result, REGNUM crn)
{
    // Don't store a register of none type.
    if(crn == NONE) {
        return;
    }
    else if(crn == MDRO) {
        cpu->MDRO = result->result;
    } 
    else if(crn == MDRE) {
        cpu->MDRE = result->result;
    }
    else {
        cpu->regBank.registers[crn]  = result->result;
    }
}

void cpu_save_status_bits(struct CPU* cpu, struct ALUByteResult* result, FLAG storeN, FLAG andZ,
FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    if(storeN) {
        cpu->PSNVCbits[N] = result->NZVC[N] ? 1 : 0;
    }
    if(storeZ) {
        if(andZ) {
            cpu->PSNVCbits[Z] = cpu->PSNVCbits[Z] && result->NZVC[Z] ? 1 : 0;
        }
        else {
            cpu->PSNVCbits[Z] = result->NZVC[Z] ? 1 : 0;
        }
    }
    if(storeV) {
        cpu->PSNVCbits[V] = result->NZVC[V] ? 1 : 0;
    }
    if(storeC) {
        cpu->PSNVCbits[C] = result->NZVC[C] ? 1 : 0;
    }
    if(storeS) {
        cpu->PSNVCbits[S] = result->NZVC[C] ? 1 : 0;
    }
}
WORD cpu_get_pair(struct CPU* cpu, REGNUM hi, REGNUM lo)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((hi>=0 && hi<=31) || hi==MDRO || hi==MDRE);
    assert((lo>=0 && lo<=31) || lo==MDRO || lo==MDRE);
    BYTE hi_value, lo_value;
    if(hi >= 0 && hi <= 31) {
        hi_value = cpu->regBank.registers[hi];
    }
    else if(hi == MDRE) {
        hi_value = cpu->MDRE;
    }
    else if(hi == MDRO) {
        hi_value = cpu->MDRO;
    }

    if(lo >= 0 && lo <= 31) {
        lo_value = cpu->regBank.registers[lo];
    }
    else if(lo == MDRE) {
        lo_value = cpu->MDRE;
    }
    else if(lo == MDRO) {
        lo_value = cpu->MDRO;
    }

    return (WORD)(((WORD)hi_value )<< 8) | lo_value;
}
void cpu_byte_add_nocarry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS) {
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_add_nocarry(arv, brv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);

}

void cpu_byte_add_carry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];
    
    //printf("start: %d\n", arv);
    struct ALUByteResult result = byte_add_carry(arv, brv, cpu->PSNVCbits[carryIn]);
    cpu_store_c(cpu, &result, crn);
    //printf("end: %d\n", cpu->regBank.registers[crn]);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);
}

void cpu_byte_sub_nocarry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS) {
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_sub_nocarry(arv, brv);

    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);

}

void cpu_byte_sub_carry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];
    
    struct ALUByteResult result = byte_sub_carry(arv, brv, cpu->PSNVCbits[carryIn]);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);
}

void cpu_byte_and(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_and(arv, brv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_nand(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_nand(arv, brv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_or(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_or(arv, brv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_nor(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_nor(arv, brv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}
void cpu_byte_xor(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    // B Register Value.
    BYTE brv = cpu->regBank.registers[brn];

    struct ALUByteResult result = byte_xor(arv, brv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_ident(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    struct ALUByteResult result = byte_ident(arv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_not(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }

    struct ALUByteResult result = byte_not(arv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_rol(struct CPU* cpu, REGNUM arn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }
    
    struct ALUByteResult result = byte_rol(arv, cpu->PSNVCbits[carryIn]);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);
}

void cpu_byte_ror(struct CPU* cpu, REGNUM arn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }
    
    struct ALUByteResult result = byte_ror(arv, cpu->PSNVCbits[carryIn]);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, storeC, storeS);
}
void cpu_byte_asl(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }
    
    struct ALUByteResult result = byte_asl(arv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);

}
void cpu_byte_asr(struct CPU* cpu, REGNUM arn, REGNUM crn,
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE || crn == NONE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }
    
    struct ALUByteResult result = byte_asr(arv);
    cpu_store_c(cpu, &result, crn);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, storeC, storeS);

}

// Unary control operations.
void cpu_set_flags(struct CPU* cpu, REGNUM arn,
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
       // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);

    // A Register Value.
    BYTE arv;
    if(arn == MDRO) {
        arv = cpu->MDRO;
    } 
    else if(arn == MDRE) {
        arv = cpu->MDRE;
    }
    else {
        arv = cpu->regBank.registers[arn];
    }
    struct ALUByteResult result = byte_flags(arv);
    cpu_save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS); 
}

void cpu_read_flags(struct CPU* cpu, REGNUM crn)
{
    // Must initialize output, otherwise it will default to 255.
    BYTE output = 0;
    output |= cpu->PSNVCbits[N] != 0 ? 1 << 3 : 0;
    output |= cpu->PSNVCbits[Z] != 0 ? 1 << 2 : 0;
    output |= cpu->PSNVCbits[V] != 0 ? 1 << 1 : 0;
    output |= cpu->PSNVCbits[C] != 0 ? 1 << 0 : 0;
    if(crn == MDRO) {
        cpu->MDRO = output;
    } 
    else if(crn == MDRE) {
        cpu->MDRE = output;
    }
    else {
        cpu->regBank.registers[crn] = output;
    }

}