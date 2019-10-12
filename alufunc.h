#ifndef ALUFUNC_H
#define ALUFUNC_H

#include "defs.h"
/*
 * Nonunary ALU operations performed on register words.
 */
WORD add_nocarry(WORD a, WORD b);
WORD sub_nocarry(WORD a, WORD b);
WORD add_carry(WORD a, WORD b, FLAG carry);
WORD sub_carry(WORD a, WORD b, FLAG carry);

WORD xor(WORD a, WORD b);
WORD and(WORD a, WORD b);
WORD or(WORD a, WORD b);

/*
 * Unary ALU operations performed on register words.
 */
WORD not(WORD a);
WORD neg(WORD a);
WORD asl(WORD a);
WORD asr(WORD a);
WORD rol(WORD a, FLAG* carry);
WORD ror(WORD a, FLAG* carry);
WORD ident(WORD a);

/*
 * Nonunary ALU operations performed on register bytes.
 */

#endif