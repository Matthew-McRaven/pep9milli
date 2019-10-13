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
    return byte_sub_carry(a, b, 1);
}

struct ALUByteResult byte_add_carry(BYTE a, BYTE b, FLAG carryIn)
{
    struct ALUByteResult result;
    result.result = a + b + (carryIn >= 1 ? 1 : 0);
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
    result.NZVC[V] =  (((a << 1) ^ a) >>7) ? 1 : 0;
     // Carry out equals the hi order bit
    result.NZVC[C] = ((a & 0x80) >> 7) ? 1 : 0;
    return result;

}
struct ALUByteResult byte_ror(BYTE a, FLAG carryIn)
{
    struct ALUByteResult result;
    result.result = a >> 1 | (carryIn ? 0x80 : 0);
    result.NZVC[N] = result.result & 0x80 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0; 
     // Carry out equals the hi order bit
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






#if false
/*
 * Nonunary ALU operations performed on register words.
 */
struct ALUWordResult add_nocarry(WORD a, WORD b)
{
    return add_carry(a, b, 0);
}
struct ALUWordResult sub_nocarry(WORD a, WORD b)
{
    return sub_carry(a, b, 1);
}
struct ALUWordResult add_carry(WORD a, WORD b, FLAG carryIn)
{
    struct ALUWordResult result;
    result.result = ((DWORD)a + (DWORD)b + ((DWORD)carryIn >= 1 ? 1 : 0));
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    // There is a signed overflow iff the high order bits of the input are the same,
    // and the inputs & output differs in sign.
    // Shifts in 0's (unsigned chars), so after shift, only high order bit remain.
    result.NZVC[V] = ((~(a ^ b) & (a ^ result.result)) >> 7) ? 1 : 0;
    // If the output shrunk in magnitude, then there was a carryout
    result.NZVC[C] = result.result < a || result.result < b ? 1 : 0;
    return result;
}
struct ALUWordResult sub_carry(WORD a, WORD b, FLAG carryIn)
{
    return add_carry(a, ~b, carryIn);
}



struct ALUWordResult and(WORD a, WORD b)
{
    struct ALUWordResult result;
    result.result = a & b;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUWordResult nor(WORD a, WORD b)
{
    struct ALUWordResult andRes = and(a, b);
    return not(andRes.result); 
}

struct ALUWordResult or(WORD a, WORD b)
{
    struct ALUWordResult result;
    result.result = a | b;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUWordResult nor(WORD a, WORD b)
{
    struct ALUWordResult orRes = or(a, b);
    return not(orRes.result); 
}

struct ALUWordResult xor(WORD a, WORD b)
{
    struct ALUWordResult result;
    result.result = a ^ b;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}
/*
 * Nonunary ALU operations performed on register words.
 */
struct ALUWordResult not(WORD a)
{
    struct ALUWordResult result;
    result.result = ~a;
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
    return result;
}

struct ALUWordResult asl(WORD a, FLAG* tempFlags)
{
    struct ALUWordResult result;
    result.result = (WORD)(a << 1);
    result.NZVC[N] = result.result & 0x8000 ? 1 : 0;
    result.NZVC[Z] = result.result == 0 ? 1 : 0;
     // Signed overflow if a<hi> doesn't match a<hi-1>
    result.NZVC[V] = (((a << 1) ^ a) >> 15 ) ? 1 : 0;
    //WORD result = (WORD)(a << 1);
    //tempFlags[N] = result & 0x8000 ? 1 : 0;
    //tempFlags[Z] = result == 0 ? 1 : 0;
    //return result;
}
struct ALUWordResult asr(WORD a, FLAG* tempFlags)
{
    // Respect sign bit of A.
    WORD hiBit = a & 0x8000 ? 1 : 0;
    return hiBit | (WORD)(a >> 1);
}

struct ALUWordResult rol(WORD a, FLAG* carry, FLAG* tempFlags)
{
    DWORD res = (a << 1) | ((*carry) ? 1 : 0);
    *carry  = res & 0x10000 ? 1 : 0;
    return (WORD) res;
}

struct ALUWordResult ror(WORD a, FLAG* carry, FLAG* tempFlags)
{
    FLAG cOut = a & 0x1 ? 1 : 0;
    DWORD res = (a >> 1) | ((*carry) ? 0x8000 : 0);
    *carry  = cOut;
    return res;
}

struct ALUWordResult ident(WORD a, FLAG* tempFlags)
{
    return a;
}

#endif