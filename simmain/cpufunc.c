#include "cpufunc.h"
#include "cpu.h"
#include "alufunc.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>

FLAG update_UPC(struct CPU *cpu, enum BRANCH_TYPE type, MCAddress trueTarget, MCAddress falseTarget)
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
        case AddressingModeDecoder:
        case InstructionSpecifierDecoder:
            assert(0 && "Addressing mode undefined");
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
void set_prefetch_flag(struct CPU *cpu, FLAG value)
{
    cpu->PSNVCbits[P] = value;
}
void move_to_mar(struct CPU *cpu, REGNUM arn, REGNUM brn)
{
    // A bus value may only be from register bank.
    assert(arn>=0 && arn<=31);
    // B bus value may only be from register bank.
    assert(brn>=0 && brn<=31);

    // Move A, B to MARA, MARB.
    cpu->MARA = cpu->regBank.registers[arn];
    cpu->MARB = cpu->regBank.registers[brn];
}
void store_c(struct CPU* cpu, struct ALUByteResult* result, REGNUM crn)
{
    if(crn == MDRO) {
        cpu->MDRO = result->result;
    } 
    else if(crn == MDRE) {
        cpu->MDRE = result->result;
    }
    else {
        cpu->regBank.registers[crn]  = result->result;
    }
}

void save_status_bits(struct CPU* cpu, struct ALUByteResult* result, FLAG storeN, FLAG andZ,
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

void cpu_byte_add_nocarry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS) {
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);

}

void cpu_byte_add_carry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    //printf("end: %d\n", cpu->regBank.registers[crn]);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);
}

void cpu_byte_sub_nocarry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS) {
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);

}

void cpu_byte_sub_carry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);
}

void cpu_byte_and(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_nand(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_or(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_nor(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}
void cpu_byte_xor(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // B bus value may only be from register bank.
    assert( brn>=0 && brn<=31);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_ident(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_not(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ)
{
        // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, 0, 0);
}

void cpu_byte_rol(struct CPU* cpu, REGNUM arn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);
}

void cpu_byte_ror(struct CPU* cpu, REGNUM arn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, storeC, storeS);
}
void cpu_byte_asl(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS);

}
void cpu_byte_asr(struct CPU* cpu, REGNUM arn, REGNUM crn,
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS)
{
    // A bus value may only be from register bank, MDRE, MDRO;
    assert((arn>=0 && arn<=31) || arn==MDRO || arn==MDRE);
    // C bus value may go to register bank, MDRE, MDRO.
    assert((crn>=0 && crn<=31) || crn==MDRO || crn==MDRE);

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
    store_c(cpu, &result, crn);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, 0, storeC, storeS);

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
    byte_flags(0);
    struct ALUByteResult result = byte_flags(arv);
    save_status_bits(cpu, &result, storeN, andZ, storeZ, storeV, storeC, storeS); 
}

void cpu_read_flags(struct CPU* cpu, REGNUM crn)
{
    BYTE output;
    output |= cpu->PSNVCbits[N] ? 1 << 3 : 0;
    output |= cpu->PSNVCbits[Z] ? 1 << 2 : 0;
    output |= cpu->PSNVCbits[V] ? 1 << 1 : 0;
    output |= cpu->PSNVCbits[C] ? 1 << 0 : 0;
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

// Debug methods that allow read, writing exactly one byte
// from memory, ignoring alignment conditions.
void mem_dbg_read_into_mdre(struct CPU* cpu)
{
    WORD address = getMARWord(cpu);
    cpu->MDRE = memory[address];
}
void mem_dbg_read_into_mdro(struct CPU* cpu)
{
    WORD address = getMARWord(cpu);
    cpu->MDRO = memory[address];
}
void mem_dbg_write_mdre(struct CPU* cpu)
{
    WORD address = getMARWord(cpu);
    memory[address] = cpu->MDRE;
}
void mem_dbg_write_mdro(struct CPU* cpu)
{
    WORD address = getMARWord(cpu);
    memory[address] = cpu->MDRO;
}