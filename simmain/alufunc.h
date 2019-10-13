#ifndef ALUFUNC_H
#define ALUFUNC_H

#include "defs.h"


struct ALUByteResult
{
    BYTE result;
    FLAG NZVC[4];
};

/*
 * Nonunary ALU operations performed on register bytes.
 */
struct ALUByteResult byte_add_nocarry(BYTE a, BYTE b);
struct ALUByteResult byte_sub_nocarry(BYTE a, BYTE b);
struct ALUByteResult byte_add_carry(BYTE a, BYTE b, FLAG carryIn);
struct ALUByteResult byte_sub_carry(BYTE a, BYTE b, FLAG carryIn);

struct ALUByteResult byte_and(BYTE a, BYTE b);
struct ALUByteResult byte_nand(BYTE a, BYTE b);
struct ALUByteResult byte_or(BYTE a, BYTE b);
struct ALUByteResult byte_nor(BYTE a, BYTE b);
struct ALUByteResult byte_xor(BYTE a, BYTE b);

/*
 * Unary ALU operations performed on register byte.
 */
struct ALUByteResult byte_not(BYTE a);
struct ALUByteResult byte_asl(BYTE a);
struct ALUByteResult byte_asr(BYTE a);
struct ALUByteResult byte_rol(BYTE a, FLAG carryIn);
struct ALUByteResult byte_ror(BYTE a, FLAG carryIn);
struct ALUByteResult byte_ident(BYTE a);
struct ALUByteResult byte_flags(BYTE a);

#if false
struct ALUWordResult
{
    WORD result;
    FLAG NZVC[4];
};
/*
 * Nonunary ALU operations performed on register words.
 */
struct ALUWordResult add_nocarry(WORD a, WORD b);
struct ALUWordResult sub_nocarry(WORD a, WORD b);
struct ALUWordResult add_carry(WORD a, WORD b, FLAG carryIn);
struct ALUWordResult sub_carry(WORD a, WORD b, FLAG carryIn);

struct ALUWordResult and(WORD a, WORD b);
struct ALUWordResult nand(WORD a, WORD b);
struct ALUWordResult or(WORD a, WORD b);
struct ALUWordResult nor(WORD a, WORD b);
struct ALUWordResult xor(WORD a, WORD b);

/*
 * Unary ALU operations performed on register words.
 */
struct ALUWordResult not(WORD a);
struct ALUWordResult asl(WORD a);
struct ALUWordResult asr(WORD a);
struct ALUWordResult rol(WORD a, FLAG carryIn);
struct ALUWordResult ror(WORD a, FLAG carryIn);
struct ALUWordResult ident(WORD a);
#endif
#endif