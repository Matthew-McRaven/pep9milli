#include <assert.h>
#include <stdio.h>

#include "alufunc.h"
#include "cpu.h"
#include "defs.h"
#include "klee/klee.h"


struct ALUWordResult shiftin1(WORD a) {
    return word_add_nocarry(word_asl(a).result, 1);
}
struct ALUWordResult asr1(WORD a) {
    return word_ror(a, 0);
}
int main(int argv, char** argc)
{
    // Declare symbolic registers
    struct CPU cpu;
    klee_make_symbolic(&cpu, sizeof(cpu), "CPU");
    cpu.PSNVCbits[Z] = 0;
    
    // Force B to always be positive.
    struct ALUWordResult (*func_arr[2])(WORD);
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
        struct ALUWordResult res = (*func_arr[which])(regA);
        setNamedRegisterWord(&cpu.regBank, A, res.result);
        setFlag(&cpu, Z, res.NZVC[Z] ? 1 : 0);
        //printf("A is %i",regA);
    }
    klee_assert(cpu.PSNVCbits[Z] == 0);

    return 0;
}