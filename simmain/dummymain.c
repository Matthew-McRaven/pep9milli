#include <assert.h>
#include <stdio.h>

#include "alufunc.h"
#include "cpu.h"
#include "defs.h"
#include "klee/klee.h"


WORD shiftin1(WORD a, FLAG* bits) {
    return add_nocarry(asl(a, bits),1, bits);
}
WORD asr1(WORD a, FLAG* bits) {
    return asr(a, bits);
}
int main(int argv, char** argc)
{
    // Declare symbolic registers
    struct CPU cpu;
    klee_make_symbolic(&cpu, sizeof(cpu), "CPU");
    cpu.PSNVCbits[Z] = 0;
    
    // Force B to always be positive.
    WORD (*func_arr[2])(WORD, FLAG*);
    func_arr[0] = &asr1;
    func_arr[1] = &shiftin1;
    int which = 0;
    while(getNamedRegisterWord(&cpu.regBank, A) != 0b11) {
        WORD regA = getNamedRegisterWord(&cpu.regBank, A);
        if((regA == 0) | (regA == 1)) {
            which = 1;
        }
        else {
            which = 0;
        }
        setNamedRegisterWord(&cpu.regBank, A, (*func_arr[which])(regA, cpu.PSNVCbits));
        //printf("A is %i",regA);
    }

    klee_assert(cpu.PSNVCbits[Z] == 0);
    //printf("Hello world!\n");
    return 0;
}