#include <stdio.h>
#include <assert.h>
#include "defs.h"
#include "alufunc.h"
#include "klee/klee.h"

int main(int argv, char** argc)
{
    // Declare symbolic registers
    WORD regA, regB, regC, regD;
    klee_make_symbolic(&regA, sizeof(regA), "A");
    klee_make_symbolic(&regB, sizeof(regB), "B");
    klee_make_symbolic(&regC, sizeof(regC), "C");
    klee_make_symbolic(&regD, sizeof(regD), "D");

    // Assume that no registers start being equal.
    klee_assume(regA != regB); klee_assume(regA != regC); klee_assume(regA != regD);
    klee_assume(regB != regC); klee_assume(regB != regD);
    klee_assume(regC != regD);
    
    // Create two temporary computation registers.
    WORD t1 = xor(regC, regB);
    WORD t2;
    klee_make_symbolic(&t1, sizeof(t1), "t1");
    klee_make_symbolic(&t2, sizeof(t2), "t2");

    // Branch depnding on the value of t1.
    if(t1 % 2) {
        t2 = add_nocarry(t1, regA);
    }
    else {
        t2 = add_nocarry(~t1, ~regA);
    }

    // Branch depnding on the value of t2.
    if(t2 % 2) {
        regA = or(t2, regD);
    }
    else {
        regA = or(t2, regD);
        regA = not(regA);
    }

    // Branch depnding on the value of a.
    if(regA % 2) {
        regB = add_nocarry(regA, 1);
    }
    else {
        regB = or(regA, 1);
    }
    
    // Assert that no two paths allow A and B to be equal.
    klee_assert(ident(regA) != ident(regB));
    printf("Hello world!\n");
    return 0;
}