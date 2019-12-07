#include "alufunc.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"

/*
 * Nonunary ALU operations performed on register bytes.
 */
struct ALUByteResult byte_add_nocarry(BYTE a, BYTE b)
{
    return byte_add_carry(a, b, 0);
}
struct ALUByteResult byte_sub_nocarry(BYTE a, BYTE b)
{
    return byte_add_carry(a, ~b, 1);
}

struct ALUByteResult byte_add_carry(BYTE a, BYTE b, FLAG carryIn)
{
    struct ALUByteResult result;
    result.result = (BYTE)(((BYTE)(a) + (BYTE)b) + (BYTE)(carryIn >= 1 ? 1 : 0));
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    // There is a signed overflow iff the high order bits of the input are the same,
    // and the inputs & output differs in sign.
    // Shifts in 0's (unsigned chars), so after shift, only high order bit remain.
    result.NZVC[V] = ((~(a ^ b) & (a ^ result.result)) >> 7) ? 1 : 0;
    // If the output shrunk in magnitude, then there was a carryout
    result.NZVC[C] = result.result < a || result.result < b ? 1 : 0;
    return result;  
}
struct ALUByteResult byte_sub_carry(BYTE a, BYTE b, FLAG carryIn)
{
    return byte_add_carry(a, ~b, carryIn);
}

struct ALUByteResult byte_and(BYTE a, BYTE b) 
{
    struct ALUByteResult result;
    result.result = a & b;
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUByteResult byte_nand(BYTE a, BYTE b)
{
    return byte_not(byte_and(a, b).result);
}

struct ALUByteResult byte_or(BYTE a, BYTE b)
{
    // Use identity a + b == (a + b)'' == (a'b')'
    return byte_not(byte_and(~a, ~b).result);
}

struct ALUByteResult byte_nor(BYTE a, BYTE b)
{
    // Use identity (a + b)' == (a'b')
    return byte_and(~a, ~b);
}

struct ALUByteResult byte_xor(BYTE a, BYTE b)
{
    // Use a XOR b == (ab')+(a'b)
    return byte_or(byte_and(a, ~b).result,
                   byte_and(~a, b).result);
}

/*
 * Unary ALU operations performed on register byte.
 */
struct ALUByteResult byte_not(BYTE a)
{
    struct ALUByteResult result;
    result.result = ~a;
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUByteResult byte_asl(BYTE a)
{
    // The implementations of asl and rol(x, 0) were proven equivilant by a theorem prover.
    return byte_rol(a,0);
}

struct ALUByteResult byte_asr(BYTE a)
{
    FLAG carryIn = a & 0x80 ? 1 : 0;
    return byte_ror(a, carryIn);
}

struct ALUByteResult byte_rol(BYTE a, FLAG carryIn)
{
    struct ALUByteResult result;
    result.result = a << 1 | (carryIn ? 1 : 0);
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
     // Signed overflow if a<hi> doesn't match a<hi-1>
    result.NZVC[V] =  ((result.result >= 0x80) == (a >=0x80)) ? 0 : 1;
     // Carry out equals the hi order bit
    result.NZVC[C] = (a >= 0x80) ? 1 : 0;
    return result;

}
struct ALUByteResult byte_ror(BYTE a, FLAG carryIn)
{
    struct ALUByteResult result;
    result.result = a >> 1 | (carryIn ? 0x80 : 0);
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
     // Carry out equals the lo order bit
    result.NZVC[C] = (a & 0x01) ? 1 : 0;
    return result;
}

struct ALUByteResult byte_ident(BYTE a)
{
    struct ALUByteResult result;
    result.result = a;
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
    return result;
}
struct ALUByteResult byte_flags(BYTE a)
{
    struct ALUByteResult result;
    result.NZVC[N] = a & (1<<3) ? 1 : 0;
    result.NZVC[Z] = a & (1<<2) ? 1 : 0;
    result.NZVC[V] = a & (1<<1) ? 1 : 0;
    result.NZVC[C] = a & (1<<0) ? 1 : 0;
    return result;
}



/*
 * Nonunary ALU operations performed on register words.
 */

struct ALUWordResult word_add_nocarry(WORD a, WORD b)
{
    return word_add_carry(a, b, 0);
}

struct ALUWordResult word_sub_nocarry(WORD a, WORD b)
{
    return word_sub_carry(a, b, 1);
}

struct ALUWordResult word_add_carry(WORD a, WORD b, FLAG carryIn)
{
    struct ALUWordResult result;
    struct ALUByteResult low = byte_add_carry((BYTE)a, (BYTE)b, carryIn);
    struct ALUByteResult hi  = byte_add_carry(a & 0xFF00 >> 8, b & 0xFF00 >> 8, low.NZVC[C]);
    result.result = hi.result << 8 | low.result;
    // All status bits are the same except for Z, which is composed of high and low.
    memcpy(&result.NZVC, &hi.NZVC, sizeof(hi.NZVC));
    result.NZVC[Z] = hi.NZVC[Z] & low.NZVC[N] ? 1 : 0;
    return result;
}

struct ALUWordResult word_sub_carry(WORD a, WORD b, FLAG carryIn)
{
    return word_add_carry(a, ~b, carryIn);
}



struct ALUWordResult word_and(WORD a, WORD b)
{
    struct ALUWordResult result;
    result.result = a & b;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUWordResult word_nand(WORD a, WORD b)
{
    return word_not(word_and(a, b).result);
}

struct ALUWordResult word_or(WORD a, WORD b)
{
    // Use identity a + b == (a + b)'' == (a'b')'
    return word_not(word_and(~a, ~b).result);
}

struct ALUWordResult word_nor(WORD a, WORD b)
{
    // Use identity (a + b)' == (a'b')
    return word_and(~a, ~b);
}

struct ALUWordResult word_xor(WORD a, WORD b)
{
    // Use a XOR b == (ab')+(a'b)
    return word_or(word_and(a, ~b).result,
                   word_and(~a, b).result);
}

/*
 * Nonunary ALU operations performed on register words.
 */
struct ALUWordResult word_not(WORD a)
{
    struct ALUWordResult result;
    result.result = ~a;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUWordResult word_asl(WORD a)
{
    // The implementations of asl and rol(x, 0) were proven equivilant by a theorem prover.
    return word_rol(a,0);
}
struct ALUWordResult word_asr(WORD a)
{
    FLAG carryIn = a & 0x8000 ? 1 : 0;
    return word_ror(a, carryIn);
}

struct ALUWordResult word_rol(WORD a, FLAG carryIn)
{
    struct ALUWordResult result;
    result.result = a << 1 | (carryIn ? 1 : 0);
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
     // Signed overflow if a<hi> doesn't match a<hi-1>
    result.NZVC[V] =  (((a << 1) ^ a) >> 15) ? 1 : 0;
     // Carry out equals the hi order bit
    result.NZVC[C] = ((a & 0x8000) >> 15) ? 1 : 0;
    return result;
}

struct ALUWordResult word_ror(WORD a, FLAG carryIn)
{
    struct ALUWordResult result;
    result.result = a >> 1 | (carryIn ? 0x8000 : 0);
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
     // Carry out equals the hi order bit
    result.NZVC[C] = (a & 0x01) ? 1 : 0;
    return result;
}

struct ALUWordResult word_ident(WORD a)
{
    struct ALUWordResult result;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
    return result;
}
