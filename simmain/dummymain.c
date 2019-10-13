#include <assert.h>
#include <stdio.h>

#include "alufunc.h"
#include "cpu.h"
#include "defs.h"
#include "klee/klee.h"


BYTE shiftin1(BYTE a) {
    return byte_add_nocarry(byte_asl(a).result, 1).result;
}
BYTE asr1(BYTE a) {
    return byte_ror(a, 0).result;
}
int main(int argv, char** argc)
{
    // Declare symbolic registers
    struct CPU cpu;
    klee_make_symbolic(&cpu, sizeof(cpu), "CPU");
    cpu.PSNVCbits[Z] = 0;
    
    // Force B to always be positive.
    BYTE (*func_arr[2])(BYTE);
    func_arr[0] = &asr1;
    func_arr[1] = &shiftin1;
    int which = 0;
    while(getNamedRegisterByte(&cpu.regBank, A) != 0b11) {
        BYTE regA = getNamedRegisterByte(&cpu.regBank, A);
        if((regA == 0) | (regA == 1)) {
            which = 1;
        }
        else {
            which = 0;
        }
        setNamedRegisterByte(&cpu.regBank, A, (*func_arr[which])(regA));
        //printf("A is %i",regA);
    }
    //klee_assert(cpu.PSNVCbits[Z] == 0);

    return 0;
}