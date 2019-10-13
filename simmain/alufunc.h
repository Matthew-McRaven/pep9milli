#ifndef ALUFUNC_H
#define ALUFUNC_H

#include "defs.h"
/*
 * Nonunary ALU operations performed on register words.
 */
WORD add_nocarry(WORD a, WORD b, FLAG* tempFlags);
WORD sub_nocarry(WORD a, WORD b, FLAG* tempFlags);
WORD add_carry(WORD a, WORD b, FLAG carry, FLAG* tempFlags);
WORD sub_carry(WORD a, WORD b, FLAG carry, FLAG* tempFlags);

WORD xor(WORD a, WORD b, FLAG* tempFlags);
WORD and(WORD a, WORD b, FLAG* tempFlags);
WORD or(WORD a, WORD b, FLAG* tempFlags);

/*
 * Unary ALU operations performed on register words.
 */
WORD not(WORD a, FLAG* tempFlags);
WORD neg(WORD a, FLAG* tempFlags);
WORD asl(WORD a, FLAG* tempFlags);
WORD asr(WORD a, FLAG* tempFlags);
WORD rol(WORD a, FLAG* carry, FLAG* tempFlags);
WORD ror(WORD a, FLAG* carry, FLAG* tempFlags);
WORD ident(WORD , FLAG* tempFlagsa);

/*
 * Nonunary ALU operations performed on register bytes.
 */

#endif