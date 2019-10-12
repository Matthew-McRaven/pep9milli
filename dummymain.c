#include <stdio.h>
#include <assert.h>
#include "defs.h"
#include "alufunc.h"
#include "klee/klee.h"

WORD shiftin1(WORD a) {
    return add_nocarry(asl(a),1);
}
int main(int argv, char** argc)
{
    // Declare symbolic registers
    WORD regA, regB, regC;
    klee_make_symbolic(&regA, sizeof(regA), "A");
    klee_make_symbolic(&regB, sizeof(regB), "B");
    klee_make_symbolic(&regC, sizeof(regC), "C");

    // Assume that no registers start being equal.
    klee_assume(regA != regB); klee_assume(regA != regC);
    klee_assume(regB != regC);
    
    // Force B to always be positive.
    WORD (*func_arr[2])(WORD);
    func_arr[0] = &asr;
    func_arr[1] = &shiftin1;
    int which = 0;
    while(regA != 0x11) {
        regA = (*func_arr[which])(regA);
        if((regA == 0) | (regA == 1)) {
            which = 1;
        }
        else {
            which = 0;
        }
    }


    //printf("Hello world!\n");
    return 0;
}