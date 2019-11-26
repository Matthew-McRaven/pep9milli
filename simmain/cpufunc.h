#ifndef CPUFUNC_H
#define CPUFUNC_H
#include "defs.h"
#include "cpu.h"
#include "alufunc.h"

enum BRANCH_TYPE
{
    Unconditional = 0,
    BRGT = 1, BRGE = 2, BREQ = 3, BRLE = 4, BRLT = 5,
    BRNE = 6, BRV = 7, BRC = 8, BRS = 9,
    IsPrefetchValid = 10,
    IsUnary = 11,
    IsPCEven = 12,
    AddressingModeDecoder = 13, // Adressing modes jump table
    InstructionSpecifierDecoder = 14, // Instruction jump table
    Stop = 15,
    AUTO_INCR = 16, // Not a real addressing mode, but included for convenience.
};

// Microcode helper functions
FLAG cpu_update_UPC(struct CPU *cpu, enum BRANCH_TYPE type, MCAddress trueTarget, MCAddress falseTarget);
void cpu_set_prefetch_flag(struct CPU *cpu, FLAG value);
void cpu_move_to_mar(struct CPU *cpu, REGNUM arn, REGNUM brn);


// Methods to simplify saving results.
void cpu_store_c(struct CPU* cpu, struct ALUByteResult* result, REGNUM crn);
void cpu_save_status_bits(struct CPU* cpu, struct ALUByteResult* result, FLAG storeN, FLAG andZ,
FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);

// Binary arithmatic operations.
void cpu_byte_add_nocarry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_byte_add_carry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_byte_sub_nocarry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_byte_sub_carry(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);    

// Binary logical operations.
void cpu_byte_and(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);
void cpu_byte_nand(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);
void cpu_byte_or(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);
void cpu_byte_nor(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);
void cpu_byte_xor(struct CPU* cpu, REGNUM arn, REGNUM brn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);

// Unary logical operations
void cpu_byte_ident(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);
void cpu_byte_not(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ);

// Unary arithmatic operations
void cpu_byte_rol(struct CPU* cpu, REGNUM arn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_byte_ror(struct CPU* cpu, REGNUM arn, REGNUM crn, enum StatusBits carryIn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_byte_asl(struct CPU* cpu, REGNUM arn, REGNUM crn, 
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_byte_asr(struct CPU* cpu, REGNUM arn, REGNUM crn,
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);

// Unary control operations.
void cpu_set_flags(struct CPU* cpu, REGNUM arn,
FLAG storeN, FLAG andZ, FLAG storeZ, FLAG storeV, FLAG storeC, FLAG storeS);
void cpu_read_flags(struct CPU* cpu, REGNUM crn);

#endif //CPUFUNC_H