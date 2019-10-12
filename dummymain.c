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
    WORD regA=0;
    klee_make_symbolic(&regA, sizeof(regA), "A");

    
    // Force B to always be positive.
    WORD (*func_arr[2])(WORD);
    func_arr[0] = &asr;
    func_arr[1] = &shiftin1;
    int which = 0;
    while(regA != 0b11) {
        if((regA == 0) | (regA == 1)) {
            which = 1;
        }
        else {
            which = 0;
        }
        regA = (*func_arr[which])(regA);
        //printf("A is %i",regA);
    }


    //printf("Hello world!\n");
    return 0;
}