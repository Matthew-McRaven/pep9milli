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


struct ALUWordResult
{
    WORD result;
    FLAG NZVC[4];
};
/*
 * Nonunary ALU operations performed on register words.
 */
struct ALUWordResult word_add_nocarry(WORD a, WORD b);
struct ALUWordResult word_sub_nocarry(WORD a, WORD b);
struct ALUWordResult word_add_carry(WORD a, WORD b, FLAG carryIn);
struct ALUWordResult word_sub_carry(WORD a, WORD b, FLAG carryIn);

struct ALUWordResult word_and(WORD a, WORD b);
struct ALUWordResult word_nand(WORD a, WORD b);
struct ALUWordResult word_or(WORD a, WORD b);
struct ALUWordResult word_nor(WORD a, WORD b);
struct ALUWordResult word_xor(WORD a, WORD b);

/*
 * Unary ALU operations performed on register words.
 */
struct ALUWordResult word_not(WORD a);
struct ALUWordResult word_asl(WORD a);
struct ALUWordResult word_asr(WORD a);
struct ALUWordResult word_rol(WORD a, FLAG carryIn);
struct ALUWordResult word_ror(WORD a, FLAG carryIn);
struct ALUWordResult word_ident(WORD a);

#endif