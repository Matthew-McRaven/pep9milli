#include "alufunc.h"
#include <stdint.h>

/*
 * Nonunary ALU operations performed on register words.
 */
WORD add_nocarry(WORD a, WORD b)
{
    return ((DWORD)a + (DWORD)b);
}
WORD sub_nocarry(WORD a, WORD b)
{
    return ((DWORD)a + (DWORD)~b);
}
WORD add_carry(WORD a, WORD b, FLAG carry)
{
    return ((DWORD)a + (DWORD)b + ((DWORD)carry>=1?1:0));
}
WORD sub_carry(WORD a, WORD b, FLAG carry)
{
    return ((DWORD)a + (DWORD)~b + ((DWORD)carry>=1?1:0));
}



WORD xor(WORD a, WORD b)
{
    return ((DWORD)a ^ (DWORD)b);
}
WORD and(WORD a, WORD b)
{
    return ((DWORD)a & (DWORD)b);
}
WORD or(WORD a, WORD b)
{
    return ((DWORD)a | (DWORD)b);
}
/*
 * Nonunary ALU operations performed on register words.
 */
WORD not(WORD a)
{
    return (WORD) ~a;
}
WORD neg(WORD a)
{
    return ((WORD)~a) + 1;
}
WORD asl(WORD a)
{
    return (WORD)(a << 1);
}
WORD asr(WORD a)
{
    // Respect sign bit of A.
    WORD hiBit = a & 0x8000 ? 1 : 0;
    return hiBit | (WORD)(a >> 1);
}

WORD rol(WORD a, FLAG* carry)
{
    DWORD res = (a << 1) | ((*carry) ? 1 : 0);
    *carry  = res & 0x10000 ? 1 : 0;
    return (WORD) res;
}

WORD ror(WORD a, FLAG* carry)
{
    FLAG cOut = a & 0x1 ? 1 : 0;
    DWORD res = (a >> 1) | ((*carry) ? 0x8000 : 0);
    *carry  = cOut;
    return res;
}

WORD ident(WORD a)
{
    return a;
}
/*
 * Nonunary ALU operations performed on register bytes.
 */