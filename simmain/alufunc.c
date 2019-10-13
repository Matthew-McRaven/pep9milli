#include "alufunc.h"

#include <stdint.h>
#include "cpu.h"
/*
 * Nonunary ALU operations performed on register words.
 */
WORD add_nocarry(WORD a, WORD b, FLAG* tempFlags)
{
    return add_carry(a, b, 0, tempFlags);
}
WORD sub_nocarry(WORD a, WORD b, FLAG* tempFlags)
{
    return sub_carry(a, b, 1, tempFlags);
}
WORD add_carry(WORD a, WORD b, FLAG carry, FLAG* tempFlags)
{
    WORD result = ((DWORD)a + (DWORD)b + ((DWORD)carry >= 1 ? 1 : 0));
    tempFlags[N] = result & 0x8000 ? 1 : 0;
    tempFlags[Z] = result == 0 ? 1 : 0;
    return result;
}
WORD sub_carry(WORD a, WORD b, FLAG carry, FLAG* tempFlags)
{
    return add_carry(a, ~b, carry, tempFlags);
}



WORD xor(WORD a, WORD b, FLAG* tempFlags)
{
    return ((DWORD)a ^ (DWORD)b);
}
WORD and(WORD a, WORD b, FLAG* tempFlags)
{
    return ((DWORD)a & (DWORD)b);
}
WORD or(WORD a, WORD b, FLAG* tempFlags)
{
    return ((DWORD)a | (DWORD)b);
}
/*
 * Nonunary ALU operations performed on register words.
 */
WORD not(WORD a, FLAG* tempFlags)
{
    return (WORD) ~a;
}
WORD neg(WORD a, FLAG* tempFlags)
{
    return ((WORD)~a) + 1;
}
WORD asl(WORD a, FLAG* tempFlags)
{
    WORD result = (WORD)(a << 1);
    //tempFlags[N] = result & 0x8000 ? 1 : 0;
    //tempFlags[Z] = result == 0 ? 1 : 0;
    return result;
}
WORD asr(WORD a, FLAG* tempFlags)
{
    // Respect sign bit of A.
    WORD hiBit = a & 0x8000 ? 1 : 0;
    return hiBit | (WORD)(a >> 1);
}

WORD rol(WORD a, FLAG* carry, FLAG* tempFlags)
{
    DWORD res = (a << 1) | ((*carry) ? 1 : 0);
    *carry  = res & 0x10000 ? 1 : 0;
    return (WORD) res;
}

WORD ror(WORD a, FLAG* carry, FLAG* tempFlags)
{
    FLAG cOut = a & 0x1 ? 1 : 0;
    DWORD res = (a >> 1) | ((*carry) ? 0x8000 : 0);
    *carry  = cOut;
    return res;
}

WORD ident(WORD a, FLAG* tempFlags)
{
    return a;
}
/*
 * Nonunary ALU operations performed on register bytes.
 */